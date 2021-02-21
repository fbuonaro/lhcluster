#include <lhcluster/logging.h>

#include <fstream>
#include <istream>
#include <stdexcept>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/keywords/channel.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>

namespace LHClusterNS
{
    void LHClusterLogging::InitializeGenericLogging( const char* settingsFilePath )
    {
        if( settingsFilePath == nullptr )
            throw std::runtime_error( "no settings file path provided" );

        std::ifstream settingsFile( settingsFilePath );

        LHClusterLogging::InitializeGenericLogging( settingsFile );
    }

    void LHClusterLogging::InitializeGenericLogging( std::istream& is )
    {
        boost::log::add_common_attributes();

        // Adds Scope attribute to all logs to allow for stack traces
        boost::log::core::get()->add_thread_attribute(
            LHCLUSTER_LOGGER_RECORD_ATTR_NAME_SCOPE,
            boost::log::attributes::named_scope());

        boost::log::init_from_stream( is );
    } 
}

BOOST_LOG_GLOBAL_LOGGER_INIT( LHClusterLoggerGeneric, LHClusterNS::LoggerType )
{
    LHClusterNS::LoggerType logger(
        boost::log::keywords::channel = LHCLUSTER_LOGGER_CHANNEL_NAME_GENERIC,
        boost::log::keywords::severity = LHClusterNS::SeverityLevel::Trace );

    return logger;
}
