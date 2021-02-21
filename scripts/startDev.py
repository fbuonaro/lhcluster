#!python3.7

import argparse
import os
import subprocess
import sys

DFLT_LHCLUSTER_IMAGE_NAME = "lhcluster:test-env"

def getParser():
    parser = argparse.ArgumentParser(
        description='start lhcluster development container')
    cwd = os.getcwd()

    parser.add_argument(
        '-i', '--image',
        help='lhcluster image name',
        default=DFLT_LHCLUSTER_IMAGE_NAME )
    parser.add_argument(
        '-p', '--project_root',
        help='root of lhcluster project, default to $PWD',
        default=cwd )

    return parser

def main():
    parser = getParser()
    args = parser.parse_args()
    dockerParams = \
    [
        'docker',
        'run',
        '--rm',
        '-it',
        '-v',
        '{0}:/lhcluster-dev'.format( args.project_root ),
        '--cap-add=SYS_PTRACE', 
        '--entrypoint',
        '/bin/bash',
        args.image
    ]
    proc = subprocess.Popen( dockerParams )

    proc.wait()

    ret = proc.returncode

    return ret


if __name__ == '__main__':
    ret = main()
    sys.exit( ret )
