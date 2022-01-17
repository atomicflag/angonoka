import logging
import sys
import optuna
import subprocess

EXE = "../../build/src/angonoka-x86_64"

optuna.logging.get_logger("optuna").addHandler(logging.StreamHandler(sys.stdout))
study_name = "angonoka-hyperparameters"
storage_name = f"sqlite:///{study_name}.db"
study = optuna.create_study(study_name=study_name, storage=storage_name, load_if_exists=True)

def run(*args):
    r = subprocess.run(
        [EXE, "--log-optimization-progress", *args])
    # TODO: parse return code
    # TODO: parse optimization.csv, find makespan and num iters


def objective(trial):
    # TODO: run "run" with correct CLI parameters
    x = trial.suggest_float("x", -10, 10)
    # TODO: figure out how to combine makespan and num iters
    return (x - 2) ** 2


study.optimize(objective, n_trials=3)
print("Best params: ", study.best_params)
print("Best value: ", study.best_value)
print("Best Trial: ", study.best_trial)
