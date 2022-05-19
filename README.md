# Angonoka

Angonoka is a time estimation software based on statistical modeling.

:warning: *This project is still in development.* :warning:

[![pipeline status](https://gitlab.com/signal9/angonoka/badges/master/pipeline.svg)](https://gitlab.com/signal9/angonoka/commits/master)
[![coverage report](https://gitlab.com/signal9/angonoka/badges/master/coverage.svg)](https://gitlab.com/signal9/angonoka/commits/master)
[![license](https://img.shields.io/badge/license-MIT-green)](/LICENSE)
[![lines of code](https://tokei.rs/b1/gitlab/signal9/angonoka?category=code)](https://github.com/XAMPPRocky/tokei)

```console
$ cat tasks.yaml
agents:
  Jennifer:
    groups:
      - Backend
  James The Intern:
    performance: 0.75
    groups:
      - Backend
  Linda:
    groups:
      - Frontend
tasks:
  - name: Develop backend
    group: Backend
    duration:
      min: 3 days
      max: 12 days
  - name: Develop frontend
    group: Frontend
    duration:
      min: 5 days
      max: 14 days

$ angonoka tasks.yaml
Parsing configuration... OK
Optimizing the schedule... OK
Optimal makespan: 10 days.
Estimation:
  25% chance to complete the project in under 8d 16h 30m.
  50% chance to complete the project in under 9d 22h 30m.
  75% chance to complete the project in under 11d 7h 30m.
  95% chance to complete the project in under 13d 7h 30m.
  99% chance to complete the project in under 14d 19h 30m.
Time estimation written to "time_estimation.json"
```

## Table of Contents
  * [Downloads](#downloads)
  * [Requirements](#requirements)
  * [Usage](#usage)
    * [Project configuration](#project-configuration)
    * [Agent](#agent)
    * [Task](#task)
    * [Schedule](#schedule)
    * [Time estimation](#time-estimation)
  * [Build instructions](#build-instructions)
  * [Contributing](#contributing)
  * [License](#license)

## Downloads

[Pre-built binaries](https://gitlab.com/signal9/angonoka/-/jobs/artifacts/master/browse?job=build-cpp) are available for Linux (`x86_64-unknown-linux-gnu`/`glibc`). This packaged version of Angonoka comes with all of the dependencies included.

## Requirements

To build Angonoka from source you need:

* [conan]
* C++20 compliant compiler

For the up-to-date list of runtime and build-time dependencies see [conanfile.txt](/conanfile.txt).

## Usage

The basic idea of Angonoka is to estimate how long a project would take to complete under given parameters and constraints. Specifically, you can set:

* Agents (could be people, computers, etc) and their relative performance
* Tasks with (rough) duration estimates
* Task dependencies/subtasks
* Task groups (only agents from the same group can work on them)

Agent performances and task durations can be set as a pair of min/max values, for when there is an uncertainty about the true value of the parameter.

### Project configuration

Agents and tasks are defined in YAML format. The minimal configuration must include at least 1 agent and 1 task:

```yaml
agents:
  Agent Name:
tasks:
  - name: Task name
    duration: 5 min
```

### Agent

Each agent section can have the following parameters:

```yaml
Agent Name:
  performance: 1.0 # default 1.0
  # or
  performance:
    min: 0.5
    max: 1.5
  groups: # default []
    - Group 1
    - Group 2
    # ...
```

**performance** is a multiplier applied to the task's duration. For example an agent with a performance value of 2 would complete a 10 min task in 5 minutes.

**groups** is an array of group labels that the agent belongs to. This parameter restricts tasks available to the agent. An agent with `groups` can either work on tasks belonging to one of the groups or tasks without groups.

The use case for the `groups` parameter is to allow you to specify that certain agents (i.e. backend developers) are the only ones who can work on certain tasks (i.e. backend tasks).

An agent without `groups` can work on any task (including tasks from groups).

Example:

```yaml
agents:
  Full Stack Developer:
  Frontend Developer:
    groups:
      - Frontend
  Backend Developer:
    groups:
      - Backend
tasks:
  - name: Make coffee
    duration: 5 min
  - name: Develop backend
    duration: 1 day
    group: Backend
  - name: Develop frontend
    duration: 1 day
    group: Frontend
```

All 3 agents can `Make coffee`.  
Only `Full Stack Developer` and `Frontend Developer` can work on `Develop frontend`.  
Only `Full Stack Developer` and `Backend Developer` can work on `Develop backend`.

### Task

Each task can have the following parameters:

```yaml
name: Task name # required
duration: 15 min # required
# or
duration:
  min: 5 min
  max: 30 min
id: TaskID # default ""
group: Task group # default ""
# or
groups:
  - Group 1
  - Group 2
# or
agent: Agent 1
subtasks: # default []
  - # Tasks
depends_on: TaskID # default ""
# or
depends_on:
  - TaskID1
  - TaskID2
```

A bare minimum task must have a name and a duration, all other parameters are optional.

**name** can be any text, doesn't have to be unique.

**duration** defines how long this task is expected to take. It should be chosen with respect to the average agent's performance, i.e. the performance value of 1. Most human-readable durations are accepted, "5 min", "24h", "3 weeks 2 days 42 seconds", etc.

> Note: To better estimate the makespan all task durations should have the same unit of time. The reason for this is that if one task is defined as "3 hours" and another as "3 days", the algorithm doesn't account for the fact that the "3 days" task doesn't literally take 3 days to complete but rather it is implied that the actual duration is 8 work hours over 3 days, i.e. 24 hours or 1 day.

**id** uniquely identifies a task. This parameter is only needed when you want to reference this task in `depends_on` of another task.

**group**/**groups** is one or more groups that this task belongs to. If you assign multiple groups, only agents that belong to all listed groups will work on this task.

Example:

```yaml
agents:
  Senior Developer:
    groups:
      - Seniors
      - Developers
  Junior Developer:
    groups:
      - Developers
tasks:
  - name: A very difficult task
    duration: 45 min
    groups:
      - Developers
      - Seniors
  - name: An easy task
    duration: 5 min
    groups:
      - Developers
```

`Senior Developer` and `Junior Developer` can work on `An easy task`.  
Only `Senior Developer` can work on `A very difficult task`.


**agent** can be set when you want this task to be assigned to a specific agent. Only one of `group`, `groups` or `agent` can be set at the same time.

**subtasks** is an array of tasks that this task implicitly depends on. Meaning, tasks from this list will be scheduled before the parent task. You don't have to assign `id`s to subtasks.

**depends_on** can be set when you want this task to be scheduled after the listed tasks. Note that you have to use task ids and not task names.

### Schedule

Once you have defined a project configuration you can make Angonoka find an optimal schedule for you. Scheduling, being an NP-hard problem, is generally hard to optimize, however Angonoka tries to find the best schedule possible in the shortest amount of time.

To find a schedule you can invoke Angonoka CLI like so:

```console
$ angonoka schedule project_configuration.yml
Parsing configuration... OK
Optimizing the schedule... OK
Optimal makespan: 3 days.
Saving the optimized schedule to "schedule.json".
```

This will output a schedule to `schedule.json` in the following format:

```js
{
    "makespan": 226278,
    "tasks": [
        {
            "agent": "Joshua Snell",
            "expected_duration": 517,
            "expected_start": 0,
            "priority": 0,
            "task": "Task name"
        },
        // ...
    ]
}
```

Note that all durations and performances are averaged. Angonoka doesn't take into account performance or duration variability during the schedule optimization process.

**makespan** is the total project runtime in seconds.

**expected_duration** is how long a task is expected to take in seconds.

**expected_start** is when the task is expected to be scheduled with relation to the start of the project.

**priority** is used to sort the tasks. Lower numbers should be scheduled before higher numbers.

To get a better view of what the schedule would look like you can use the [schedule visualizer](https://signal9.gitlab.io/angonoka/visualizer/).

### Time estimation

Time estimation side of Angonoka builds a histogram of likely completion times. It makes a statistical model and runs the simulation enough times to estimate the most likely durations, taking into account performance and task duration variations.

To output a histogram:

```console
$ angonoka project_configuration.yml
Parsing configuration... OK
Optimizing the schedule... OK
Optimal makespan: 10 days.
Estimation:
  25% chance to complete the project in under 8d 16h 30m.
  50% chance to complete the project in under 9d 22h 30m.
  75% chance to complete the project in under 11d 7h 30m.
  95% chance to complete the project in under 13d 7h 30m.
  99% chance to complete the project in under 14d 19h 30m.
Time estimation written to "time_estimation.json"
```

This will output a histogram, stats and the optimized schedule to `time_estimation.json` in the following format:

```js
{
    "histogram": {
        "bucket_size": 3599,
        "buckets": [
            [
                197999,
                1
            ],
            // ...
        ]
    },
    "makespan": 820800,
    "stats": {
        "p25": 750600,
        "p50": 858600,
        "p75": 977400,
        "p95": 1150200,
        "p99": 1279800
    },
    "tasks": [
        {
            "agent": "Jennifer",
            "expected_duration": 648000,
            "expected_start": 0,
            "priority": 0,
            "task": "Develop backend"
        },
        // ...
    ]
}
```

For **makespan** and **tasks** see [Schedule](#schedule).

**histogram.bucket_size** is the size of each bucket in seconds.

**histogram.buckets** is an array of buckets where each bucket is `[lower value, count]`. 

**stats** contains some commonly used quantiles for the histogram.

## Build instructions

Angonoka uses [conan] package manager and [meson] build system. The latter is provided by conan so you don't have to install both.

To build the release version of Angonoka from source:

```console
$ mkdir build && cd build
$ conan remote add signal9 https://signal9.jfrog.io/artifactory/api/conan/conan
$ conan install ..
$ . ./activate.sh
$ export PKG_CONFIG_PATH=$(pwd)
$ meson --buildtype release ..
$ ninja
$ ninja install # optionally
```

See [meson_options.txt](/meson_options.txt) for additional build-time options.

In principle, you can build Angonoka without conan, using system dependencies if you provide meson with compatible pkg-config files.

A proper `conanfile.py` is planned. This addition would allow anyone to build Angonoka with a single command, without having to deal with meson/ninja.

## Contributing

CI, coverage, tests, linting, etc. all use an [LLVM 13+][llvm] toolchain. You can pull a pre-built [Docker][docker] image from [signal9/cpp-env](https://gitlab.com/signal9/cpp-env) repo:

```console
$ docker pull registry.gitlab.com/signal9/cpp-env:13.0.0
```

Angonoka uses a [Makefile](/Makefile) for CI builds which can be reused for local development. See the rule definitions and adapt accordingly if you choose to use a non-LLVM toolchain since the Makefile is only compatible with LLVM.

Make sure that all tests are green before opening a pull request:

```console
$ make test # Unit tests
$ make check # Formatting, linting
$ make test/functional # Functional tests, requires pytest
```

Fixing the formatting with `clang-format`:

```console
$ make format
```

## License

Angonoka is licensed under the [MIT](/LICENSE) license.  
Copyright &copy; 2019, Andrew

[conan]: https://conan.io/
[meson]: https://mesonbuild.com/
[llvm]: https://llvm.org/
[docker]: https://www.docker.com/

<!--
https://gist.github.com/rowanmanning/77f31b2392dda1b58674#file-readme-md
-->
