#ifndef __LHCLUSTER_INOTIFYHANDLER_H__
#define __LHCLUSTER_INOTIFYHANDLER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/zmqaddin.h>

namespace LHClusterNS
{
    class INotifyHandler
    {
        public:
            virtual ~INotifyHandler();

            // NOTE - notificationMsg is owned by caller, can be modified, copied from,
            //        but not destroyed or persist after call to Process
            virtual RequestStatus Process(
                const NotificationType& notifyType,
                ZMQMessage* notificationMsg ) = 0;
    };
}

#endif
