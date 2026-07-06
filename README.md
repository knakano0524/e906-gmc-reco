# e906-gmc-reco

A set of scripts to run the E906 GMC and kTracker.


## Configuration

The software versions of GMC and kTracker and the simulation settings are specified in `setup.sh`.
It is sourced in main scripts (such as `submit-gmc.sh`) so that all the configurations are commonly used.
You might source it at any time, to use `root` etc. interactively.

All output files are placed under `/pnfs/e906/scratch/users/$USER/data_production/GMC`.
`setup.sh` creates a symbolic link, `GMC`, pointing to the output directory for your convenience.


## GMC Production

You might edit `setup.sh` to change the production setting.
You might change `N_EVT_GEN` and `N_RUN_GEN` in `submit-gmc.sh` to adjust the amount of generated events.

If you haven't submitted any grid jobs recently (in a week?), you have to complete the authentication in advance by this command;
```
jobsub_submit -G seaquest file:///usr/bin/printenv
```

You then execute `submit-gmc.sh`:
```
./submit-gmc.sh
```

You can/should use `ana_event.cc` to confirm that generated events are appropriate.
you can run the following commands even if a part of jobs is still running:
```
./make-list-for-ana.sh
root -b ana_event.cc
```
The 1st command is to list up all available event files to be analyzed.


## Tracking

`submit-tracking.sh` is a script to
 * List up all available event files to be tracked,
 * Create a conf file for `submitAll.py`, and
 * Execute `submitAll.py` for tracking.

You simply execute it to submit tracking jobs:
```
./submit-tracking.sh
```

You can/should check tracked events:
```
./make-list-for-ana.sh
root -b ana_track.cc
```


## Vertexing

`submit-vertexing.sh` is a script for this step, similarly to the tracking.

You simply execute it to submit vertexing jobs:
```
./submit-vertexing.sh
```

You can/should check vertexed events:
```
./make-list-for-ana.sh
root -b ana_vertex.cc
root -b ana_both.cc
```

`ana_both.cc` is a macro to parse the event and vertex files together.


## To-Do List

* Rewrite `setup.sh` etc. so that multiple settings can be generated+reconstructed in parallel.
* Include generating and reconstructing messy MC.
