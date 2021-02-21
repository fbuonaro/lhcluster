#ifndef __LHCLUSTER_CLUSTER_H__
#define __LHCLUSTER_CLUSTER_H__

#include <chrono>
#include <cstdint>
#include <string>

#define LHCLUSTER_PROTOCOL_VERSION_MAJOR 0
#define LHCLUSTER_PROTOCOL_VERSION_MINOR 1

namespace LHClusterNS
{
    typedef std::chrono::seconds Delay;

    typedef std::chrono::milliseconds DelayMS;

    typedef std::uint32_t RequestType;

    typedef std::uint64_t RequestId;

    typedef std::string NotificationType;

    enum class RequestStatus : std::uint32_t
    {
        None,
        New,
        InProgress, // Application-level, will not trigger unsub when returned from notify handler
        Succeeded,  // Application-level, will trigger unsub when returned from notify handler
        Failed,     // Application-level
        FatalError, // !!! Application-level !!!: Will terminate Worker when returned by processor
                    // Broker-level: Request failed due to crash, unreachability, etc
        Rejected,   // Broker-level indicator that request is already in progress
        WorkerDeath,// Broker-level indicator of a lost connection to worker,
                    // possibly a result of a crash in worker while processing the request
        Redirected  // Broker-level: Redirected to another client
    };

    enum class ClusterMessage : std::uint32_t
    {
        None,
        WorkerHeartbeat,
        WorkerReady,
        WorkerFinished,
        WorkerReceipt,
        BrokerHeartbeat,
        BrokerNewRequest,
        BrokerResponse,
        BrokerSentReceipt,
        BrokerReceivedReceipt,
        BrokerShuttingDown,
        ClientHeartbeat,
        ClientNewRequest
    };

    enum class ClientCommand : std::uint32_t
    {
        None,
        Start,
        Stop
    };

    enum class WorkerCommand : std::uint32_t
    {
        None,
        Start,
        Stop
    };

    enum class BrokerCommand : std::uint32_t
    {
        None,
        Start,
        Stop
    };

    enum class NotifySubscriberCommand : std::uint32_t
    {
        None,
        Start,
        Stop,
        Subscribe,
        UnSubscribe
    };

    struct LHCVersion
    {
        std::uint16_t lhcmajor;
        std::uint16_t lhcminor;

        LHCVersion(
            std::uint16_t _lhcmajor,
            std::uint16_t _lhcminor )
        :   lhcmajor( _lhcmajor )
        ,   lhcminor( _lhcminor )
        {
        }

        LHCVersion()
        :   lhcmajor( LHCLUSTER_PROTOCOL_VERSION_MAJOR )
        ,   lhcminor( LHCLUSTER_PROTOCOL_VERSION_MINOR )
        {
        }
    };

    inline bool operator==( const LHCVersion& a, const LHCVersion& b )
    {
        return ( a.lhcmajor == b.lhcmajor ) &&
               ( a.lhcminor == b.lhcminor );
    }

    inline bool operator<( const LHCVersion& a, const LHCVersion& b )
    {
        return ( a.lhcmajor < b.lhcmajor ) ||
               ( ( a.lhcmajor == b.lhcmajor ) &&
                 ( a.lhcminor < b.lhcminor ) );
    }

    inline bool operator!=( const LHCVersion& a, const LHCVersion& b )
    {
        return !( a == b );
    }

    inline bool operator>( const LHCVersion& a, const LHCVersion& b )
    {
        return ( a.lhcmajor > b.lhcmajor ) ||
               ( ( a.lhcmajor == b.lhcmajor ) &&
                 ( a.lhcminor > b.lhcminor ) );
    }

    inline bool operator<=( const LHCVersion& a, const LHCVersion& b )
    {
        return ( a.lhcmajor < b.lhcmajor ) ||
               ( ( a.lhcmajor == b.lhcmajor ) &&
                 ( ( a.lhcminor == b.lhcminor ) ||
                   ( a.lhcminor < b.lhcminor ) ) );
    }

    inline bool operator>=( const LHCVersion& a, const LHCVersion& b )
    {
        return ( a.lhcmajor > b.lhcmajor ) ||
               ( ( a.lhcmajor == b.lhcmajor ) &&
                 ( ( a.lhcminor == b.lhcminor ) ||
                   ( a.lhcminor > b.lhcminor ) ) );
    }

    struct MessageFlag
    {
        enum MessageFlagEnum : std::uint64_t
        {
            //                   >32bit|<=32bit 
            //                  xxxxxxxxoooooooo
            //                  o bits supported in boost python enums only
            None            = 0x0000000000000000,
            Broker          = 0x8000000000000000,   // whether the worker is really another broker
                                                    // will cause broker to forego some actions
                                                    // and just pass through most messages to
                                                    // clients
            Asynchronous    = 0x0000000000000001,   // No response
            ReceiptOnReceive= 0x0000000000000002,   // Receipt on FIRST worker receive
            ReceiptOnSend   = 0x0000000000000004,   // Receipt on FIRST broker send to worker
            NotifyOnDeath   = 0x0000000000000008,   // Notify on worker death (keep env in broker)
            RetryOnDeath    = 0x0000000000000010,   // Retry 1x on worker death (saves message)
            AllowRedirect   = 0x0000000000000020,   // Allow redirect of response to other client
            NotifyOnRedirect= 0x0000000000000040    // Notify on redirect (keep env in broker)
        };
    };

    typedef std::uint64_t MessageFlags;

    struct LHCVersionFlags
    {
        LHCVersion version;
        MessageFlags flags;

        LHCVersionFlags(
            MessageFlags _flags )
        :   version()
        ,   flags( _flags )
        {
        }

        LHCVersionFlags(
            LHCVersion _version,
            MessageFlags _flags )
        :   version( _version )
        ,   flags( _flags )
        {
        }

        LHCVersionFlags(
            uint16_t vlhcmajor,
            uint16_t vlhcminor,
            MessageFlags _flags )
        :   version( vlhcmajor, vlhcminor )
        ,   flags( _flags )
        {
        }

        LHCVersionFlags()
        :   version()
        ,   flags( MessageFlag::None )
        {
        }

        void Reset()
        {
            version.lhcmajor = 0;
            version.lhcminor = 0;
            flags = MessageFlag::None;
        }

        bool hasAny( MessageFlags _flags ) const
        {
            return ( ( flags & _flags ) != MessageFlag::None );
        }

        bool hasAll( MessageFlags _flags ) const
        {
            return ( ( flags & _flags ) == _flags );
        }
    };

    inline bool operator==( const LHCVersionFlags& a, const LHCVersionFlags& b )
    {
        return ( a.version == b.version ) &&
               ( a.flags == b.flags );
    }
}

#endif
