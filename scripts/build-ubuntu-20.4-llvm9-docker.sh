#!/bin/bash
#
# Copyright (C) 2018-2020 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

git fetch -t
git clone ../compute-runtime neo
docker build -f scripts/docker/Dockerfile-ubuntu-20.04-llvm-9 -t neo-ubuntu-20.4-llvm-9:ci .

