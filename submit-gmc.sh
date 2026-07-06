#!/bin/bash
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))

source $DIR_TOP/setup.sh
echo "RAW_NAME = $RAW_NAME"

PRESET=
case $RUN_MODE in
    run2 ) PRESET=run2 ;;
    run3 ) PRESET=run3 ;;
    run5 ) PRESET=run5 ;;
    * ) echo "ERROR: Unknown RUN_MODE ($RUN_MODE)." ; exit 0 ;;
esac
echo "PRESET = $PRESET"

TARGET=
case $TGT_MODE in
    lh2 ) TARGET=H ;;
    ld2 ) TARGET=D ;;
    * ) echo "ERROR: Unknown TGT_MODE ($TGT_MODE)." ; exit 0 ;;
esac
echo "TARGET = $TARGET"

ACC=
case $ACC_MODE in
    acc ) ACC=Acc ;;
    4pi ) ACC=4pi ;;
    * ) echo "ERROR: Unknown ACC_MODE ($ACC_MODE)." ; exit 0 ;;
esac
echo "ACC = $ACC"

N_EVT=100000
N_RUN=100
echo "N_EVT = $N_EVT"
echo "N_RUN = $N_RUN"

runGMC.py --grid --preset=$PRESET --Record=ROOT --server=e906-db1.fnal.gov \
	  --raw-name=$RAW_NAME \
	  --n-events=$N_EVT --n-subruns=$N_RUN --first-subrun=1 \
	  --Target=$TARGET --EventPosition=Target --Generator=DY --Acceptance=$ACC \
	  --gmc-devel
