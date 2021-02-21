##################################################################################
# STAGE 0 - base environemtn with first set of runtime dependencies
##################################################################################
FROM centos:centos7 as lhcluster-s0-base-env
LABEL lhcluster-stage-base-env="yes"
LABEL lhcluster-stage-build-env="no"
LABEL lhcluster-stage-build="no"
LABEL lhcluster-stage-test-env="no"
LABEL lhcluster-stage-main="no"

RUN yum -y --enablerepo=extras install epel-release && \
    yum -y install https://centos7.iuscommunity.org/ius-release.rpm &&\
    yum -y install \
        boost169-log \
        boost169-python3 \
        czmq \
        python36u \
        zeromq && \
    yum clean all

##################################################################################
# STAGE 1 - build tools and libraries needed to build lhcluster
##################################################################################
FROM lhcluster-s0-base-env as lhcluster-s1-build-env
LABEL lhcluster-stage-base-env="no"
LABEL lhcluster-stage-build-env="yes"
LABEL lhcluster-stage-build="no"
LABEL lhcluster-stage-test-env="no"
LABEL lhcluster-stage-main="no"

# for compiling and unit testing
RUN yum -y install \
        cmake3 \
        czmq-devel \
        boost169-devel \
        boost169-python3-devel \
        gcc \
        gcc-c++ \
        gtest-devel \
        make \
        python36u-devel \
        python36u-pip \
        zeromq-devel && \
    yum clean all && \
    pip3.6 install tox

# for protobuf3 since not available yet in centos
# remove once it is or put this into another base image
# since it appears to take up 90% of the image build time :/
# echo "/usr/local/lib" >> /etc/ld.so.conf.d/protobuf.conf && \
RUN yum -y install autoconf automake git libtool unzip && \
    git clone \
        -b 3.5.x https://github.com/protocolbuffers/protobuf.git \
        /protobuf35x && \
    cd /protobuf35x && \
    ./autogen.sh && ./configure --prefix=/usr && \
    make && make check && make install && \
    ldconfig
ENTRYPOINT [ "bash" ]

##################################################################################
# STAGE 2 - the lhcluster source and compiled binaries
##################################################################################
FROM lhcluster-s1-build-env as lhcluster-s2-build
LABEL lhcluster-stage-base-env="no"
LABEL lhcluster-stage-build-env="no"
LABEL lhcluster-stage-build="yes"
LABEL lhcluster-stage-test-env="no"
LABEL lhcluster-stage-main="no"

ADD . /lhcluster
RUN cd /lhcluster && \
    mkdir ./build && \
    cd ./build && \
    cmake3 \
        -DBOOST_INCLUDEDIR=/usr/include/boost169 \
        -DBOOST_LIBRARYDIR=/usr/lib64/boost169 \
        -DCMAKE_BUILD_TYPE=Release \
        ../ && \
    make && \
    make test

##################################################################################
# STAGE 3 - the base image with additional built runtime dependencies, lhcluster 
#           binaries and test binaries needed for running integration tests
#           includes everything from build-env
##################################################################################
FROM lhcluster-s2-build as lhcluster-s3-test-env
LABEL lhcluster-stage-base-env="no"
LABEL lhcluster-stage-build-env="no"
LABEL lhcluster-stage-build="no"
LABEL lhcluster-stage-test-env="yes"
LABEL lhcluster-stage-main="no"

RUN cd /lhcluster/build && \
    make install-lhcluster && \
    export PYLHC_TEST_ENV=1 && make install-pylhcluster
ENTRYPOINT [ "python3.6", "-m", "pylhcluster" ]

##################################################################################
# STAGE 4 - the base image with additional built runtime dependencies and 
#           lhcluster binaries includes nothing from build-env
##################################################################################
FROM lhcluster-s0-base-env as lhcluster-s4-main-env
LABEL lhcluster-stage-base-env="no"
LABEL lhcluster-stage-build-env="no"
LABEL lhcluster-stage-build="no"
LABEL lhcluster-stage-test-env="no"
LABEL lhcluster-stage-main="yes"

COPY --from=lhcluster-s2-build /usr/ /usr/
COPY --from=lhcluster-s2-build /lhcluster/ /lhcluster/
RUN cd /lhcluster/build && \
    make install-lhcluster && \
    make install-pylhcluster && \
    cd / && \
    rm -rf /lhcluster
ENTRYPOINT [ "python3.6", "-m", "pylhcluster" ]
