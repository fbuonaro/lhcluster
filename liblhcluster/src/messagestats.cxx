#include <lhcluster_stats/messagestats.h>

#include <lhcluster_stats/brokerstats.h>
#include <lhcluster_stats/clientproxystats.h>
#include <lhcluster_stats/notifysubscriberstats.h>
#include <lhcluster_stats/requesttypestats.h>
#include <lhcluster_stats/workerstats.h>

#ifdef STATS
namespace LHClusterNS
{

#ifdef STAT
#undef STAT
#endif
#define STAT( stat ) #stat,

    const char* brokerStatNames[] = {
        #include <lhcluster_stats/brokerstatchoices.h>
        "MAX"
    };

    template<>
    const std::vector< std::string >
    MessageStats< BrokerStat >::StatNames(
        brokerStatNames,
        brokerStatNames + ( sizeof( brokerStatNames ) / sizeof( brokerStatNames[0] ) ) );

    template<>
    const std::uint32_t
    MessageStats< BrokerStat >::StatMaxVal( static_cast< std::uint32_t >( BrokerStat::MAX ) );

    const char* workerStatNames[] = {
        #include <lhcluster_stats/workerstatchoices.h>
        "MAX"
    };

    template<>
    const std::vector< std::string >
    MessageStats< WorkerStat >::StatNames(
        workerStatNames,
        workerStatNames + ( sizeof( workerStatNames ) / sizeof( workerStatNames[0] ) ) );

    template<>
    const std::uint32_t
    MessageStats< WorkerStat >::StatMaxVal( static_cast< std::uint32_t >( WorkerStat::MAX ) );

    const char* clientProxyStatNames[] = {
        #include <lhcluster_stats/clientproxystatchoices.h>
        "MAX"
    };

    template<>
    const std::vector< std::string >
    MessageStats< ClientProxyStat >::StatNames(
        clientProxyStatNames,
        clientProxyStatNames +
        ( sizeof( clientProxyStatNames ) / sizeof( clientProxyStatNames[0] ) ) );

    template<>
    const std::uint32_t
    MessageStats< ClientProxyStat >::StatMaxVal(
        static_cast< std::uint32_t >( ClientProxyStat::MAX ) );

    const char* requestTypeStatNames[] = {
        #include <lhcluster_stats/requesttypestatchoices.h>
        "MAX"
    };

    template<>
    const std::vector< std::string >
    MessageStats< RequestTypeStat >::StatNames(
        requestTypeStatNames,
        requestTypeStatNames +
        ( sizeof( requestTypeStatNames ) / sizeof( requestTypeStatNames[0] ) ) );

    template<>
    const std::uint32_t
    MessageStats< RequestTypeStat >::StatMaxVal(
        static_cast< std::uint32_t >( RequestTypeStat::MAX ) );

    const char* notifySubscriberStatNames[] = {
        #include <lhcluster_stats/notifysubscriberstatchoices.h>
        "MAX"
    };

    template<>
    const std::vector< std::string >
    MessageStats< NotifySubscriberStat >::StatNames(
        notifySubscriberStatNames,
        notifySubscriberStatNames +
        ( sizeof( notifySubscriberStatNames ) / sizeof( notifySubscriberStatNames[0] ) ) );

    template<>
    const std::uint32_t
    MessageStats< NotifySubscriberStat >::StatMaxVal(
        static_cast< std::uint32_t >( NotifySubscriberStat::MAX ) );
}

#endif
