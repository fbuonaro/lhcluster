import sys

from pylhcluster.plhc_api import PyLHClusterMain

if __name__ == '__main__':
    ret = PyLHClusterMain( sys.argv )
    sys.exit( ret )
