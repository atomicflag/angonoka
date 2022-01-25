import logging
from optuna.samplers import TPESampler
import sys
import optuna
import subprocess
from pathlib import Path
from itertools import chain
import time

EXE = "../../build/src/angonoka-x86_64"

optuna.logging.get_logger("optuna").addHandler(logging.StreamHandler(sys.stdout))
study_name = "angonoka-hyperparameters"
storage_name = f"sqlite:///{study_name}.db"
study = optuna.create_study(
    study_name=study_name,
    storage=storage_name,
    load_if_exists=True,
    sampler=TPESampler(multivariate=True),
)


def parameters(trial):
    batch_size = 10 ** trial.suggest_int("batch_size", 1, 6)
    params = [
        ["--batch-size", batch_size],
        [
            "--max-idle-iters",
            int(batch_size * 100 * (trial.suggest_int("max_idle_iters", 1, 5) / 5)),
        ],
        ["--beta-scale", eval("1e" + str(trial.suggest_int("beta_scale", -7, 0)))],
        ["--stun-window", 100],
        ["--gamma", trial.suggest_int("gamma", 1, 100) / 10],
        ["--restart-period", 256],
    ]
    return [str(a) for a in chain(*params)]


def run(args):
    for a in args:
        print(a)
    then = time.monotonic()
    r = subprocess.run([EXE, *args, "--log-optimization-progress", "tasks.yml"])
    assert r.returncode == 0
    now = time.monotonic()
    log = Path("optimization_log.csv").read_text().splitlines()[-1]
    makespan, epoch = [int(a) for a in log.split(",")[1:3]]
    print(makespan)
    return ((makespan - 3200) / 50 + (now - then) / 3) / 2


def objective(trial):
    params = parameters(trial)
    return sum([run(params) for i in range(5)])


study.optimize(objective)
# fig = optuna.visualization.plot_slice(study)
# fig.show()
print("Best params: ", study.best_params)
print("Best value: ", study.best_value)
print("Best Trial: ", study.best_trial)
