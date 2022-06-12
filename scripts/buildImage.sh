#!/bin/bash

DOCKERFILE_PATH="./Dockerfile"
TARGET_STAGE="lhcluster-s4-main-env"
TARGET_TAG="lhcluster:main"
TEST_STAGE="lhcluster-stage-test-env"
TEST_TAG="lhcluster:test-env"
BASE_STAGE="lhcluster-stage-base-env"

# Build the main image
docker build --target ${TARGET_STAGE} -t ${TARGET_TAG} -f ${DOCKERFILE_PATH} .

# TODO - fix tag the intermediate image, expecting it to be test-env
# docker tag $(docker image ls -f "label=${TEST_STAGE}=yes" -q | head -1) ${TEST_TAG}

# Clean up overridden images
docker image prune -f --filter "label=${BASE_STAGE}"
