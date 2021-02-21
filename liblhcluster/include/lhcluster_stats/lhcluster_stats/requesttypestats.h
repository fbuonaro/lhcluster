#ifndef __LHCLUSTER_MESSAGESTATS_REQUESTTYPE_H__
#define __LHCLUSTER_MESSAGESTATS_REQUESTTYPE_H__

#include <lhcluster_stats/messagestats.h>

namespace LHClusterNS
{

#ifdef STAT
#undef STAT
#endif
#define STAT( stat ) stat,

    enum class RequestTypeStat : std::uint32_t
    {
        #include <lhcluster_stats/requesttypestatchoices.h>
        MAX
    };
}

#endif
