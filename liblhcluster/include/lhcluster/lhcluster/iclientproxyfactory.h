#ifndef __LHCLUSTER_ICLIENTPROXYFACTORY_H__
#define __LHCLUSTER_ICLIENTPROXYFACTORY_H__

#include <lhcluster/iclientproxy.h>
#include <lhcluster/clientproxyparameters.h>

#include <memory>

namespace LHClusterNS
{
    class IClientProxyFactory
    {
        public:
            IClientProxyFactory();
            virtual ~IClientProxyFactory();

            virtual std::unique_ptr< IClientProxy > Create( const ClientProxyParameters& p ) = 0;

            static std::unique_ptr< IClientProxyFactory > GetDefaultFactory();
    };
}

#endif
