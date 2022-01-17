import logging
import sys
import optuna
import subprocess
from pathlib import Path
from itertools import chain

EXE = "../../build/src/angonoka-x86_64"

optuna.logging.get_logger("optuna").addHandler(logging.StreamHandler(sys.stdout))
study_name = "angonoka-hyperparameters"
storage_name = f"sqlite:///{study_name}.db"
study = optuna.create_study(
    study_name=study_name, storage=storage_name, load_if_exists=True
)


def parameters(trial):
    batch_size = trial.suggest_int("batch_size", 10, 500000)
    params = [
        ["--batch-size", batch_size],
        ["--max-idle-iters", batch_size * trial.suggest_int("max_idle_iters", 1, 150)],
        ["--beta-scale", trial.suggest_float("beta_scale", 1e-7, 1)],
        ["--stun-window", trial.suggest_int("stun_window", 10, 500000)],
        ["--gamma", trial.suggest_float("gamma", 0.01, 0.99)],
        ["--restart-period", 2 ** trial.suggest_int("restart-period", 5, 28)],
    ]
    return [str(a) for a in chain(*params)]


def run(args):
    r = subprocess.run([EXE, *args, "--log-optimization-progress", "tasks.yml"])
    assert r.returncode == 0
    log = Path("optimization_log.csv").read_text().splitlines()[-1]
    makespan, epoch = [int(a) for a in log.split(",")[1:3]]
    return makespan


def objective(trial):
    params = parameters(trial)
    return run(params)


study.optimize(objective)
print("Best params: ", study.best_params)
print("Best value: ", study.best_value)
print("Best Trial: ", study.best_trial)
