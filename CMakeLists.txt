cmake_minimum_required( VERSION 3.10 )

project( lhcluster
         DESCRIPTION "lhcluster library and python extension module"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhcluster"
         LANGUAGES CXX
         VERSION 1.0.0 
 )

enable_testing()

add_subdirectory( liblhcluster )
add_subdirectory( libpylhclusterext )
add_subdirectory( pylhcluster )
