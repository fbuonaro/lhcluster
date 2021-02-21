#ifndef __LHCLUSTER_MESSAGESTATS_H__
#define __LHCLUSTER_MESSAGESTATS_H__

#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define STATS

#ifdef STATS
#define IncrementStat( stats, stat ) stats.Increment( stat )
#define ClearStats( stats ) stats.Clear()

namespace LHClusterNS
{
    template< typename T >
    class MessageStats
    {
        public:
            typedef T StatEnum;
            // DATA
            static const std::vector< std::string > StatNames;
            static const std::uint32_t StatMaxVal;

            // CTORS, DTORS
            MessageStats()
            :   stats{ 0 }
            ,   oss()
            {
            }

            MessageStats( const MessageStats& other )
            :   stats( other.stats )
            ,   oss()
            {
            }

            MessageStats(
                const std::unordered_map< StatEnum, std::uint64_t >& mStats )
            :   MessageStats()
            {
                for( auto it = mStats.cbegin(); it != mStats.cend(); ++it )
                    stats[ it->first ] = it->second;
            }

            // METHODS
            inline void Increment( StatEnum stat )
            {
                ++stats[ static_cast< std::uint32_t >( stat ) ];
            }

            inline void Clear()
            {
                std::fill( stats, stats + ( sizeof( stats ) / sizeof( stats[0] ) ), 0 );
            }

            inline const std::uint64_t& operator[]( std::uint32_t statVal ) const
            {
                return stats[ statVal ];
            }

            inline const std::uint64_t& operator[]( StatEnum stat ) const
            {
                return stats[ static_cast< std::uint32_t >( stat ) ];
            }

            inline std::uint64_t& operator[]( StatEnum stat )
            {
                return stats[ static_cast< std::uint32_t >( stat ) ];
            }

            bool operator==( const MessageStats& rstats ) const
            {
                bool match = true;
                std::uint32_t stat = 0;
                std::uint32_t eStat =
                    static_cast< std::uint32_t >( StatEnum::MAX );
                for( stat = 0; stat != eStat; ++stat )
                {
                    if( stats[ stat ] != rstats[ stat ] )
                    {
                        match = false;
                        break;
                    }
                }

                return match;
            }

            std::unordered_map<
                StatEnum, std::pair< std::uint64_t, std::uint64_t > >
            GetDifferences( const MessageStats& rstats )
            {
                std::unordered_map<
                    StatEnum,
                    std::pair< std::uint64_t, std::uint64_t > > differences;
                std::uint32_t stat = 0;
                std::uint32_t eStat =
                    static_cast< std::uint32_t >( StatEnum::MAX );
                for( stat = 0; stat != eStat; ++stat )
                {
                    if( stats[ stat ] != rstats[ stat ] )
                        differences[ static_cast< StatEnum >( stat ) ] =
                            std::pair< std::uint64_t, std::uint64_t >(
                                stats[ stat ],
                                rstats[ stat ] );
                }

                return differences;
            }

            static std::string GetDelimitedStatNames()
            {
                auto it = StatNames.cbegin();
                auto itEnd = StatNames.cend();
                std::ostringstream oss;

                oss.str( std::string() );

                oss << *it;
                while( ++it < itEnd )
                    oss << "|" << *it;
 
                return oss.str();
            }

            std::string GetDelimitedStatValues()
            {
                oss.str( std::string() );

                oss << stats[ 0 ];
                for( std::uint32_t i = 1; i < StatMaxVal; ++i )
                    oss << "|" << stats[ i ];
 
                return oss.str();
            }

        private:
            std::uint64_t stats[ static_cast< std::uint32_t >(  StatEnum::MAX ) ];
            std::ostringstream oss;
    };
}
#else

#define IncrementStat( stats, stat ) 
#define ClearStats( stats )

#endif

#endif
