from dataclasses import dataclass
from operator import itemgetter
from random import sample, choice, uniform, randint
from itertools import groupby, chain
from pprint import pprint
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
tasks = sort(tasks)
validate(tasks)
# print(len(tasks))
# exit()
# print(tasks)
agent_performance = ",".join(str(uniform(.5,1.5))+'F' for i in range(10))
task_duration = ",".join(str(t.dur)+'F' for t in tasks)
available_agents_data = ",".join(map(str,chain(*(t.agents for t in tasks))))
available_agents = ",".join(f'next_aa({len(a.agents)})' for a in tasks)
dependencies_data = ",".join(map(str,chain(*(t.deps for t in tasks))))
dependencies = ",".join(f'next_dep({len(a.deps)})' for a in tasks)
state = ",".join(f'{{{t.id},{t.agents[0]}}}' for t in tasks)
print(f'''
ScheduleInfo info{{
    .agent_performance{{{agent_performance}}},
    .task_duration{{{task_duration}}},
    .available_agents_data{{{available_agents_data}}},
    .dependencies_data{{{dependencies_data}}}
}};
auto* aa_head = info.available_agents_data.data();
const auto next_aa = [&](auto s) {{
    return span<int16>{{std::exchange(aa_head, std::next(aa_head, s)), s}};
}};
info.available_agents = {{{available_agents}}};
auto* dep_head = info.dependencies_data.data();
const auto next_dep = [&](auto s) {{
    return span<int16>{{std::exchange(dep_head, std::next(dep_head, s)), s}};
}};
info.dependencies = {{{dependencies}}};

std::vector<StateItem> state{{{state}}};
''')
exit()

def mutate(tasks):
    i = randint(1, len(tasks)-1)
    to_swap = i-1
    if not is_swappable(tasks[to_swap], tasks[i]):
        return False
    tasks[i], tasks[to_swap] = tasks[to_swap], tasks[i]
    return True

def makespan(tasks):
    task_done = {t.id:0 for t in tasks}
    work = [0]*2
    for t in tasks:
        prev = [task_done[d] for d in t.deps]
        prev += [work[t.agent]]
        prev = max(prev)
        work[t.agent] = task_done[t.id] = prev + t.dur
    return max(work)

m = 9999999
for i in range(100000):
    # print(mutate(tasks))
    # pprint(('before', [t.id for t in tasks]))
    v = mutate(tasks)
    # pprint(('after', [t.id for t in tasks]))
    # validate(tasks)
    nm = makespan(tasks)
    if nm < m:
        m = nm
        print(m)
    # print(v, makespan(tasks))

# G = Digraph(format='png')
# for t in tasks:
#     for d in t.deps:
#         G.edge(t.id, d)
#
# G.render('graph')
