#ifndef __LHCLUSTER_IMPL_DEBUG_H__
#define __LHCLUSTER_IMPL_DEBUG_H__

#include <lhcluster/logging.h>

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>

// #define DEBUG

#define checkRet( ret, success, msg ) \
    if( ret != success ) \
        BOOST_LOG_SEV( LHClusterLoggerGeneric::get(), LHClusterNS::SeverityLevel::Warning )<<msg;

#define checkRetAndReturn( ret, success, msg ) \
    if( ret != success ) \
    { \
        BOOST_LOG_SEV( LHClusterLoggerGeneric::get(), LHClusterNS::SeverityLevel::Error )<<msg; \
        return ret; \
    }

// call at top of every function
#define clusterLogSetScope( scopeName ) \
    BOOST_LOG_NAMED_SCOPE( scopeName );

// only call once at the topmost level
#define clusterLogSetActor( actor ) \
    BOOST_LOG_SCOPED_THREAD_TAG( LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTOR, actor );

#define clusterLogSetAction( action ) \
    clusterLogSetScope( action ) \
    BOOST_LOG_SCOPED_THREAD_TAG( LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTION, action );

#define clusterLogSetTag( tag ) \
    BOOST_LOG_SCOPED_THREAD_TAG( "Tag", tag );

#define clusterLog( msg ) \
    BOOST_LOG( LHClusterLoggerGeneric::get() ) << msg;

#define clusterLogWithSeverity( severity, msg ) \
    BOOST_LOG_SEV( LHClusterLoggerGeneric::get(), severity ) << msg;

// TODO - transition to separate loggers for different channels
#define clusterLogToChannel( logToChannel, msg ) \
    BOOST_LOG_CHANNEL( LHClusterLoggerGeneric::get(), logToChannel ) << msg;

#define clusterLogToChannelWithSeverity( logToChannel, severity, msg ) \
    BOOST_LOG_CHANNEL_SEV( LHClusterLoggerGeneric::get(), logToChannel, severity ) << msg;

#endif
