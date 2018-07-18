#!/bin/bash

export KDB_BINARY=$HOME/runq5000
export LD_LIBRARY_PATH=$PWD
export STARTUP_QSCRIPT=getStats.q
export EXTRA_SWITCHES=-q

export BANNER_ITEMS="For|Tutorials|To|Welcome"
export RESULT=$(eval $KDB_BINARY $STARTUP_QSCRIPT $EXTRA_SWITCHES | egrep -v $BANNER_ITEMS)
echo "result = " $RESULT
exit 0
