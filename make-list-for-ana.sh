#!/bin/bash
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

source $DIR_TOP/setup.sh
echo "RAW_NAME = $RAW_NAME"
echo "BG_MODE  = $BG_MODE"

mkdir -p $DIR_AUTO

#DIR=$DIR_DATA_BASE/data/$RAW_NAME
DIR=$DIR_DATA_BASE/clean/$RAW_NAME
#DIR=$DIR_DATA_BASE/$BG_MODE/$RAW_NAME
if [ -e $DIR ] ; then
    find $DIR -name "user_*.root" | sort >$DIR_AUTO/list_ana_event.txt
fi

DIR=$DIR_DATA_BASE/track/$RAW_NAME/$BG_MODE/track
if [ -e $DIR ] ; then
    find $DIR -name "track_*.root" | sort >$DIR_AUTO/list_ana_track.txt
fi

DIR=$DIR_DATA_BASE/vertex/$RAW_NAME/$BG_MODE/vertex
if [ -e $DIR ] ; then
    find $DIR -name "vertex_*.root" | sort >$DIR_AUTO/list_ana_vertex.txt
fi
