# Scheduler

## Problem Statement
This is my solution to a past programming competition problem I found online. I rephrase it as follows:

Your program will take an read file that contains a list of jobs. Each job has 4 attributes:
* Priority
* Earliest allowed time to start
* Number of subjobs
* Time required to finish

Each job is already divided into fixed number of subjobs, each requires the same amount of time to complete. 

You also read from the input file how many robots you have. Each robot can process at most one subjob at any given time, but you can assign as many robots to execute as many subjobs from the same job as you wish. Once a robot starts to execute a subjob, it must run till completion without interruption.

You're to write a program that comes up with a schedule (which robot executes which subjob at what time), that optimizes for the following cost function (the lower the better):

    SUM_OF_ALL_JOBS[  priority * sqrt( (completion_time-start_time)^2 + (completion_time-earliest_allowed_start_time)^2 )  ]

where completion_time is defined as when the last subjob of the job is done, and start_time is defined as when the first subjob of the job starts to execute.

Time value, duration and priority are represented by basic unsigned integers. Define 0 as genesis.

Run the python script /src/input/gen.py to generate random input file.
