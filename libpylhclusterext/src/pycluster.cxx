#include <lhcluster/cluster.h>

#include <boost/python.hpp>

namespace LHClusterNS
{
namespace Python
{
    void ExportPythonCluster()
    {
        using namespace boost::python;

        class_< Delay >(
            "Delay",
            init< Delay::rep >() )
        .def( "count", &Delay::count );

        class_< DelayMS >(
            "DelayMS",
            init< DelayMS::rep >() )
        .def( "count", &DelayMS::count );

        class_< LHCVersion >(
            "LHCVersion",
            init<>() )
        .def_readonly( "lhcmajor", &LHCVersion::lhcmajor )
        .def_readonly( "lhcminor", &LHCVersion::lhcminor )
        .def( self == self )
        .def( self != self )
        .def( self < self )
        .def( self <= self )
        .def( self > self )
        .def( self >= self );

        class_< LHCVersionFlags >(
            "LHCVersionFlags",
            init<>() )
        .def_readonly( "version", &LHCVersionFlags::version )
        .def_readonly( "flags", &LHCVersionFlags::flags )
        .def( self == self );

        enum_< std::uint64_t >(
            "MessageFlag" )
        .value( "Nil",              LHClusterNS::MessageFlag::None )
        // boost python only supports long values at the moment for some reason
        // .value( "Broker",           LHClusterNS::MessageFlag::Broker )
        .value( "Asynchronous",     LHClusterNS::MessageFlag::Asynchronous )
        .value( "ReceiptOnReceive", LHClusterNS::MessageFlag::ReceiptOnReceive )
        .value( "ReceiptOnSend",    LHClusterNS::MessageFlag::ReceiptOnSend )
        .value( "NotifyOnDeath",    LHClusterNS::MessageFlag::NotifyOnDeath )
        .value( "RetryOnDeath",     LHClusterNS::MessageFlag::RetryOnDeath )
        .value( "AllowRedirect",    LHClusterNS::MessageFlag::AllowRedirect )
        .value( "NotifyOnRedirect", LHClusterNS::MessageFlag::NotifyOnRedirect );
    }
}
}
