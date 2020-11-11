from dataclasses import dataclass
from operator import itemgetter
from random import sample, choice
from itertools import groupby


@dataclass
class Task:
    duration: float
    dependencies: list
    id: str


@dataclass
class Agent:
    id: str
    performance: float


TASKS = [
    Task(1, [], "A"),
    Task(2, [], "B"),
    Task(3, [], "C"),
    Task(1, [], "A1"),
    Task(2, [], "B1"),
    Task(3, [], "C1"),
    Task(3, ["A"], "D"),
    Task(2, ["B"], "E"),
    Task(1, ["C"], "F"),
    Task(2, ["D", "E"], "G"),
    Task(1, ["F", "A"], "H"),
    Task(1, ["F", "A"], "H1"),
    Task(2, ["G", "H", "A"], "I"),
]

AGENTS = [Agent("A", 1.2), Agent("B", 0.8), Agent("C", 1.0)]


def make_state_and_ranks(tasks, agents):
    tasks = sample(tasks, len(tasks))

    def get_task(idx):
        for t in tasks:
            if t.id == idx:
                return t

    def get_rank(t):
        if len(t.dependencies) == 0:
            return 0
        return max(map(get_rank, map(get_task, t.dependencies))) + 1

    state = []

    for idx, t in enumerate(tasks):
        state.append([[t.id, choice(agents).id], get_rank(t)])
    state.sort(key=itemgetter(1))
    state_ranks = [v[1] for v in state]

    ranks = []

    for k, g in groupby(enumerate(state_ranks), itemgetter(1)):
        g = list(g)
        ranks.append(slice(g[0][0], g[-1][0] + 1))

    state = [v[0] for v in state]

    return state, ranks


state, ranks = make_state_and_ranks(TASKS, AGENTS)
print(ranks, state)
