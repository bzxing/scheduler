# Scheduler

## Problem Statement
This is my solution to a past programming competition problem I found online. I rephrase it as follows:

Your program will read an input file that contains a list of jobs. Each job has 5 attributes:
* Name
* Priority
* Earliest allowed time to start
* Number of subjobs
* Time required to finish

Each job is divided into a fixed number of subjobs for you, each requires the same amount of time to complete. 

The input file also tells you how many robots you have. Each robot can process at most one subjob at any given time, but you can assign as many robots to execute as many subjobs from the same job as you wish. Once a robot starts to execute a subjob, it must run till completion without interruption. Robots are allowed to idle.

You're to write a program that generates a schedule (which robot executes which subjob at what time), that (tries to) optimize for the following cost function (the lower the better):

    SUM_OF_ALL_JOBS[  priority * sqrt( (completion_time-start_time)^2 + (completion_time-earliest_allowed_start_time)^2 )  ]

where completion_time is defined as when the last subjob of the job is done, and start_time is defined as when the first subjob of the job starts to execute.

Time value, duration and priority are represented by basic unsigned integers. Define 0 as genesis.

Run the python ```script /src/input/gen.py``` to generate random input file. To run the scheduler program, first run ```make``` under ```//src``` directory, then pipe a legal input file into it ```//src/build/bin/scheduler``` executable.
