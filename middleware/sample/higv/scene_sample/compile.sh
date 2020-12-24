#!/bin/bash
# Perform hiberry standard release.
#  Copyright © Hisilicon Tech. Co, Ltd. 2010-2020. All rights reserved.
set -e
cd ../../../component/higv/
make clean
make
cd -
make clean
make
