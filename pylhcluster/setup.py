#!/usr/bin/python3.6

import setuptools
import os
import io

longDesc = ""
cwd = os.path.abspath( os.path.dirname( __file__ ) )
with io.open( os.path.join( cwd, 'README.md' ), encoding='utf-8' ) as f:
    longDesc = f.read()

# always exlcude test dir
# exclude any package ending in _test only when building outside of tox
excludeList = []
if( ( "TOX_WORK_DIR" not in os.environ ) and \
    ( "PYLHC_TEST_ENV" not in os.environ ) ):
    excludeList = [ "*.tests", "*.tests.*", "tests.*", "tests", "*.pylhcluster_test", "*.pylhcluster_test.*", "pylhcluster_test.*", "pylhcluster_test" ]

setuptools.setup( \
    author          =   "Frank Buonarota",
    classifiers     =   [   "Development Status :: 3 - Alpha",
                            "Intended Audience :: Developers",
                            "License :: OSI Approved :: GNU General Public License",
                            "Programming Language :: Python :: 3.6" ],
    description     =   "Python wrapper around liblhcluster",
    # entry_points    =   {   "console_scripts" : [ \
    #                             'run = pylhcluster.plhc_api.Run' ] },
    long_description=   longDesc,
    name            =   "pylhcluster",
    # packages        =   [ "pylhcluster" ],
    packages        =   setuptools.find_packages(exclude=excludeList),
    package_data    =   {   "pylhcluster" : [ \
                                "plhc_api/logging_config_templates/*.ini",
                                "plhc_ext/*.so" ] },
    python_requires =   ">=3.6",
    url             =   "https://github.com/fbuonaro/lhcluster/pylhcluster",
    version         =   "0.0.1",
)
