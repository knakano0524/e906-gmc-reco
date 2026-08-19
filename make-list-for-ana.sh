#!/bin/bash
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

source $DIR_TOP/setup.sh
echo "RAW_NAME = $RAW_NAME"
echo "BG_MODE  = $BG_MODE"

mkdir -p $DIR_AUTO

if [ $ACC_MODE = 4pi ] ; then
    DIR=$DIR_DATA_BASE/data/$RAW_NAME
    if [ -e $DIR ] ; then
	find $DIR -name "user_*.root" | sort >$DIR_AUTO/list_ana_4pi.txt
    fi
    exit
fi

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

DIR_C=$DIR_DATA_BASE/vertex/$RAW_NAME/clean/vertex
DIR_M=$DIR_DATA_BASE/vertex/$RAW_NAME/messy/vertex
if [ -e $DIR_C -a -e $DIR_M ] ; then
    find $DIR_C -name "vertex_*.root" | sort | while read FN_C ; do
	FN_M=${FN_C/clean/messy}
	test -e $FN_M && echo "$FN_C	$FN_M"
    done >$DIR_AUTO/list_ana_clean_messy.txt
fi
