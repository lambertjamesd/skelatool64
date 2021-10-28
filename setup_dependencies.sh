#!/usr/bin/bash
git submodule update --init --remote assimp
# git submodule update --init --remote yaml-cpp

cd assimp
cmake .
make

cd ../yaml-cpp
cmake .
make