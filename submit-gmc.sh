#!/bin/bash
DIR_TOP=$(dirname $(readlink -f $BASH_SOURCE))
source $DIR_TOP/setup.sh

ACC_MODE=$1
SetRawName $ACC_MODE
echo "RAW_NAME = $RAW_NAME"

PRESET=
case $RUN_MODE in
    rs57 ) PRESET=run2 ;;
    rs59 ) PRESET=rs59 ;;
    rs62 ) PRESET=rs62 ;;
    rs67 ) PRESET=run3 ;;
    rs70 ) PRESET=rs70 ;;
    run5 ) PRESET=run5 ;;
    * ) echo "ERROR: Unknown RUN_MODE ($RUN_MODE)." ; exit 0 ;;
esac
echo "PRESET = $PRESET"

TARGET=
case $TGT_MODE in
    lh2 ) TARGET=H ;;
    ld2 ) TARGET=D ;;
    fe  ) TARGET=Fe ;;
    c   ) TARGET=C ;;
    w   ) TARGET=W ;;
    * ) echo "ERROR: Unknown TGT_MODE ($TGT_MODE)." ; exit 0 ;;
esac
echo "TARGET = $TARGET"

ACC=
case $ACC_MODE in
    acc ) ACC=Acc ;;
    4pi ) ACC=DimuonOnly ;;
    * ) echo "ERROR: Unknown ACC_MODE ($ACC_MODE)." ; exit 0 ;;
esac
echo "ACC = $ACC"

N_EVT_GEN=100000
N_RUN_GEN=200
RUN_1ST=1
echo "N_EVT_GEN = $N_EVT_GEN"
echo "N_RUN_GEN = $N_RUN_GEN"

runGMC.py --grid --preset=$PRESET --Record=ROOT --server=e906-db1.fnal.gov \
	  --raw-name=$RAW_NAME \
	  --n-events=$N_EVT_GEN --n-subruns=$N_RUN_GEN --first-subrun=$RUN_1ST \
	  --Target=$TARGET --EventPosition=Target --Generator=DY --Acceptance=$ACC \
	  --gmc-devel
