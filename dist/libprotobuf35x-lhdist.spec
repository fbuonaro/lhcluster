%global __requires_exclude ^libjwt\\.so.*$

Name:       libprotobuf35x-lhdist
Version:    1
Release:    1
Summary:    custom libprotobuf 3.5.x lib
License:    See https://github.com/protocolbuffers/protobuf

%description
custom libprotobuf 3.5.x libwrapped in rpm

%prep
git clone -b 3.5.x https://github.com/protocolbuffers/protobuf.git protobuf35x

%build
cd protobuf35x
./autogen.sh
./configure --prefix=/usr
make
make check

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr
cd protobuf35x
make install prefix=%{buildroot}/usr
cd %{buildroot}
find ./ -type f -or -type l | grep usr | cut -d '.' -f 2- > %{buildroot}/../libprotobuf35x_install_files.txt
find ./ -type f -or -type l | grep usr | xargs -I{} chmod 777 {}

%files -f %{buildroot}/../libprotobuf35x_install_files.txt

%changelog
