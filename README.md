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

## How to Build
```shell
cd ~
git clone https://github.com/xza218/scheduler.git
cd scheduler/src
make -j
```
## How to Run (Just One Example)
```shell
cd ~/scheduler/src
./build/bin/scheduler < ../input/t12.txt
```

Feel free to use/modify the python script ```//input/gen.py``` to generate your own random input file.

## My Current Solution (in C++11 like Pseudo Code)
```c++
main () 
{
    job_queue.load();
    job_queue_cost_func = 
        [](job)
        {
            return (job.earliest + job.subtask_duration * ceil(job.num_subtasks / num_robots)) / priority;
        };
    job_queue.sort(
        [](job_rhs, job_rhs) 
        {
            return job_queue_cost_func(job_lhs) < job_queue_cost_func(job_rhs);
        });
    while (!job_queue.empty())
    {
        dispatcher_cost_func = [](job)
            {
                return workers.submit_job_and_get_eta(job, true) / priority;
            };
        least_cost_so_far = numeric_limits::max();
        best_job = null;
        for (job: job_queue)
        {
            cost = dispatcher_cost_func(job);
            if (cost < least_cost_so_far)
            {
                least_cost_so_far = cost;
                best_job = job;
            }            
            if (cost / 5.0 > least_cost_so_far)
            {
                break;
            }
        }
        workers.submit_job_and_get_eta(best_job, false);
    }
}

WORKERS::submit_job_and_get_eta(job, test_eta_only)
{
    job_end_time = numeric_limits::min();
    for (i: 0 to job.num_subtasks)
    {
        (best_worker, best_subtask_eta) = find_best_worker_and_subtask_eta(job);
            // Best worker is defined as the one with a hole in its subtask execution history, that fits
            // the subtask, and the start time of the hole is earlier than any other worker. The bottom
            // of all previously executed subtasks is also counted as a hole.
        job_end_time = max(job_end_time, best_subtask_eta);
        best_worker.submit(job);
            // This will let the worker store a list of executed subtasks, and their start & finish time.
            // The info will be used in all future job submissions, unless reverted.
    }
    if (test_eta_only)
    {
        revert_job_submissions(job);
    }
    return job_end_time;
    
    // TODO: optimize for start time (later the better) by aligning end time of all subtasks as possible.
}
```

## Benchmarking

t12.txt: 3.1504e+06
t15.txt: 6.39546e+06
