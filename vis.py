import matplotlib.pyplot as plt
from pickle import load
from dataclasses import dataclass
from random import sample

@dataclass
class Task:
    dur: float
    deps: list
    id: str
    agents: list[int]

with open("info.pickle", "rb") as f:
    agent_perfs, tasks = load(f)

# state = [(t.id, sample(t.agents, k=1)[0]) for t in tasks]
state = ((0, 2), (1, 0), (2, 2), (3, 2), (5, 2), (4, 0), (6, 7), (7, 2), (9, 2), (8, 7), (15, 7), (14, 6), (17, 6), (10, 2), (21, 5), (18, 2), (12, 1), (11, 7), (13, 2), (16, 2), (22, 7), (23, 2), (19, 1), (20, 1), (27, 7), (25, 1), (26, 8), (28, 7), (24, 4), (29, 7), (30, 7), (31, 7), (33, 2), (32, 1), (36, 2), (34, 7), (35, 2), (38, 1), (37, 2), (39, 3), (41, 3), (40, 2), (42, 7), (43, 2), (107, 0), (44, 2), (45, 2), (46, 2), (47, 1), (48, 2), (50, 1), (49, 1), (51, 7), (58, 6), (52, 7), (55, 1), (53, 2), (54, 2), (56, 2), (59, 4), (57, 2), (60, 2), (68, 1), (71, 9), (62, 7), (63, 0), (88, 0), (61, 7), (64, 1), (65, 2), (66, 2), (70, 0), (72, 7), (67, 7), (69, 1), (73, 2), (86, 3), (75, 2), (76, 7), (74, 1), (77, 2), (78, 1), (79, 2), (80, 2), (81, 7), (82, 7), (83, 7), (84, 1), (95, 1), (85, 7), (87, 7), (89, 7), (90, 7), (91, 7), (92, 7), (93, 0), (94, 2), (98, 3), (96, 2), (97, 2), (99, 2), (100, 7), (101, 4), (102, 2), (103, 2), (104, 0), (105, 7), (106, 1), (108, 7), (109, 7), (111, 6), (110, 7), (112, 1), (113, 6), (114, 2), (115, 2), (116, 2), (117, 0), (118, 7), (119, 2))

fig = plt.figure()
ax = fig.add_subplot(111)
ax.axes.get_yaxis().set_visible(False)
ax.set_aspect(1)

def avg(a, b):
    return (a + b) / 2.0

def draw(agent_id, task_id, start, duration):
    x1 = [start,start+duration]
    y1 = [agent_id,agent_id]
    y2 = [agent_id+1,agent_id+1]
    plt.fill_between(x1, y1, y2=y2)
    # plt.text(avg(x1[0], x1[1]), avg(y1[0], y2[0]), str(task_id), 
    #                         horizontalalignment='center',
    #                         verticalalignment='center')

def get_task(idx):
    for t in tasks:
        if t.id == idx:
            return t

agent_done = [0]*len(agent_perfs)
task_done = {}

for t, a in state:
    start = agent_done[a]
    task = get_task(t)
    # print('AA', t, task.deps)
    if task.deps:
        start = max(start,max(map(task_done.__getitem__, task.deps)))
    duration = task.dur / agent_perfs[a]
    agent_done[a] = task_done[t] = start + duration
    draw(a,t,start,duration)
# draw(0,0,1,1)

# plt.ylim(3, 0)
plt.show()
