from dataclasses import dataclass
from operator import itemgetter
from random import sample, choice, uniform, randint
from itertools import groupby, chain
from pprint import pprint
from pickle import dump
# import networkx as nx
# import matplotlib.pyplot as plt
# from graphviz import Digraph


@dataclass
class Task:
    dur: float
    deps: list
    id: str
    agents: list[int]


@dataclass
class Agent:
    id: str
    performance: float

def make_ranks(ranks_count, rank_width=3, task_counter=0):
    if ranks_count == 0:
        return
    for i in range(rank_width):
        v = tuple(make_ranks(ranks_count-1, rank_width, task_counter))
        task_counter += len(v)
        yield from v
        if not v:
            deps = []
        else:
            deps = [t.id for t in sample(v, randint(1,rank_width))]
        yield Task(uniform(.02,.5), deps, task_counter, sample(range(10), randint(1,10)))
        task_counter += 1

def add_dependencies(tasks):
    for i, t in enumerate(tasks[1:], 1):
        deps = tasks[0:i]
        deps = sample(deps, randint(0, len(deps)))
        tasks[i].deps += [d.id for d in deps]
        tasks[i].deps = list(set(tasks[i].deps))

def sort(tasks):

    def get_task(idx):
        for t in tasks:
            if t.id == idx:
                return t

    ranks_b = {}
    def get_rank(t):
        if len(t.deps) == 0:
            return 0
        if t.id in ranks_b:
            return ranks_b[t.id]
        v = max(map(get_rank, map(get_task, t.deps))) + 1
        ranks_b[t.id] = v
        return v

    tasks = [(get_rank(t), t) for t in tasks]
    tasks.sort(key=itemgetter(0))
    return [t[1] for t in tasks]

def has_dep(tasks, dep):
    for t in tasks:
        if t.id == dep:
            return True
    raise Exception('Invalid dependency')

def validate(tasks):
    for i, t in enumerate(tasks):
        for d in t.deps:
            has_dep(tasks[0:i], d)

def is_swappable(a, b):
    return not a.id in b.deps


tasks = tuple(make_ranks(4, 3))
add_dependencies(tasks)
# tasks = sort(tasks)
# validate(tasks)

agent_perfs = [uniform(.5,1.5) for i in range(10)]

agent_performance = ",".join(str(a)+'F' for a in agent_perfs)
task_duration = ",".join(str(t.dur)+'F' for t in tasks)
available_agents_data = ",".join(map(str,chain(*(sorted(t.agents) for t in tasks))))
available_agents = ",".join(f'n({len(a.agents)})' for a in tasks)
dependencies_data = ",".join(map(str,chain(*(sorted(t.deps) for t in tasks))))
dependencies = ",".join(f'n({len(a.deps)})' for a in tasks)
state = ",".join(f'{{{t.id},{t.agents[0]}}}' for t in tasks)
print(f'''
ScheduleInfo info{{
    .agent_performance{{{agent_performance}}},
    .task_duration{{{task_duration}}},
    .available_agents_data{{{available_agents_data}}},
    .dependencies_data{{{dependencies_data}}}
}};
{{
    auto* h = info.available_agents_data.data();
    const auto n = [&](auto s) {{
        return span<int16>{{std::exchange(h, std::next(h, s)), s}};
    }};
    info.available_agents = {{{available_agents}}};
}}
{{
    auto* h = info.dependencies_data.data();
    const auto n = [&](auto s) {{
        return span<int16>{{std::exchange(h, std::next(h, s)), s}};
    }};
    info.dependencies = {{{dependencies}}};
}}

std::vector<StateItem> state{{{state}}};
''')
to_save = [agent_perfs, tasks]
with open("info.pickle", "bw") as f:
    dump(to_save, f)
exit()
