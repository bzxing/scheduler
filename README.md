# Scheduler

## Problem Statement
This is my (still ongoing) solution to a past programming competition problem I found online. I rephrase the problem as follows:

Your program will read an input file (from stdin) that contains a list of jobs you have to program a bunch of robots to finish. Each job has 5 attributes:
* Name
* Number of subtasks
* Time required for each subtask to finish
* Earliest allowed time for robots to start
* Priority

Each job is divided into a fixed number of subtasks for you. Each subtask under the same job requires the same amount of time to complete. These parameters are all constrained by the input file. Look into ```//input``` directory for a script that generate random input files, as well as some input file examples I generated for you.

The input file also tells you the number of robots available to you. Each robot can process at most one subtask at any given time. Any subtasks from any jobs can be executed in parallel. Once a robot starts to execute a subtask, it must run till the subtask completes without interruption. Robots are allowed to idle.

You'll write a program that generates a schedule (which robot executes which subtask at what time), that optimizes for the following cost function (the lower the better):

    SUM_OF_ALL_JOBS[  priority * sqrt( (completion_time-start_time)^2 + (completion_time-earliest_allowed_start_time)^2 )  ]

, where completion_time is defined as when the last subtask of the job is done, and start_time is defined as when the first subtask of the job starts to execute.

Time value, duration and priority are represented by basic unsigned integers. Define 0 as genesis.

Run the python script ```//input/gen.py``` to generate random input file. To run the scheduler program, first run ```make``` under ```//src``` directory, then pipe a legal input file into it ```//src/build/bin/scheduler``` executable.
