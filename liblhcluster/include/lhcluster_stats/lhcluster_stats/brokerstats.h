#ifndef __LHCLUSTER_MESSAGESTATS_BROKER_H__
#define __LHCLUSTER_MESSAGESTATS_BROKER_H__

#include <lhcluster_stats/messagestats.h>

namespace LHClusterNS
{

#ifdef STAT
#undef STAT
#endif
#define STAT( stat ) stat,

    enum class BrokerStat : std::uint32_t
    {
        #include <lhcluster_stats/brokerstatchoices.h>
        MAX
    };
}

#endif
