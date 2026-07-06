source /exp/seaquest/app/software/osg/software/AL9/setup.sh
#source /exp/seaquest/app/software/osg/software/AL9/seaquest-dev/ktracker/setup.sh
#source /exp/seaquest/app/software/osg/software/AL9/GMC-20260209/setup.sh
#source /exp/seaquest/app/software/osg/software/AL9/seaquest/NEW/ktracker/setup.sh
#source /exp/seaquest/app/software/osg/users/kenichi/e906/ktracker0609/setup.sh
#source /exp/seaquest/app/software/osg/users/kenichi/e906/ktracker0622i/setup.sh
source /exp/seaquest/app/software/osg/users/kenichi/e906/ktracker0705d/setup.sh

export GEN_MODE=dy
export RUN_MODE=run2  # run2 run3 run5
export TGT_MODE=lh2
export ACC_MODE=acc
export RAW_NAME=gmc_${GEN_MODE}_${TGT_MODE}_${RUN_MODE}_${ACC_MODE}

export BG_MODE=clean

export DIR_DATA_BASE=/pnfs/e906/scratch/users/$USER/data_production/GMC
ln -nfs $DIR_DATA_BASE GMC

ROADSET_ID=
case $RUN_MODE in
    run2 ) ROADSET_ID=57 ;;
    run3 ) ROADSET_ID=67 ;;
    run5 ) ROADSET_ID=78 ;;
    *    ) ROADSET_ID=0  ;;
esac
export ROADSET_ID

OPT_NAME=
case $RUN_MODE in
    run2 ) OPT_NAME=57_2_noreal ;;
    #run2 ) OPT_NAME=57_2 ;;
    run3 ) OPT_NAME=67 ;;
    run5 ) OPT_NAME=5ea ;;
    *    ) OPT_NAME=NA
esac
export OPT_NAME
