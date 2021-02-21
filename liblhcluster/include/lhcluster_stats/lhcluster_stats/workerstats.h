#ifndef __LHCLUSTER_MESSAGESTATS_WORKER_H__
#define __LHCLUSTER_MESSAGESTATS_WORKER_H__

#include <lhcluster_stats/messagestats.h>

namespace LHClusterNS
{

#ifdef STAT
#undef STAT
#endif
#define STAT( stat ) stat,

    enum class WorkerStat : std::uint32_t
    {
        #include <lhcluster_stats/workerstatchoices.h>
        MAX
    };
}

#endif
