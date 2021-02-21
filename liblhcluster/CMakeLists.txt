cmake_minimum_required( VERSION 3.10 )

project( liblhcluster )

# general flags for building successfully
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_DYN_LINK" )

# for debugging
set( LLHC_CXX_FLAGS_DBG "${LLHC_CXX_FLAGS_DBG} -Wall -Wextra -W" )
set( LLHC_CXX_FLAGS_DBG "${LLHC_CXX_FLAGS_DBG} -DDEBUG" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLHC_CXX_FLAGS_DBG}" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${LLHC_CXX_FLAGS_DBG}" )

##############################################################
# library
##############################################################

set( LIB_SRC_DIR "src" )
set( LIB_INC_DIR "include" )

# pull in boost log
find_package( Boost 1.69 COMPONENTS log REQUIRED )
if( NOT Boost_FOUND )
    message( FATAL_ERROR "boost log could not be found" )
endif( NOT Boost_FOUND )

# pull in protobuf
find_package( Protobuf 3.0 REQUIRED )
if( NOT PROTOBUF_FOUND )
    message( FATAL_ERROR "protobuf could not be found" )
endif( NOT PROTOBUF_FOUND )

# pull in gtest
find_package( GTest REQUIRED )
if( NOT GTEST_FOUND )
    message( FATAL_ERROR "gtest could not be found" )
endif( NOT GTEST_FOUND )

# generate protobuf source and headers from prototype
PROTOBUF_GENERATE_CPP(
    PROTO_SRCS PROTO_HDRS "./proto/zmqframetype.proto" )

add_library( lhcluster SHARED
    "${LIB_SRC_DIR}/broker.cxx"
    "${LIB_SRC_DIR}/brokerrequesttypecontext.cxx"
    "${LIB_SRC_DIR}/client.cxx"
	"${LIB_SRC_DIR}/clientproxy.cxx"
    "${LIB_SRC_DIR}/clientrequestbuffer.cxx"
    "${LIB_SRC_DIR}/clientrequestcontext.cxx"
	"${LIB_SRC_DIR}/endpoint.cxx"
    "${LIB_SRC_DIR}/ibrokerfactory.cxx"
    "${LIB_SRC_DIR}/iclientfactory.cxx"
    "${LIB_SRC_DIR}/iclientproxyfactory.cxx"
    "${LIB_SRC_DIR}/inotifybrokerfactory.cxx"
    "${LIB_SRC_DIR}/inotifypublisherfactory.cxx"
    "${LIB_SRC_DIR}/inotifysubscriberfactory.cxx"
	"${LIB_SRC_DIR}/interfaces.cxx"
    "${LIB_SRC_DIR}/iworkerfactory.cxx"
    "${LIB_SRC_DIR}/logging.cxx"
    "${LIB_SRC_DIR}/notifybroker.cxx"
    "${LIB_SRC_DIR}/notifypublisher.cxx"
    "${LIB_SRC_DIR}/notifysubscriber.cxx"
	"${LIB_SRC_DIR}/requestprocessor.cxx"
	"${LIB_SRC_DIR}/requestresponse.cxx"
	"${LIB_SRC_DIR}/requeststate.cxx"
    "${LIB_SRC_DIR}/messagestats.cxx"
    "${LIB_SRC_DIR}/utils.cxx"
	"${LIB_SRC_DIR}/worker.cxx"
    "${LIB_SRC_DIR}/workerinfo.cxx"
    "${LIB_SRC_DIR}/workerinfolookup.cxx"
    "${LIB_SRC_DIR}/workerrequesttypecontext.cxx"
    "${LIB_SRC_DIR}/zmqframehandler.cxx"
	"${LIB_SRC_DIR}/endpointbuilder.cxx"
    "${LIB_SRC_DIR}/brokerparameters.cxx"
    "${LIB_SRC_DIR}/brokerparametersbuilder.cxx"
    "${LIB_SRC_DIR}/clientparameters.cxx"
    "${LIB_SRC_DIR}/clientparametersbuilder.cxx"
    "${LIB_SRC_DIR}/clientproxyparameters.cxx"
    "${LIB_SRC_DIR}/clientproxyparametersbuilder.cxx"
    "${LIB_SRC_DIR}/notifybrokerparameters.cxx"
    "${LIB_SRC_DIR}/notifybrokerparametersbuilder.cxx"
    "${LIB_SRC_DIR}/notifybrokerparameters.cxx"
    "${LIB_SRC_DIR}/notifybrokerparametersbuilder.cxx"
    "${LIB_SRC_DIR}/notifypublisherparameters.cxx"
    "${LIB_SRC_DIR}/notifypublisherparametersbuilder.cxx"
    "${LIB_SRC_DIR}/notifysubscriberparameters.cxx"
    "${LIB_SRC_DIR}/notifysubscriberparametersbuilder.cxx"
    "${LIB_SRC_DIR}/workerparameters.cxx"
    "${LIB_SRC_DIR}/workerparametersbuilder.cxx"
    ${PROTO_SRCS} )

target_link_libraries( lhcluster
    PUBLIC
        czmq
        zmq
        "${Boost_LIBRARIES}"
    PRIVATE
        pthread protobuf )

target_include_directories( lhcluster
    PUBLIC
        "${Boost_INCLUDE_DIR}"
        "${LIB_INC_DIR}/lhcluster"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${LIB_INC_DIR}/lhcluster_impl"
        "${LIB_INC_DIR}/lhcluster_stats" )

##############################################################
# unit tests
##############################################################

set( TST_SRC_DIR "./test" )

enable_testing()

add_executable( testlhcluster
    "${TST_SRC_DIR}/test.cxx"
    "${TST_SRC_DIR}/testbroker.cxx"
    "${TST_SRC_DIR}/testclient.cxx"
    "${TST_SRC_DIR}/testclientproxy.cxx"
    "${TST_SRC_DIR}/teste2enotifypubsub.cxx"
    "${TST_SRC_DIR}/teste2eclientworker.cxx"
    "${TST_SRC_DIR}/testlhcluster.cxx"
    "${TST_SRC_DIR}/testnotifypublisher.cxx"
    "${TST_SRC_DIR}/testnotifysubscriber.cxx"
    "${TST_SRC_DIR}/testparameters.cxx"
    "${TST_SRC_DIR}/testrequestprocessor.cxx"
    "${TST_SRC_DIR}/testworker.cxx"
    "${TST_SRC_DIR}/testzmqmessagefactoryutils.cxx"
    "${TST_SRC_DIR}/testzmqsocketinterfaces.cxx"
    "${TST_SRC_DIR}/testzmqsocketutils.cxx" )

target_link_libraries( testlhcluster
    PRIVATE
        "${GTEST_BOTH_LIBRARIES}"
        lhcluster )

target_include_directories( testlhcluster
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${GTEST_INCLUDE_DIRS}"
        "${LIB_INC_DIR}/lhcluster"
        "${LIB_INC_DIR}/lhcluster_impl"
        "${LIB_INC_DIR}/lhcluster_stats" )

include( GoogleTest )
gtest_add_tests( TARGET testlhcluster )

##############################################################
# installation
##############################################################

# library
install( TARGETS lhcluster
         LIBRARY
            DESTINATION lib
            COMPONENT lhcluster )

# include directory
install( DIRECTORY "${LIB_INC_DIR}/lhcluster/lhcluster"
         DESTINATION include
         COMPONENT lhcluster
         FILES_MATCHING PATTERN "*.h" )

ADD_CUSTOM_TARGET( install-lhcluster
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=lhcluster"
                   -P "cmake_install.cmake" )
