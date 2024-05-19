#! /usr/bin/env bash
#
find server -name '*.*pp' | xargs clang-tidy
