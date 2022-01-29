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
      min: 1 weeks
      max: 3 weeks
  - name: Develop frontend
    group: Frontend
    duration:
      min: 5 days
      max: 14 days

$ angonoka tasks.yaml
Parsing configuration... OK
Optimizing the schedule... OK
Optimal makespan: 14 days.
Probability estimation complete.
Done.
```

## Table of Contents

  * [Requirements](#requirements)
  * [Usage](#usage)
    * [Project configuration](#project-configuration)
    * [Agent](#agent)
    * [Task](#task)
  * [Contributing](#contributing)
  * [License](#license)

## Requirements

[Pre-built binaries](https://gitlab.com/signal9/angonoka/-/pipelines?page=1&scope=branches&ref=master) are available for Linux (`x86_64-unknown-linux-gnu`/`glibc`). This packaged version of Angonoka comes with all of the dependencies included.

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

The idea behind `groups` parameter is to allow you to specify that certain agents (i.e. backend developers) are the only ones who can work on certain tasks (i.e. backend tasks).

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
```

*WIP: Add release links, building instructions...*

## Contributing

WIP

## License

Angonoka is licensed under the [MIT](/LICENSE) license.  
Copyright &copy; 2019, Andrew

[conan]: https://conan.io/

<!--
https://gist.github.com/rowanmanning/77f31b2392dda1b58674#file-readme-md
-->
