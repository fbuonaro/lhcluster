cmake_minimum_required( VERSION 3.10 )

project( lhcluster
         DESCRIPTION "lhcluster library and python extension module"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhcluster"
         LANGUAGES CXX
         VERSION 1.0.0 
 )

enable_testing()

set(LH_SUBDIRECTORIES liblhcluster libpylhclusterext pylhcluster)

include("modules/lhscriptutil/cmake/lhmacros.cmake")

lh_toplevel_add_subdirectories()

lh_toplevel_add_cpack_package_components()