from dataclasses import dataclass
from operator import itemgetter
from random import sample, choice, uniform, randint
from itertools import groupby, chain
from pprint import pprint
from pickle import dump
import names
import humanize
from datetime import timedelta as td
from essential_generators import DocumentGenerator
from stringcase import sentencecase
import string

def to_letter(n):
    l = string.ascii_uppercase
    s = ''
    for i in reversed(range(0, 6)):
        v = n // 26**i
        if s or v > 0:
            s += l[v - 1 if i > 0 else v]
            n -= v*26**i
    return s or 'A'

# print(to_letter(0+0*26**1+1*26**2))
# print(to_letter(1))
# print(to_letter(0))
# print(to_letter(1*26**1))
# print(to_letter(25))
# print(to_letter(26))
# exit()
gen = DocumentGenerator()
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

print("agents:", end='')
groups = ['Design', 'Backend', 'Frontend']
agent_perfs = [uniform(.5,1.5) for i in range(10)]
for ap in agent_perfs:
    s = uniform(.1,.4)
    x = (ap-s, ap+s)
    grps = sample(groups, k=randint(0, 2))
    print(f"""
  {names.get_full_name()}:
    performance:
      min: {x[0]:.2f}
      max: {x[1]:.2f}""", end='')
    if grps:
        print("""
    groups:""", end='')
        for g in grps:
            print(f"""
      - {g}""", end='')

print("\ntasks:", end='')
for t in tasks:
    s = uniform(1,1.5)
    d = t.dur*3*60
    d_min = humanize.precisedelta(td(minutes=d/s), minimum_unit='minutes', format='%0.0f')
    d_max = humanize.precisedelta(td(minutes=d*s), minimum_unit='minutes', format='%0.0f')
    g = choice([None, *groups])
    name = gen.sentence()
    name = ''.join(n for n in name if n in string.ascii_letters+string.digits+' ')
    name = [s.encode('ascii').decode().strip() for s in name.split()]
    # print(name)
    name = ' '.join(filter(None,name))
    # name = sentencecase(name)
    print(f"""
  - name: {name}
    id: {to_letter(t.id)}
    duration:
      min: {d_min}
      max: {d_max}""", end='')
    if g:
        print(f"""
    group: {g}""", end='')
    if t.deps:
        deps = ', '.join([f'"{to_letter(i)}"' for i in t.deps])
        print(f"""
    depends_on: [{deps}]""", end='')
