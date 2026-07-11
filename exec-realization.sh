#!/bin/bash
## Script to run the chamber realization.
## It is similar to `seaquest-ktracker/scripts/embed/process_realization.sh`,
## but automatically sets the parameters.
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

source $DIR_TOP/setup.sh
echo "RAW_NAME = $RAW_NAME"

DIR_IN=$DIR_DATA_BASE/data/$RAW_NAME
DIR_OUT=$DIR_DATA_BASE/clean/$RAW_NAME
echo "DIR_IN  = $DIR_IN"
echo "DIR_OUT = $DIR_OUT"

LIST_INPUT=()
while read FN_DAT ; do
    LIST_INPUT+=( $(basename $FN_DAT) )
done < <( find $DIR_IN -name "*_${RAW_NAME}_*.root" | sort )

N_INPUT=${#LIST_INPUT[*]}
echo "N_INPUT = $N_INPUT"

mkdir -p $DIR_OUT
for (( II = 0 ; II < $N_INPUT ; II++ )) ; do
    FNAME=${LIST_INPUT[$II]}
    FNAME_END=${FNAME%.root}.end
    echo "$II/$N_INPUT: $FNAME"
    if [ -e $DIR_OUT/$FNAME -a -e $DIR_OUT/$FNAME_END ] ; then
	echo "  The output file already exists.  Skipped."
	continue
    fi
    OPTS_FILE=$KTRACKER_ROOT/opts/mc_$OPT_NAME.opts
    SEED=1
    $KTRACKER_ROOT/filtering_and_realization $OPTS_FILE $DIR_IN $DIR_OUT $FNAME $SEED
    RET=$?
    if [ $RET -eq 0 ] ; then
	touch $DIR_OUT/$FNAME_END
    else
	echo "  'filtering_and_realization' returned $RET.  Abort."
	exit 1
    fi
done
