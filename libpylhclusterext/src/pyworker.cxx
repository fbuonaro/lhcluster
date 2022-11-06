#include <pylhclusterext/pygilguard.h>
#include <pylhclusterext/pyzmqaddin.h>

#include <lhcluster/worker.h>
#include <lhcluster/workerfactory.h>

#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/shared_ptr.hpp>

namespace LHClusterNS
{
    namespace Python
    {
        namespace
        {
            // Python interface for returning results from the processor
            // Better RAII and python friendlier
            class PyProcessorResult
            {
            public:
                PyProcessorResult()
                    : status( RequestStatus::None )
                    , pyZMQMessage()
                {
#ifdef PYDEBUG
                    clusterLogSetAction( "PyProcessorResult.CTOR" )
                        clusterLog( "PyProcessorResult.CTOR: begin" )
#endif
                }

#ifdef PYDEBUG
                ~PyProcessorResult()
                {
                    clusterLogSetAction( "PyProcessorResult.DTOR" )
                        clusterLog( "PyProcessorResult.DTOR: begin" )
                }

                PyProcessorResult( const PyProcessorResult& other )
                    : status( other.status )
                    , pyZMQMessage( other.pyZMQMessage )
                {
                    clusterLogSetAction( "PyProcessorResult.CCTOR" )
                        clusterLog( "PyProcessorResult.CCTOR: begin" )
                }

                PyProcessorResult( PyProcessorResult&& other )
                    : status( other.status )
                    , pyZMQMessage( std::move( other.pyZMQMessage ) )
                {
                    clusterLogSetAction( "PyProcessorResult.MCTOR" )
                        clusterLog( "PyProcessorResult.MCTOR: begin" )
                }
#endif

                RequestStatus GetStatus() const
                {
                    return status;
                }

                void SetStatus( RequestStatus _status )
                {
                    status = _status;
                }

                PyZMQMessage& GetPyZMQMessage()
                {
                    return pyZMQMessage;
                }

            private:
                RequestStatus status;
                PyZMQMessage pyZMQMessage;
            };

            // Specialized interface for python equivalent of IRequestHandler
            // The IRequestHandler handler interface was not python friendly
            class PyIRequestHandler
            {
            public:
#ifdef PYDEBUG
                PyIRequestHandler()
                {
                    clusterLogSetAction( "PyIRequestHandler.CTOR" )
                        clusterLog( "PyIRequestHandler.CTOR: begin" )
                }

                PyIRequestHandler( const PyIRequestHandler& other )
                {
                    clusterLogSetAction( "PyIRequestHandler.CCTOR" )
                        clusterLog( "PyIRequestHandler.CCTOR: begin" )
                }
#else
                PyIRequestHandler()
                {
                }
#endif

                virtual ~PyIRequestHandler()
                {
#ifdef PYDEBUG
                    clusterLogSetAction( "PyIRequestHandler.DTOR" )
                        clusterLog( "PyIRequestHandler.DTOR: begin" )
#endif
                }

                virtual PyProcessorResult PyProcess(
                    const LHCVersionFlags& vfs,
                    RequestType requestType,
                    RequestId requestId,
                    const PyZMQMessage& pyMessage ) = 0;

                virtual const std::vector< RequestType >& PySupportedRequestTypes() const = 0;
            };

            // Wrapper around Python Implementation of IRequestHandler
            // Implements the C++/Python boundary logic in the implementation
            // of the IRequestHandler interface (aquire GIL, etc)
            // Dispatches to the python object for the equivalent Py* logic
            // It does not need to implement PyIRequestHandler but I did it anyway
            class PyIRequestHandlerWrapper : public PyIRequestHandler, public IRequestHandler
            {
            public:
                PyIRequestHandlerWrapper( boost::python::object _pyIRequestHandlerImpl )
                    : PyIRequestHandler()
                    , IRequestHandler()
                    , pyIRequestHandlerImpl( new boost::python::object( _pyIRequestHandlerImpl ) )
                    , getPyIRequestHandlerImpl( *pyIRequestHandlerImpl )
                {
#ifdef PYDEBUG
                    clusterLogSetAction( "PyIRequestHandlerWrapper.CTOR" )
                        clusterLog( "PyIRequestHandlerWrapper.CTOR: begin" )
#endif
                        if ( !getPyIRequestHandlerImpl.check() )
                        {
                            throw std::runtime_error(
                                "handler not an instance of PyIRequestHandler" );
                        }
                }

                ~PyIRequestHandlerWrapper()
                {
#ifdef PYDEBUG
                    clusterLog( "PyIRequestHandlerWrapper.DTOR: begin" )
#endif
                        // The entire purpose of this destructor and of storing
                        // the boost python object in a pointer is to ensure that the destruction
                        // of the boost python object only occurs when the GIL is locked
                        GIL getGIL;
#ifdef PYDEBUG
                    clusterLog( "PyIRequestHandlerWrapper.DTOR: GIL acquired" )
#endif
                        pyIRequestHandlerImpl.reset();
#ifdef PYDEBUG
                    clusterLog( "PyIRequestHandlerWrapper.DTOR: end" )
#endif
                }

