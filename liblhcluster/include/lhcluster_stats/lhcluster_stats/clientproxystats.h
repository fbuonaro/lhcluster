#ifndef __LHCLUSTER_MESSAGESTATS_CLIENTPROXY_H__
#define __LHCLUSTER_MESSAGESTATS_CLIENTPROXY_H__

#include <lhcluster_stats/messagestats.h>

namespace LHClusterNS
{

#ifdef STAT
#undef STAT
#endif
#define STAT( stat ) stat,

    enum class ClientProxyStat : std::uint32_t
    {
        #include <lhcluster_stats/clientproxystatchoices.h>
        MAX
    };
}

#endif
