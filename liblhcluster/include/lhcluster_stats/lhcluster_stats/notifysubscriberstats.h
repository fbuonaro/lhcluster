#ifndef __LHCLUSTER_MESSAGESTATS_NOTIFYSUBSCRIBER_H__
#define __LHCLUSTER_MESSAGESTATS_NOTIFYSUBSCRIBER_H__

#include <lhcluster_stats/messagestats.h>

namespace LHClusterNS
{

#ifdef STAT
#undef STAT
#endif
#define STAT( stat ) stat,

    enum class NotifySubscriberStat : std::uint32_t
    {
        #include <lhcluster_stats/notifysubscriberstatchoices.h>
        MAX
    };
}

#endif
