#!/bin/bash
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

if [ -z $DIR_DATA_BASE ] ; then
    echo "Need 'source setup.sh' first.  Abort."
    exit 1
fi

function PrintHelp {
    cat <<-EOF
	This script counts the number of MC files and prints their max/min sizes.
	You might run it during/after executing grid jobs to confirm that 
	the MC files have been created properly.
	
	Usage:
	  $BASH_SOURCE (data-4pi|d-4)
	  $BASH_SOURCE (data-acc|d-a)
	  $BASH_SOURCE (clean|c)
	  $BASH_SOURCE (messy|m)
	  $BASH_SOURCE (track-clean|t-c)
	  $BASH_SOURCE (track-messy|t-m)
	  $BASH_SOURCE (vertex-clean|v-c)
	  $BASH_SOURCE (vertex-messy|v-m)
	EOF
}
    
DIR=
case $1 in
    data-4pi | d-4 )
	SetRawName 4pi
	DIR=$DIR_DATA_BASE/data/$RAW_NAME
	;;
    data-acc | d-a) 
	SetRawName acc
	DIR=$DIR_DATA_BASE/data/$RAW_NAME
	;;
    clean | c)
	SetRawName acc
	DIR=$DIR_DATA_BASE/clean/$RAW_NAME
	;;
    messy | m)
	SetRawName acc
	DIR=$DIR_DATA_BASE/messy/$RAW_NAME
	;;
    track-clean | t-c )
	SetRawName acc
	DIR=$DIR_DATA_BASE/track/$RAW_NAME/clean/track
	;;
    track-messy | t-m )
	SetRawName acc
	DIR=$DIR_DATA_BASE/track/$RAW_NAME/clean/track
	;;
    vertex-clean | v-c )
	SetRawName acc
	DIR=$DIR_DATA_BASE/vertex/$RAW_NAME/clean/vertex
	;;
    vertex-messy | v-m )
	SetRawName acc
	DIR=$DIR_DATA_BASE/vertex/$RAW_NAME/clean/vertex
	;;
    * )
	PrintHelp
	exit 0
	;;
esac

echo "DIR = $DIR"
if [ ! -d $DIR ] ; then
    echo "The directory doesn't exist.  Abort."
    exit 1
fi

LIST_FILES=( $(find $DIR -type f -name '*.root' -printf '%k\n' | sort -n) )
N_FILE=${#LIST_FILES[*]}
echo "N_FILE = $N_FILE"
if [ $N_FILE -gt 0 ] ; then
    MIN_K=${LIST_FILES[0]}
    MAX_K=${LIST_FILES[((N_FILE - 1))]}
    echo "MIN    = $(( MIN_K / 1024 )) MB"
    echo "MAX    = $(( MAX_K / 1024 )) MB"
fi

#echo "${LIST_FILES[*]}"
