#include <pylhcluster/pyzmqaddin.h>

namespace LHClusterNS
{
namespace Python
{
        void ExportPythonZMQAddin()
        {
            using namespace boost::python;

            opaque< LHClusterNS::ZMQMessage >();

            class_< PyZMQMessage >(
                "PyZMQMessage", init<>() )
                .def( init< ZMQMessage* >() )
                .def( "Duplicate", &PyZMQMessage::Duplicate )
                .def( "AppendBytes", &PyZMQMessage::AppendBytes )
                .def( "PrependBytes", &PyZMQMessage::PrependBytes )
                .def( "PopBytesAtBack", &PyZMQMessage::PopBytesAtBack )
                .def( "PopBytesAtFront", &PyZMQMessage::PopBytesAtFront )
                .def( "BytesAtBack",
                      &PyZMQMessage::BytesAtBack,
                      with_custodian_and_ward_postcall< 0, 1 >() )
                .def( "BytesAtFront",
                      &PyZMQMessage::BytesAtFront,
                      with_custodian_and_ward_postcall< 0, 1 >() )
                .def( "NextBytes",
                      &PyZMQMessage::NextBytes,
                      with_custodian_and_ward_postcall< 0, 1 >() )
                .def( "TotalNumberOfFrames", &PyZMQMessage::TotalNumberOfFrames )
                .def( "TotalSizeOfMessage", &PyZMQMessage::TotalSizeOfMessage )
                .def( "Relinquish", &PyZMQMessage::Relinquish );
        }
}
}