                IRequestHandler::ProcessorResult Process(
                    const LHCVersionFlags& vfs,
                    RequestType requestType,
                    RequestId requestId,
                    ZMQMessage* message )
                {
#ifdef PYDEBUG
                    clusterLogSetAction( "PyIRequestHandlerWrapper.Process" )
                        clusterLog( "PyIRequestHandlerWrapper.Process: begin" )
                        clusterLog( "PyIRequestHandlerWrapper.Process: acquiring GIL" )
#endif
                        GIL getGIL;
#ifdef PYDEBUG
                    clusterLog( "PyIRequestHandlerWrapper.Process: GIL acquired" )
#endif
                        PyZMQMessage pyZMQMessage( message );
                    PyProcessorResult pyRet = PyProcess(
                        vfs, requestType, requestId, pyZMQMessage );
                    IRequestHandler::ProcessorResult ret;

                    ret.first = pyRet.GetStatus();

                    if ( pyRet.GetPyZMQMessage().Owning() )
                        ret.second = zmsg_dup( pyRet.GetPyZMQMessage().GetUnderlyingMessage() );
                    else
                        ret.second = pyRet.GetPyZMQMessage().GetUnderlyingMessage();

                    return ret;
                }

                PyProcessorResult PyProcess(
                    const LHCVersionFlags& vfs,
                    RequestType requestType,
                    RequestId requestId,
                    const PyZMQMessage& pyZMQMessage )
                {
#ifdef PYDEBUG
                    clusterLogSetAction( "PyIRequestHandlerWrapper.PyProcess" )
                        clusterLog( "PyIRequestHandlerWrapper.Process: begin" )
#endif
                        // Should be called from IRequestHandler::Process which should already
                        // have aquired the GIL
                        PyIRequestHandler& handler( getPyIRequestHandlerImpl() );
                    return handler.PyProcess( vfs, requestType, requestId, pyZMQMessage );
                }

                // TODO - get rid of this, pass to factory/worker ctor instead
                const std::vector< RequestType >& SupportedRequestTypes() const
                {
                    GIL getGIL;
                    return PySupportedRequestTypes();
                }

                const std::vector< RequestType >& PySupportedRequestTypes() const
                {
                    PyIRequestHandler& handler( getPyIRequestHandlerImpl() );
                    return handler.PySupportedRequestTypes();
                }

            private:
                std::unique_ptr< boost::python::object > pyIRequestHandlerImpl;
                // TODO - not 100% sure if this should be a reference or not
                boost::python::extract< PyIRequestHandler& > getPyIRequestHandlerImpl;
            };


            // Wrapper around PyIRequestHandler interface that is eventually exposed to Python
            class PyIRequestHandlerExposed :
                public PyIRequestHandler, public boost::python::wrapper< PyIRequestHandler >
            {
            public:
                PyProcessorResult PyProcess(
                    const LHCVersionFlags& vfs,
                    RequestType requestType,
                    RequestId requestId,
                    const PyZMQMessage& pyZMQMessage )
                {
#ifdef PYDEBUG
                    clusterLogSetAction( "PyIRequestHandlerExposed.PyProcess" )
                        clusterLog( "PyIRequestHandlerExposed.PyProcess: begin" )
#endif
                        return this->get_override( "PyProcess" )(
                            vfs, requestType, requestId, pyZMQMessage );
                }

                const std::vector< RequestType >& PySupportedRequestTypes() const
                {
                    return this->get_override( "PySupportedRequestTypes" )( );
                }
            };

            boost::shared_ptr< Worker > proxyCreateViaFactory(
                WorkerFactory& workerFactory,
                const WorkerParameters& p,
                boost::python::object pythonIRequestHandlerImpl )
            {
                // TODO - update to make_unique, currently will leak memory every time
                //        someone provides a bad IRequestHandler implementation and the ctor throws
                // void (&gilDeleter)( IRequestHandler* ) = DeleteWithGIL< IRequestHandler >;
                return boost::shared_ptr< Worker >(
                    new Worker(
                        workerFactory.CreateConcrete(
                            p,
                            std::unique_ptr< IRequestHandler >(
                                new PyIRequestHandlerWrapper( pythonIRequestHandlerImpl ) ) ) ),
                    DeleteWithNoGIL< Worker > );
            }
        }

        void ExportPythonWorker()
        {
            using namespace boost::python;

            class_< std::vector< RequestType > >( "RequestTypeList" )
                .def( vector_indexing_suite< std::vector< RequestType > >() );

            class_< PyProcessorResult >( "PyProcessorResult", init<>() )
                .def( "SetStatus", &PyProcessorResult::SetStatus,
                    return_self<>() )
                .def( "GetStatus", &PyProcessorResult::GetStatus )
                .def( "GetPyZMQMessage", &PyProcessorResult::GetPyZMQMessage,
                    return_internal_reference<>() );

            class_< PyIRequestHandlerExposed, boost::noncopyable >(
                "PyIRequestHandler", init<>() )
                .def( "PyProcess",
                    pure_virtual( &PyIRequestHandlerExposed::PyProcess ) )
                .def( "PySupportedRequestTypes",
                    pure_virtual( &PyIRequestHandlerExposed::PySupportedRequestTypes ),
                    return_value_policy< copy_const_reference >() );

            class_<
                Worker,
                boost::noncopyable, boost::shared_ptr< Worker > >(
                    "Worker", no_init )
                .def( "__init__", make_constructor( &proxyCreateViaFactory ) )
                .def( "Start", with< noGIL >( &Worker::Start ) )
                .def( "StartAsPrimary", with< noGIL >( &Worker::StartAsPrimary ) )
                .def( "Stop", with< noGIL >( &Worker::Stop ) );
        }
    }
}
