#!/bin/bash
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

source $DIR_TOP/setup.sh
echo "RAW_NAME = $RAW_NAME"
echo "BG_MODE  = $BG_MODE"

DIR_IN=$DIR_DATA_BASE/track/$RAW_NAME/$BG_MODE
DIR_OUT=$DIR_DATA_BASE/vertex/$RAW_NAME/$BG_MODE
FN_LIST=list_for_vertexing.txt
echo "DIR_IN   = $DIR_IN"
echo "DIR_OUT  = $DIR_OUT"
echo "FN_LIST  = $FN_LIST"

mkdir -p $DIR_AUTO

rm -f $DIR_AUTO/$FN_LIST # Comment out this line, not to re-make the list
if [ ! -e $DIR_AUTO/$FN_LIST ] ; then
    echo -n "Making $DIR_AUTO/$FN_LIST... "
    find $DIR_IN -name "track_*.root" | sort | while read FN_DAT ; do
	BN_DAT=$(basename $FN_DAT .root)
	RUN=${BN_DAT:6:6}
	echo -n "o" #  $BN_DAT
	echo -e "$RUN\t$FN_DAT" >>$DIR_AUTO/$FN_LIST
    done
    echo " ...done."
fi

FN_CONF=vertexing_${RAW_NAME}_${BG_MODE}.conf
echo -n "Making $DIR_AUTO/$FN_CONF... "
{
    echo "indir = "
    echo "outdir = $DIR_OUT"
    echo "opts = \$KTRACKER_ROOT/opts/mc_$OPT_NAME.opts"
    echo "osg"
    echo "localmap"
    echo "grid-args = '--expected-lifetime-eq-'short''"
} >$DIR_AUTO/$FN_CONF
echo " ...done."

submitAll.py -m -j vertex -l $DIR_AUTO/$FN_LIST -c $DIR_AUTO/$FN_CONF
