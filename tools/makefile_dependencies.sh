#!/usr/bin/env bash

DIR=$(dirname "${BASH_SOURCE[0]}")
DIR="$( cd "$DIR" && pwd)"

make -pn "$@" | MODE=dot "$DIR"/makefile_dependencies.pl | tee $DEBUGOUTPUT | dot -Tsvg -oMakefile.svg
