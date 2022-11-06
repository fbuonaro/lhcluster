#include <pylhclusterext/pyguard.h>

namespace LHClusterNS
{
    namespace Python
    {
        struct noGIL
        {
        public:
            noGIL()
                : state( PyEval_SaveThread() )
            {
#ifdef PYDEBUG
                clusterLogSetAction( "noGIL.CTOR" )
                    clusterLog( "noGIL.CTOR: begin[" << PyGILState_Check() << "]" )
#endif
            }

            ~noGIL()
            {
#ifdef PYDEBUG
                clusterLogSetAction( "noGIL.DTOR" )
                    clusterLog( "noGIL.DTOR: begin[" << PyGILState_Check() << "]" )
#endif
                    PyEval_RestoreThread( state );
#ifdef PYDEBUG
                clusterLog( "noGIL.DTOR: end[" << PyGILState_Check() << "]" )
#endif
            }
        private:
            PyThreadState* state;
        };

        struct GIL
        {
        public:
            GIL()
                : pyGILState( PyGILState_Ensure() )
            {
#ifdef PYDEBUG
                clusterLogSetAction( "GIL.CTOR" )
                    clusterLog( "GIL.CTOR: begin[" << PyGILState_Check() << "]" )
#endif
            }

            ~GIL()
            {
#ifdef PYDEBUG
                clusterLogSetAction( "GIL.DTOR" )
                    clusterLog( "GIL.DTOR: begin[" << PyGILState_Check() << "]" )
#endif
                    PyGILState_Release( pyGILState );
#ifdef PYDEBUG
                clusterLog( "GIL.DTOR: end[" << PyGILState_Check() << "]" )
#endif
            }

        private:
            PyGILState_STATE pyGILState;
        };

        template< typename T >
        void DeleteWithGIL( T* t )
        {
            GIL theGIL;
            delete t;
        }

        template< typename T >
        void DeleteWithNoGIL( T* t )
        {
            noGIL releaseTheGIL;
            delete t;
        }
    }
}
