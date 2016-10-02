#!/usr/bin/env python2.7
import random

"""
Define a job
"""
class Job:
  def __init__(self, name, can_begin, tasks, task_time, priority):
    self.name = name
    self.can_begin = can_begin
    self.tasks = tasks
    self.task_time = task_time
    self.priority = priority

  def __repr__(self):
    return "job {} {} {} {} {}".format(
      self.name, self.tasks, self.task_time, self.can_begin, self.priority)

"""
Generate a set of jobs and workers. There are a lot of ways to configure this
"""
def generate_jobs(seed=1, max_jobs=200, min_jobs=200, max_tasks=200, max_task_time=50, max_can_begin_time=800, max_priority=20,
  num_jobs=200, task_time=None, can_begin_time=None, priority=None):
  jobs=[]

  random.seed(seed)
  #how many jobs?
  if not num_jobs:
    num_jobs = random.randrange(min_jobs, max_jobs)

  for jb in xrange(0, num_jobs):
    name = 'job_{}'.format(jb)

    #figure out how many tasks to have
    tasks = random.randrange(0, max_tasks) + 1

    if not task_time:
      job_task_time = random.randrange(1, max_task_time + 1)
    else:
      job_task_time = task_time

    if can_begin_time == None:
      job_can_begin_time = random.randrange(0, max_can_begin_time)
    else:
      job_can_begin_time = can_begin_time

    if not priority:
      job_priority = random.randrange(1, max_priority)
    else:
      job_priority = priority

    jobs.append(Job(name, job_can_begin_time, tasks, job_task_time, job_priority))

  #normalize start times (make sure at least one job starts at 0)
  min_start = jobs[0].can_begin
  for job in jobs[1:]:
    if job.can_begin < min_start:
      min_start = job.can_begin

  for job in jobs:
    job.can_begin = job.can_begin - min_start

  #make sure there are more jobs than workers to keep things interesting
  num_workers = random.randrange(max_jobs / 4, max_jobs / 2)
  #give the workers names! These are not good names but they are names
  workers = []
  for wrk in xrange(0, num_workers):
    name = 'android_{}'.format(wrk)
    workers.append(name)

  return jobs, workers

"""
write to stdout
"""
def print_problem(jobs, workers):
  for job in jobs:
    print(job)

  for worker in workers:
    print("worker {}".format(worker))

if __name__ == "__main__":
  jobs, workers = generate_jobs()
  print_problem(jobs, workers)
