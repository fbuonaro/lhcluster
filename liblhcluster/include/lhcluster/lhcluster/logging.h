#ifndef __LHCLUSTER_LOGGING_H__
#define __LHCLUSTER_LOGGING_H__

#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/trivial.hpp>

#include <istream>

#define LHCLUSTER_LOGGER_CHANNEL_NAME_GENERIC "Generic"
#define LHCLUSTER_LOGGER_CHANNEL_NAME_STATS   "Stats"

#define LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTOR     "Actor"     // id of broker, worker, etc
#define LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTION    "Action"    // Broker.handle.., etc
#define LHCLUSTER_LOGGER_RECORD_ATTR_NAME_SCOPE     "Scope"
#define LHCLUSTER_LOGGER_RECORD_ATTR_NAME_TAG       "Tag"       // request type in some contexts

namespace LHClusterNS
{
    enum class SeverityLevel : std::uint8_t
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    typedef
        boost::log::sources::severity_channel_logger_mt< SeverityLevel, std::string >
        LoggerType;

    class LHClusterLogging
    {
        public:
            static void InitializeGenericLogging( const char* settingsFilePath );
            static void InitializeGenericLogging( std::istream& is );

        private:
            LHClusterLogging()
            {
            }
    };
}

// Singleton logger
BOOST_LOG_GLOBAL_LOGGER( LHClusterLoggerGeneric, LHClusterNS::LoggerType );
// TODO - another logger for stats LHClusterLoggerStats

// Custom Attributes
// Meant to be used in one scope per thread level
// i.e. at the topmost handler
BOOST_LOG_ATTRIBUTE_KEYWORD(
    LHClusterAttrActor,  LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTOR,  std::string );
BOOST_LOG_ATTRIBUTE_KEYWORD(
    LHClusterAttrAction, LHCLUSTER_LOGGER_RECORD_ATTR_NAME_ACTION, std::string );


#endif
