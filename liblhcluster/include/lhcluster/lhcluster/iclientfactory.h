#ifndef __LHCLUSTER_ICLIENTFACTORY_H__
#define __LHCLUSTER_ICLIENTFACTORY_H__

#include <lhcluster/iclient.h>
#include <lhcluster/clientparameters.h>

#include <memory>

namespace LHClusterNS
{
    class IClientFactory
    {
        public:
            IClientFactory();
            virtual ~IClientFactory();

            virtual std::unique_ptr< IClient > Create( const ClientParameters& p ) = 0;

            static std::unique_ptr< IClientFactory > GetDefaultFactory();
    };
}

#endif
