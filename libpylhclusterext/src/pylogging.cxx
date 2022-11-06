#include <lhcluster/logging.h>

#include <boost/python.hpp>

#include <sstream>

namespace LHClusterNS
{
namespace Python
{

    void initializeGenericLoggingFromSettingsString( const std::string& settingsString )
    {
        std::istringstream iss( settingsString );
        LHClusterLogging::InitializeGenericLogging( iss );
    }

    void ExportPythonLogging()
    {
        using namespace boost::python;

        void
        (*InitializeGenericLoggingFromFile)(
            const char* settingsFilePath ) = &LHClusterLogging::InitializeGenericLogging;

        def( "InitializeGenericLoggingFromFile", InitializeGenericLoggingFromFile);

        def( "InitializeGenericLoggingFromSettingsString",
             initializeGenericLoggingFromSettingsString );
    }
}
}
