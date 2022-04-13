#!/usr/bin/python
import csv
import argparse
import enum
import json
import os.path

import numpy as np
import pandas as pd
import jinja2

# Instantiate the parser
parser = argparse.ArgumentParser(description='Compiles an html timeline from a Job System CSV file')

parser.add_argument('file', type=str, help='The csv file to use')
parser.add_argument('--template', type=str, help='The mustache template file to use', default="timeline.html.j2")
args = parser.parse_args()

file = args.file
if file is None:
    parser.error("No CSV file given")
    exit(0)


class EventType(enum.Enum):
    WORKER_STARTED = 0
    WORKER_EXITED = 1
    WORKER_SLEEPING = 2
    WORKER_AWAKEN = 3
    JOB_STARTED = 4
    JOB_FINISHED = 5
    JOB_ENQUEUED = 6


data = pd.read_csv(file)
type_column = data["type"]


def compute_workloads(df: pd.DataFrame):
    is_job_started = type_column == EventType.JOB_STARTED.value
    is_job_finished = type_column == EventType.JOB_FINISHED.value
    only_jobs = df.loc[is_job_started | is_job_finished]
    only_jobs["handle"] = only_jobs["handle"].astype(np.int32)

    start_times = only_jobs[is_job_started][["handle", "time"]]
    end_times = only_jobs[is_job_finished][["handle", "time"]]

    times = pd.merge(start_times, end_times, on="handle", how="inner", suffixes=("_start", "_end"))
    jobs_df = only_jobs.loc[only_jobs["type"] == EventType.JOB_STARTED.value]
    jobs_trimmed = jobs_df[["handle", "archetype", "worker", "label"]]
    merge = pd.merge(jobs_trimmed, times, on="handle")
    return merge


def compute_worker_states(df: pd.DataFrame):
    only_workers = df.loc[(type_column != EventType.JOB_STARTED.value) & (type_column != EventType.JOB_FINISHED.value)]
    return only_workers[["archetype", "worker", "label", "type", "time"]]


workloads = compute_workloads(data)
states = compute_worker_states(data)
workloads_list = list()
for archetype, byArchetype in workloads.groupby("archetype"):
    archetypeDat = list()
    for worker, byWorker in byArchetype.groupby("worker"):
        archetypeDat.append(byWorker[["handle", "label", "time_start", "time_end"]].to_dict("records"))
    workloads_list.append(archetypeDat)

states_list = list()
for worker, byWorker in states.groupby("worker"):
    states_list.append(byWorker.to_dict("records"))

output = os.path.join(os.path.curdir, os.path.basename(file) + ".timeline.html")
templates = {
    "workloads": workloads_list,
    "states": states
}
print(f"Writing to {output} using {args.template}")
file_loader = jinja2.FileSystemLoader("templates")
env = jinja2.Environment(loader=file_loader)
template = env.get_template("timeline.html.j2")
render = template.render(workloads=workloads_list)

with open(output, "w+") as f:
    f.write(render)
