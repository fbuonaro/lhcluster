#include <boost/function.hpp>
#include <boost/function_types/components.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/result_type.hpp>

#include <boost/python.hpp>

#include <boost/tuple/tuple.hpp>

#include <utility>

// Solution from:
//     https://stackoverflow.com/a/18648366

namespace LHClusterNS
{
namespace Python
{
    namespace
    {
        // Functor that will invoke a function while holding a guard.
        // Upon returning from the function, the guard is released.
        template < typename Signature, typename Guard >
        class guardedFunction
        {
            public:
                typedef typename boost::function_types::result_type< Signature >::type resultType;

                template < typename Fn >
                guardedFunction( Fn _fn )
                :   fn( _fn )
                {
                }

                // TODO - as you can see, I have no idea how variadic template arguments work
                //        and barely any idea how non-variadic templates work

                /*
                template < typename... As >
                resultType operator()( As... as )
                {
                    Guard g;
                    return fn( as... );
                }
                */
                resultType operator()()
                {
                    Guard g;
                    return fn();
                }

                /*
                BAD
                template < typename A1 >
                resultType operator()( A1 a1 )
                {
                    Guard g;
                    return fn( a1 );
                }

                template < typename A1, typename A2 >
                resultType operator()( A1 a1, A2 a2 )
                {
                    Guard g;
                    return fn( a1, a2 );
                }
                */

                // NOTE - I have no idea what I am doing
                //        I only know that this fails when A1, etc is not copyable
                //        and "perfect forwarding" via && + std::forward  seems to fix that
                template < typename A1 >
                resultType operator()( A1&& a1 )
                {
                    Guard g;
                    return fn( std::forward< A1 >( a1 ) );
                }

                template < typename A1, typename A2 >
                resultType operator()( A1&& a1, A2&& a2 )
                {
                    Guard g;
                    return fn( std::forward< A1 >( a1 ), std::forward< A2 >( a2 ) );
                }

                template < typename A1, typename A2, typename A3 >
                resultType operator()( A1&& a1, A2&& a2, A3&& a3 )
                {
                    Guard g;
                    return fn( std::forward< A1 >( a1 ), 
                               std::forward< A2 >( a2 ),
                               std::forward< A3 >( a3 ) );
                }

                template < typename A1, typename A2, typename A3, typename A4 >
                resultType operator()( A1&& a1, A2&& a2, A3&& a3, A4&& a4 )
                {
                    Guard g;
                    return fn( std::forward< A1 >( a1 ), 
                               std::forward< A2 >( a2 ),
                               std::forward< A3 >( a3 ),
                               std::forward< A4 >( a4 ) );
                }

                template < typename A1, typename A2, typename A3, typename A4, typename A5 >
                resultType operator()( A1&& a1, A2&& a2, A3&& a3, A4&& a4, A5&& a5 )
                {
                    Guard g;
                    return fn( std::forward< A1 >( a1 ), 
                               std::forward< A2 >( a2 ),
                               std::forward< A3 >( a3 ),
                               std::forward< A4 >( a4 ),
                               std::forward< A5 >( a5 ) );
                }

            private:
                boost::function< Signature > fn;
        };

        // Provides signature type.
        template < typename Signature >
        struct mplSignature
        {
            typedef typename boost::function_types::components< Signature >::type type;
        };

        // Support boost::function.
        template < typename Signature >
        struct mplSignature< boost::function< Signature > >: public mplSignature< Signature >
        {
        };

        // Create a callable object with guards.
        template < typename Guard,
                   typename Fn,
                   typename Policy >
        boost::python::object withAux( Fn fn, const Policy& policy )
        {
            // Obtain the components of the Fn.  This will decompose non-member
            // and member functions into an mpl sequence.
            //   R (*)(A1)    => R, A1
            //   R (C::*)(A1) => R, C*, A1
            typedef typename mplSignature<Fn>::type mplSignatureType;

            // Synthesize the components into a function type.  This process
            // causes member functions to require the instance argument.
            // This is necessary because member functions will be explicitly
            // provided the 'self' argument.
            //   R, A1     => R (*)(A1)
            //   R, C*, A1 => R (*)(C*, A1)
            typedef typename boost::function_types::function_type<
              mplSignatureType >::type signatureType;

            // Create a callable boost::python::object that delegates to the
            // guardedFunction.
            return boost::python::make_function(
                guardedFunction< signatureType, Guard >( fn ),
                policy,
                mplSignatureType() );
        }
    }

    // Create a callable object with guards.
    template < typename Guard,
               typename Fn,
               typename Policy >
    boost::python::object with( const Fn& fn, const Policy& policy )
    {
        return withAux< Guard >( fn, policy );
    }

    // Create a callable object with guards.
    template < typename Guard,
               typename Fn>
    boost::python::object with( const Fn& fn )
    {
        return with< Guard >( fn, boost::python::default_call_policies() );
    }
}
}
