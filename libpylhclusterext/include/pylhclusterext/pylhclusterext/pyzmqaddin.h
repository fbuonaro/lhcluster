#ifndef __LHCLUSTER_PYTHON_PYZMQADDIN_H__
#define __LHCLUSTER_PYTHON_PYZMQADDIN_H__

#include <lhcluster/zmqaddin.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include <boost/python.hpp>
#pragma GCC diagnostic push

#include <stdexcept>

BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID( LHClusterNS::ZMQMessage )

namespace LHClusterNS
{
    namespace Python
    {
        namespace
        {
            void deleteZMQFrame( ZMQFrame* _zmqFrame )
            {
                ZMQFrame* zmqFrame = _zmqFrame;
                zframe_destroy( &zmqFrame );
            }

        }

        // Not random access, frames of bytes must be access sequentially from start to front
        class PyZMQMessage
        {
        public:
            // CTORS, DTORS
            ~PyZMQMessage()
            {
                if ( owning && zmqMessage )
                    zmsg_destroy( &zmqMessage );
            }

            PyZMQMessage()
                : zmqMessage( zmsg_new() )
                , owning( true )
            {
                if ( !zmqMessage )
                    throw std::runtime_error( "zmqMessage failed to be created" );
            }

            // take ownership
            PyZMQMessage( ZMQMessage** lpZMQMessage )
                : zmqMessage( *lpZMQMessage )
                , owning( true )
            {
                *lpZMQMessage = nullptr;

                if ( !zmqMessage )
                    throw std::runtime_error( "zmqMessage is not valid" );
            }

            // take ownership when true else just a view of
            PyZMQMessage( ZMQMessage* _zmqMessage, bool copyMessage )
                : zmqMessage( copyMessage ? zmsg_dup( _zmqMessage ) : _zmqMessage )
                , owning( copyMessage )
            {
                if ( !zmqMessage )
                    throw std::runtime_error( "zmqMessage is not valid" );
            }

            PyZMQMessage( ZMQMessage* _zmqMessage )
                : PyZMQMessage( _zmqMessage, false )
            {
            }

            // never share ownership but may share view of another message
            PyZMQMessage( const PyZMQMessage& other )
                : zmqMessage( other.owning ? zmsg_dup( other.zmqMessage ) : other.zmqMessage )
                , owning( other.owning )
            {
            }

            PyZMQMessage( PyZMQMessage&& other )
                : zmqMessage( other.zmqMessage )
                , owning( other.owning )
            {
                other.zmqMessage = nullptr;
                other.owning = false;
            }

            PyZMQMessage& operator=( PyZMQMessage other )
            {
                swap( *this, other );
                return *this;
            }

            // METHODS
            friend void swap( PyZMQMessage& a, PyZMQMessage& b )
            {
                using std::swap;

                swap( a.zmqMessage, b.zmqMessage );
                swap( a.owning, b.owning );
            }

            PyZMQMessage Duplicate()
            {
                return PyZMQMessage( zmqMessage, true );
            }

            // No link between object and object passed in
            // The bytes are copied directly into an internal buffer
            // NOTE - a None and empty bytes will result in the same zero-length
            // block of memory being added to the message and
            // will both result in an empty bytes object on the receiving side
            void AppendBytes( boost::python::object object )
            {
                PyObject* pyObject = object.ptr();
                if ( pyObject == Py_None )
                {
                    zmsg_addmem( zmqMessage, nullptr, 0 );
                }
                else if ( PyObject_CheckBuffer( pyObject ) )
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
                    Py_buffer pyBuffer = { 0 };
#pragma GCC diagnostic pop
                    // PyBUF_SIMPLE -> contiguous C-style array
                    int rc = PyObject_GetBuffer( pyObject, &pyBuffer, PyBUF_SIMPLE );
                    if ( rc )
                    {
                        throw std::runtime_error(
                            "AppendBytes failed to read object bytes" );
                    }
                    else
                    {
                        zmsg_addmem( zmqMessage, pyBuffer.buf, pyBuffer.len );
                        PyBuffer_Release( &pyBuffer );
                    }
                }
                else
                {
                    throw std::runtime_error(
                        "AppendBytes expects an object supporting buffer protocol or None" );
                }
            }

            // No link between object and object passed in
            // The bytes are copied directly into an internal buffer
            void PrependBytes( boost::python::object object )
            {
                PyObject* pyObject = object.ptr();
                if ( pyObject == Py_None )
                {
                    zmsg_pushmem( zmqMessage, nullptr, 0 );
                }
                else if ( PyObject_CheckBuffer( pyObject ) )
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
                    Py_buffer pyBuffer = { 0 };
#pragma GCC diagnostic pop
                    // PyBUF_SIMPLE -> contiguous C-style array
                    int rc = PyObject_GetBuffer( pyObject, &pyBuffer, PyBUF_SIMPLE );
                    if ( rc )
                    {
                        throw std::runtime_error(
                            "PrependBytes failed to read object bytes" );
                    }
                    else
                    {
                        zmsg_pushmem( zmqMessage, pyBuffer.buf, pyBuffer.len );
                        PyBuffer_Release( &pyBuffer );
                    }
                }
                else
                {
                    throw std::runtime_error(
                        "PrependBytes expects an object supporting buffer protocol or None" );
                }
            }

