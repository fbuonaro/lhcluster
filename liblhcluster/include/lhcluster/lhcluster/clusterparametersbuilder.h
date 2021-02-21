#ifndef __LHCLUSTER_CLUSTERPARAMETERSBUILDER_H__
#define __LHCLUSTER_CLUSTERPARAMETERSBUILDER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/endpoint.h>
#include <lhcluster/zmqaddin.h>

#include <stdexcept>
#include <string>
#include <sstream>

namespace LHClusterNS
{

    class ClusterParametersBuildFailed : public std::runtime_error
    {
        public:
            explicit ClusterParametersBuildFailed( const std::string& _msg )
            :   std::runtime_error( _msg )
            {
            }

            explicit ClusterParametersBuildFailed()
            :   std::runtime_error( "Failed to construct cluster parameters"  )
            {
            }
    };

    template< class T, class P > // T extends ClusterParametersBuilder via CRTP, Builds P
    class ClusterParametersBuilder
    {
        public:
            typedef T BuilderType;
            typedef P ParametersType;

            ClusterParametersBuilder()
            :   instance( 0 )
            ,   process( 0 )
            ,   thread( 0 )
            {
            }

            virtual ~ClusterParametersBuilder()
            {
            }

            virtual P Build( std::ostringstream& oss ) const = 0;
            virtual bool OK() const = 0;

            P Build() const
            {
                std::ostringstream oss;
                return this->Build( oss );
            }

            T& SetInstance( int _instance )
            {
                instance = _instance;
                // assumed that T is the derived class
                return *( dynamic_cast< T* >( this ) );
            }

            T& SetProcess( int _process )
            {
                process = _process;
                return *( dynamic_cast< T* >( this ) );
            }

            T& SetThread( int _thread )
            {
                thread = _thread;
                return *( dynamic_cast< T* >( this ) );
            }

            int GetInstance() const
            {
                return instance;
            }

            int GetProcess() const
            {
                return process;
            }

            int GetThread() const
            {
                return thread;
            }

        private:
            int instance;
            int process;
            int thread;
    };
}

#endif
