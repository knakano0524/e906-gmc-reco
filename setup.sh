source /exp/seaquest/app/software/osg/software/AL9/setup.sh
source /exp/seaquest/app/software/osg/software/AL9/seaquest-dev/ktracker-0728/setup.sh
#source /exp/seaquest/app/software/osg/users/kenichi/e906/ktracker0803c/setup.sh

export GEN_MODE=dy
export RUN_MODE=rs59  # run2 rs59 rs62 run3 rs70 run5 run6
export TGT_MODE=lh2
export RAW_NAME_BASE=gmc_${GEN_MODE}_${TGT_MODE}_${RUN_MODE}
#export ACC_MODE=acc # 4pi acc
#export RAW_NAME=gmc_${GEN_MODE}_${TGT_MODE}_${RUN_MODE}_${ACC_MODE}
function SetRawName {
    local -r ACC_MODE=$1
    if [ "X$ACC_MODE" != 'X4pi' -a "X$ACC_MODE" != 'Xacc' ] ; then
	echo "SetRawName:  Invalid ACC_MODE ($ACC_MODE).  Abort." >/dev/stderr
	exit 1
    fi
    export RAW_NAME="${RAW_NAME_BASE}_${ACC_MODE}"
}

#export BG_MODE=messy # clean messy
function SetBgMode {
    local -r X=$1
    if [ "X$X" != 'Xclean' -a "X$X" != 'Xmessy' ] ; then
	echo "SetBgMode:  Invalid BG_MODE ($X).  Abort." >/dev/stderr
	exit 1
    fi
    export BG_MODE=$X
}

export DIR_AUTO=auto_file
export DIR_DATA_BASE=/pnfs/e906/scratch/users/$USER/data_production/GMC
ln -nfs $DIR_DATA_BASE GMC

ROADSET_ID=
case $RUN_MODE in
    run2 ) ROADSET_ID=57 ;;
    rs59 ) ROADSET_ID=59 ;;
    rs62 ) ROADSET_ID=62 ;;
    run3 ) ROADSET_ID=67 ;;
    rs70 ) ROADSET_ID=70 ;;
    run5 ) ROADSET_ID=78 ;;
    *    ) ROADSET_ID=0  ;;
esac
export ROADSET_ID

TGT_ID=
case $TGT_MODE in
    lh2 ) TGT_ID=1 ;;
    ld2 ) TGT_ID=3 ;;
    *   ) TGT_ID=0 ;;
esac
export TGT_ID

OPT_NAME=
case $RUN_MODE in
    run2 ) OPT_NAME=57_2 ;;
    rs59 ) OPT_NAME=59 ;;
    rs62 ) OPT_NAME=62 ;;
    run3 ) OPT_NAME=67 ;;
    rs70 ) OPT_NAME=70 ;;
    run5 ) OPT_NAME=5ea ;;
    #run2 ) OPT_NAME=57_2_AZE0 ;;
    #run3 ) OPT_NAME=67_AZE0 ;;
    #run5 ) OPT_NAME=5ea_AZE0 ;;
    *    ) OPT_NAME=NA ;;
esac
export OPT_NAME
