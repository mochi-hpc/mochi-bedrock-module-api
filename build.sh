#!/bin/bash
SCRIPT_DIR=$(dirname "$0")
cmake $SCRIPT_DIR -DENABLE_EXAMPLES=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
