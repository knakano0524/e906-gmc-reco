#!/bin/bash
## Script to run the hit embedding.
## It is similar to `seaquest-ktracker/scripts/embed/process_embed_inte.sh` etc.,
## but automatically sets the parameters and calls `embedding.cc` in this directory.
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

source $DIR_TOP/setup.sh

TGT_ID=
case $TGT_MODE in
    lh2 ) TGT_ID=1 ;;
    ld2 ) TGT_ID=3 ;;
    *   ) TGT_ID=0 ;;
esac

echo "RAW_NAME      = $RAW_NAME"
echo "ROADSET_ID    = $ROADSET_ID"
echo "TGT_MODE      = $TGT_MODE"
echo "TGT_ID        = $TGT_ID"
echo "DIR_NIM3_DATA = $DIR_NIM3_DATA"
root -b "embedding.cc(\"$ROADSET_ID\", $TGT_ID, \"$RAW_NAME\", \"$DIR_DATA_BASE\")"
