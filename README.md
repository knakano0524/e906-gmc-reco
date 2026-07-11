# e906-gmc-reco

A set of scripts to run the E906 GMC and kTracker.
It uses the offical version of GMC and kTracker under `/exp/seaquest/app/software/osg/software/AL9/`.


## Configuration

The software versions of GMC and kTracker and the simulation settings are specified in `setup.sh`.
It is sourced in main scripts (such as `submit-gmc.sh`) so that all the configurations are commonly used.
You might source it at any time, to use `root` etc. interactively.

All output files are placed under `/pnfs/e906/scratch/users/$USER/data_production/GMC`.
`setup.sh` creates a symbolic link, `GMC`, pointing to the output directory for your convenience.
The following sub-directories are used by default:
    | Sub-Directory               | Stored Files                   |
    | --------------------------- | ------------------------------ |
    | GMC/data/                   | Raw (GMC-produced) event files |
    | GMC/clean/		  | Clean-event files              |
    | GMC/messy/		  | Messy-event files	           |
    | GMC/track/RAW_NAME/clean/	  | Tracked clean-event files      |
    | GMC/track/RAW_NAME/messy/	  | Tracked messy-event files      |
    | GMC/vertex/RAW_NAME/clean/  | Vertexed clean-event files     |
    | GMC/vertex/RAW_NAME/messy/  | Vertexed messy-event files     |


## Program Flow

```
  Event Production                 --- submit-gmc.sh
    |
  Chamber Realization              --- exec-realization.sh
  [Clean-Event Files]
    |             |
    |           Hit Embedding      --- exec-embedding.sh
    |           [Messy-Event Files]
    |             |
  Tracking      Tracking           --- submit-tracking.sh
  (Clean)       (Messy)
    |             |
  Vertexing     Vertexing          --- submit-vertexing.sh
  (Clean)       (Messy)
    |             |
  Event Analysis (Clean & Messy)
```


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


## Chamber Realization with Event Filtering

This step is to
* Discard events in which the hodo and chamber planes have too few hits, and
* Implement the chamber realization.
The 1st function is helpful in speeding up the later steps and also reducing the number of NIM3 events embedded multiple times.

You simply execute this command:
```
./exec-realization.sh
```
It processes all event files locally (not on GRID).
If you stop the process halfway, you can execute the same command to resume it.
The two functions are programmed in `seaquest-ktracker/exe/filtering_and_realization.cxx`.


## Hit Embedding

This step is to embed NIM3 events into MC (signal) events.
The NIM3 events to be embedded are already prepared and selected in `exec-embedding.sh`.

You simply execute this command:
```
exec-embedding.sh
```
A main function is programmed in `embedding.cc`.
You can find details of the embedding algorithm in it.


## Tracking

`submit-tracking.sh` is a script to
 * List up all available event files to be tracked,
 * Create a conf file for `submitAll.py`, and
 * Execute `submitAll.py` for tracking.

You simply execute it to submit tracking jobs:
```
./submit-tracking.sh
```

One job usually takes 1-2 hours for Clean and 3-12 hours for Messy.

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
* Import `seaquest-ktracker/exe/filtering_and_realization.cxx` into this package, so that users can modify it easily.  New LinkDef files for some kTracker classes are needed.
