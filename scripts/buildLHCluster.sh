#!/bin/bash

TARGET_STAGE="$1"
if [[ "x${TARGET_STAGE}" == "x" ]];
then
    TARGET_STAGE="dist"
fi

# libprotobuf35x
./scripts/buildLibProtobuf35xLHDistImage.sh

./modules/lhscriptutil/scripts/buildImage.sh ./Dockerfiles/Dockerfile.lhcluster lhcluster "${TARGET_STAGE}"