            // No relationship between returned object and message
            // Will return None if there
            // are no more frames
            boost::python::object PopBytesAtBack()
            {
                ZMQFrame* frame = zmsg_last( zmqMessage );

                if ( frame )
                {
                    zmsg_remove( zmqMessage, frame );

                    // TODO - make_unique
                    std::unique_ptr< ZMQFrame, decltype( &deleteZMQFrame ) > frameScopeGuard(
                        frame,
                        deleteZMQFrame );
                    size_t frameSize = zframe_size( frame );

                    if ( frameSize )
                    {
                        return boost::python::object(
                            boost::python::handle<>(
                                PyByteArray_FromStringAndSize(
                                    reinterpret_cast<const char*>( zframe_data( frame ) ),
                                    zframe_size( frame ) ) ) );
                    }
                    else // empty bytes in a new object, seems inefficient but
                    {    // if I do not do this then I overload the meaning of None
                        return boost::python::object(
                            boost::python::handle<>(
                                PyByteArray_FromStringAndSize( nullptr, 0 ) ) );
                    }
                }
                else
                    return boost::python::object();
            }

            // No relationship between returned object and message
            // Will return None if there are no more frames
            boost::python::object PopBytesAtFront()
            {
                ZMQFrame* frame = zmsg_pop( zmqMessage );

                if ( frame )
                {
                    // TODO - make_unique
                    std::unique_ptr< ZMQFrame, decltype( &deleteZMQFrame ) > frameScopeGuard(
                        frame,
                        deleteZMQFrame );

                    size_t frameSize = zframe_size( frame );

                    if ( frameSize )
                    {
                        return boost::python::object(
                            boost::python::handle<>(
                                PyByteArray_FromStringAndSize(
                                    reinterpret_cast<const char*>( zframe_data( frame ) ),
                                    zframe_size( frame ) ) ) );
                    }
                    else // empty bytes in a new object, seems inefficient but
                    {    // if I do not do this then I overload the meaning of None
                        return boost::python::object(
                            boost::python::handle<>(
                                PyByteArray_FromStringAndSize( nullptr, 0 ) ) );
                    }
                }
                else
                    return boost::python::object();

            }

            // Message must stay alive as long as the view is alive
            // Will return None if BytesAtFront was not called or there
            // are no more frames
            boost::python::object BytesAtBack()
            {
                ZMQFrame* frame = zmsg_last( zmqMessage );

                if ( frame )
                {
                    size_t frameSize = zframe_size( frame );
                    if ( frameSize )
                    {
                        return boost::python::object(
                            boost::python::handle<>(
                                PyMemoryView_FromMemory(
                                    reinterpret_cast<char*>( zframe_data( frame ) ),
                                    zframe_size( frame ),
                                    PyBUF_READ ) ) );
                    }
                    else // empty bytes in a new object, seems inefficient but
                    {    // if I do not do this then I overload the meaning of None
                        return boost::python::object(
                            boost::python::handle<>(
                                PyBytes_FromString( nullptr ) ) );
                    }
                }
                else
                    return boost::python::object();
            }

            // Message must stay alive as long as the view is alive
            // Will return None if BytesAtFront was not called or there
            // are no more frames
            boost::python::object BytesAtFront()
            {
                ZMQFrame* frame = zmsg_first( zmqMessage );

                if ( frame )
                {
                    size_t frameSize = zframe_size( frame );
                    if ( frameSize )
                    {
                        return boost::python::object(
                            boost::python::handle<>(
                                PyMemoryView_FromMemory(
                                    reinterpret_cast<char*>( zframe_data( frame ) ),
                                    zframe_size( frame ),
                                    PyBUF_READ ) ) );
                    }
                    else // empty bytes in a new object, seems inefficient but
                    {    // if I do not do this then I overload the meaning of None
                        return boost::python::object(
                            boost::python::handle<>(
                                PyBytes_FromString( nullptr ) ) );
                    }
                }
                else
                    return boost::python::object();
            }

            // Message must stay alive as long as the view is alive
            // with_custodian_and_ward_postcall< 0, 1 >()
            // Must be preceded by a call to ViewBytesAtFront
            // Will return None if BytesAtFront was not called or there
            // are no more frames
            boost::python::object NextBytes()
            {
                ZMQFrame* frame = zmsg_next( zmqMessage );

                if ( frame )
                {
                    size_t frameSize = zframe_size( frame );
                    if ( frameSize )
                    {
                        return boost::python::object(
                            boost::python::handle<>(
                                PyMemoryView_FromMemory(
                                    reinterpret_cast<char*>( zframe_data( frame ) ),
                                    zframe_size( frame ),
                                    PyBUF_READ ) ) );
                    }
                    else // empty bytes in a new object, seems inefficient but
                    {    // if I do not do this then I overload the meaning of None
                        return boost::python::object(
                            boost::python::handle<>(
                                PyBytes_FromString( nullptr ) ) );
                    }
                }
                else
                    return boost::python::object();
            }

            size_t TotalNumberOfFrames() const
            {
                return zmsg_size( zmqMessage );
            }

            size_t TotalSizeOfMessage() const
            {
                return zmsg_content_size( zmqMessage );
            }

            ZMQMessage* GetUnderlyingMessage() const
            {
                return zmqMessage;
            }

            // NOTE - using this object after Relinquish has been called on it is risky
            //        in some contexts, some API's will take ownership
            //        and the underlying message will become unusable at some point
            //        IF a copy of the message is required then call Duplicate prior
            //        to passing the message to an API that will Relinquish ownership
            //        from this object or do a deep copy (hopefully it will call copy ctor)
            void Relinquish()
            {
                owning = false;
            }

            bool Owning() const
            {
                return owning;
            }

        private:
            ZMQMessage* zmqMessage;
            bool owning;
        };
    }
}

#endif
