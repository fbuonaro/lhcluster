#ifndef __LHCLUSTER_IBROKERFACTORY_H__
#define __LHCLUSTER_IBROKERFACTORY_H__

#include <lhcluster/ibroker.h>
#include <lhcluster/brokerparameters.h>

#include <memory>

namespace LHClusterNS
{
    class IBrokerFactory
    {
        public:
            IBrokerFactory();
            virtual ~IBrokerFactory();

            virtual std::unique_ptr< IBroker > Create( const BrokerParameters& p ) = 0;

            // TODO - do not use, will be deprecated
            static std::unique_ptr< IBrokerFactory > GetDefaultFactory();
    };
}

#endif
