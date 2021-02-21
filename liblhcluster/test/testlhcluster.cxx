#include <lhcluster/logging.h>

#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    try
    {
        LHClusterNS::LHClusterLogging::InitializeGenericLogging( "./testlhcluster_logging.ini" );
    }
    catch( const std::exception& e )
    {
        ; 
    }

    return RUN_ALL_TESTS();
}
