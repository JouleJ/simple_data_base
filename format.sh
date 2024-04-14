#! /usr/bin/env bash
#
find . -name '*.cpp' -or -name '*.hpp' | xargs clang-format -i
