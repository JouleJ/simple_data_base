#! /usr/bin/env bash

clang-tidy `find server -name '*.*pp'` -- "$CFLAGS"
