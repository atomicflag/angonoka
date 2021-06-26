#!/bin/bash -u

#
# UTILS
#

EXE=../../build/src/angonoka-x86_64

TEST_INDEX=0

cd "$(dirname "${BASH_SOURCE[0]}")"

run() {
  export LLVM_PROFILE_FILE=functional$TEST_INDEX.profraw
  TEST_INDEX=$(($TEST_INDEX + 1))
  REF="$(cat)"
  OUTPUT="$($EXE "$@" 2>&1 | cat -A && return ${PIPESTATUS[0]})"
  EC=$?
  if ! diff -d <(echo "$REF") <(echo "$OUTPUT"); then
    echo "Line $BASH_LINENO: FAIL."
    exit 1
  fi
  echo OK $(basename $EXE) "$@"
  return $EC
}

runc() {
  base64 -d | bzip2 -d | run "$@"
  TEST_INDEX=$(($TEST_INDEX + 1))
}

require_status() {
  STATUS="$?"
  if [[ "$STATUS" != "$1" ]]; then
    echo "Line $BASH_LINENO: FAIL. Expected: $1, Actual: $STATUS"
    exit 1
  fi
}

compress() {
  echo -n "$($EXE "$@" 2>&1 | cat -A | bzip2 -9 | base64 -w72)"
}

#
# TEST SUITE
#

#
# Prints help
#
run --help <<EOF
Angonoka is a time estimation software based on statistical modeling.$
$
Usage: angonoka [OPTIONS] input file$
$
Positionals:$
  input file TEXT REQUIRED    $
$
Options:$
  -h,--help                   Print this help message and exit$
  --version                   Display program version information and exit$
  --color,--no-color{false}   Force colored output$
  -q,--quiet                  Give less output$
  -v,--verbose                Give more output$
$
EOF
require_status 0

#
# Prints version
#
run --version <<EOF
angonoka version 0.3.0$
EOF
require_status 0

#
# Invalid option
#
run --asdf <<EOF
input file is required$
Run with --help for more information.$
EOF
require_status 106

#
# Invalid option but requested version
#
run --asdf --version <<EOF
angonoka version 0.3.0$
EOF
require_status 0

#
# Basic non-TTY output
#
run --no-color tasks.yml <<EOF
Parsing configuration... OK$
Optimizing the schedule...$
Optimization progress: 0.00%$
Schedule optimization complete.$
Probability estimation complete.$
Done.$
EOF
require_status 0

#
# Basic TTY output
#
runc --color tasks.yml <<EOF
QlpoOTFBWSZTWX3p6zUAAnDfgAAQRgNyAKRKwQs/594wMADRAkqZTJ6TaQ9TNQ0B6mmnqPUE
VCNRoaaDQaMhoAGGhkyBkYgxMmhpiKbUQZKohaKDkJig44fcCJTbCEKxQakQdyisiZAMsShE
GmyOwr1kHVcgnAouQTmgk7gEnUWVMkExoBBmEJBihAtfJAjKpAiVlsX9s/jkCNJTNhkPPvXE
aVsaE87DQnBquoMLYDqFLggipYXsPzHSHcY4FsRhC8rH0J8Gn8XckU4UJB96es1A
EOF
require_status 0

#
# Quiet non-TTY output
#
run --no-color -q tasks.yml <<EOF
Done.$
EOF
require_status 0

#
# Verbose non-TTY output
#
run --no-color -v tasks.yml <<EOF
Parsing configuration... OK$
Optimizing the schedule...$
Optimization progress: 0.00%$
Schedule optimization complete.$
Probability estimation complete.$
Done.$
EOF
require_status 0

#
# Missing file
#
run --no-color missing.yml <<EOF
Parsing configuration... Error$
Error reading tasks and agents from file "missing.yml".$
EOF
require_status 1

#
# Missing file, TTY output
#
run --color missing.yml <<EOF
Parsing configuration... ^[[31mError$
^[[0m^[[31mError reading tasks and agents from file "missing.yml".$
^[[0m
EOF
require_status 1

#
# Invalid YAML
#
run --no-color invalid.yml <<EOF
Parsing configuration... Error$
Validation error: "tasks" is expected to be a sequence.$
EOF
require_status 1

#
# Invalid YAML, format error
#
run --no-color invalid2.yml <<EOF
Parsing configuration... Error$
Error parsing YAML: Error at line 4, column 10: illegal map value$
EOF
require_status 1

#
# Aborted
#
{
  export LLVM_PROFILE_FILE=functional$TEST_INDEX.profraw
  TEST_INDEX=$(($TEST_INDEX + 1))
  REF="$(cat)"
  END="$($EXE --color ../../tasks.yml 2>&1 | cat -A | tail -c7)"
  [[ "$END" == "^[[?25h" ]] || { echo "Line $BASH_LINENO: FAIL. The cursor wasn't reset"; exit 1; }
  echo OK $(basename $EXE) --color ../../tasks.yml
} &
sleep 1
pkill $(basename $EXE)
wait
