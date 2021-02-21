#ifndef __LHCLUSTER_INOTIFYPUBLISHER_H__
#define __LHCLUSTER_INOTIFYPUBLISHER_H__

#include <lhcluster/cluster.h>
#include <lhcluster/zmqaddin.h>

namespace LHClusterNS 
{
    class INotifyPublisher
    {
        public:
            // CTORS, DTORS
            virtual ~INotifyPublisher();

            // METHODS
            virtual int Publish(
                const NotificationType& notifyType ) = 0;
            virtual int Publish(
                const NotificationType& notifyType,
                RequestId requestId ) = 0;
            virtual int Publish(
                const NotificationType& notifyType,
                ZMQMessage** lpZMQMessage ) = 0;
            virtual int Publish(
                const NotificationType& notifyType,
                RequestId requestId,
                ZMQMessage** lpZMQMessage ) = 0;
    };
}

#endif
