import pytest
import subprocess
from textwrap import dedent
import os

EXE = "../../build/src/angonoka-x86_64"
TEST_IDX = 0


def run(*args):
    global TEST_IDX
    env = {**os.environ, "LLVM_PROFILE_FILE": f"functional{TEST_IDX}.profraw"}
    TEST_IDX += 1
    r = subprocess.run(
        [EXE, *args], capture_output=True, universal_newlines=True, env=env
    )
    return r.returncode, r.stdout, r.stderr


def match(cout, text):
    __tracebackhide__ = True
    text = text.splitlines()
    while cout:
        if cout[0] == text[0]:
            break
        cout.pop(0)
    else:
        pytest.fail("No match found")
    while text and cout:
        assert text.pop(0) == cout.pop(0)


def test_prints_help():
    code, cout, cerr = run("--help")
    assert code == 0
    assert cout == dedent(
        """\
    Angonoka is a time estimation software based on statistical modeling.

    Usage: angonoka [OPTIONS] input file

    Positionals:
      input file TEXT REQUIRED    

    Options:
      -h,--help                   Print this help message and exit
      --version                   Display program version information and exit
      --color,--no-color{false}   Force colored output
      -q,--quiet                  Give less output
      -v,--verbose                Give more output

    """
    )


def test_version():
    code, cout, cerr = run("--version")
    assert code == 0
    assert cout == "angonoka version 0.6.0\n"

def test_version_with_file():
    code, cout, cerr = run("--version", "file.yaml")
    assert code == 0
    assert cout == "angonoka version 0.6.0\n"


def test_invalid_option():
    code, cout, cerr = run("--asdf")
    assert code == 106
    assert cerr == dedent(
        """\
    input file is required
    Run with --help for more information.
    """
    )


def test_invalid_option_with_version():
    code, cout, cerr = run("--asdf", "--version")
    assert code == 0
    assert cout == "angonoka version 0.6.0\n"


def test_basic_non_tty_output():
    code, cout, cerr = run("--no-color", "tasks.yml")
    assert code == 0
    cout = cout.splitlines()
    text = dedent(
        """\
    Parsing configuration... OK
    Optimizing the schedule...
    Optimization progress: 0.00%"""
    )
    match(cout, text)

    text = dedent(
        """\
    Schedule optimization complete.
    Optimal makespan: about an hour.
    Probability estimation complete.
    Done.
    """
    )
    match(cout, text)


def test_basic_tty_output():
    code, cout, cerr = run("--color", "tasks.yml")
    assert code == 0
    cout = cout.splitlines()
    text = dedent(
        """\
    Parsing configuration... OK
    Optimizing the schedule..."""
    )
    match(cout, text)

    l = cout.pop(0)
    assert l.startswith("\x1b[?25l")

    while cout:
        if "\x1b[?25h\x1b[2K\x1b[A" in cout[0]:
            break
        cout.pop(0)
    else:
        pytest.fail("Can't find the end of the progress bar")
    text = dedent(
        """\
    Optimizing the schedule... OK
    Optimal makespan: about an hour.
    Probability estimation complete.
    Done.
    """
    )
    match(cout, text)


def test_quiet_non_tty_output():
    code, cout, cerr = run("--no-color", "-q", "tasks.yml")
    assert code == 0
    assert cout == "Done.\n"


def test_verbose_non_tty_output():
    code, cout, cerr = run("--no-color", "-v", "tasks.yml")
    assert code == 0
    cout = cout.splitlines()
    text = dedent(
        """\
    Parsing configuration... OK
    Optimizing the schedule...
    Optimization progress: 0.00%"""
    )
    match(cout, text)

    text = dedent(
        """\
    Schedule optimization complete.
    Optimal makespan: 41m 24s.
    Probability estimation complete.
    Done.
    """
    )
    match(cout, text)


def test_missing_file():
    code, cout, cerr = run("--no-color", "missing.yml")
    assert code == 1
    assert cout == dedent(
        """\
    Parsing configuration... Error
    Error reading tasks and agents from file "missing.yml".
    """
    )


def test_missing_file_tty():
    code, cout, cerr = run("--color", "missing.yml")
    assert code == 1
    assert cout == dedent(
        """\
    Parsing configuration... \x1b[31mError
    \x1b[0m\x1b[31mError reading tasks and agents from file "missing.yml".
    \x1b[0m"""
    )


def test_invalid_yaml():
    code, cout, cerr = run("--no-color", "invalid.yml")
    assert code == 1
    assert cout == dedent(
        """\
    Parsing configuration... Error
    Validation error: "tasks" is expected to be a sequence.
    """
    )


def test_invalid_yaml_format():
    code, cout, cerr = run("--no-color", "invalid2.yml")
    assert code == 1
    assert cout == dedent(
        """\
    Parsing configuration... Error
    Error parsing YAML: Error at line 4, column 10: illegal map value
    """
    )


def test_abort():
    global TEST_IDX
    env = {**os.environ, "LLVM_PROFILE_FILE": f"functional{TEST_IDX}.profraw"}
    TEST_IDX += 1
    r = subprocess.run(
        f"timeout 1 {EXE} --color ../../tasks.yml",
        capture_output=True,
        universal_newlines=True,
        shell=True,
        env=env,
    )
    code, cout, cerr = r.returncode, r.stdout, r.stderr
    assert cout.endswith("\x1b[?25h")
