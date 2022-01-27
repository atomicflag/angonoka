# Angonoka

Angonoka is a time estimation software based on statistical modeling.

:warning: *This project is still in development.* :warning:

[![pipeline status](https://gitlab.com/signal9/angonoka/badges/master/pipeline.svg)](https://gitlab.com/signal9/angonoka/commits/master)
[![coverage report](https://gitlab.com/signal9/angonoka/badges/master/coverage.svg)](https://gitlab.com/signal9/angonoka/commits/master)
[![license](https://img.shields.io/badge/license-MIT-green)](/LICENSE)
[![lines of code](https://tokei.rs/b1/gitlab/signal9/angonoka?category=code)](https://github.com/XAMPPRocky/tokei)

```console
$ cat tasks.yaml
agents:
  Jennifer:
    groups:
      - Backend
  James The Intern:
    performance: 0.75
    groups:
      - Backend
  Linda:
    groups:
      - Frontend
tasks:
  - name: Develop backend
    group: Backend
    duration:
      min: 1 weeks
      max: 3 weeks
  - name: Develop frontend
    group: Frontend
    duration:
      min: 5 days
      max: 14 days

$ angonoka tasks.yaml
Parsing configuration... OK
Optimizing the schedule... OK
Optimal makespan: 14 days.
Probability estimation complete.
Done.
```

## Table of Contents

  * [Requirements](#requirements)
  * [Usage](#usage)
  * [Contributing](#contributing)
  * [License](#license)

## Requirements

[Pre-built binaries](https://gitlab.com/signal9/angonoka/-/pipelines?page=1&scope=branches&ref=master) are available for Linux (`x86_64-unknown-linux-gnu`/`glibc`). This packaged version of Angonoka comes with all of the dependencies included.

To build Angonoka from source you need:

* [conan]
* C++20 compliant compiler

For the up-to-date list of runtime and build-time dependencies see [conanfile.txt](/conanfile.txt).

## Usage

WIP

*Add release links, building instructions...*

## Contributing

WIP

## License

Angonoka is licensed under the [MIT](/LICENSE) license.  
Copyright &copy; 2019, Andrew

[conan]: https://conan.io/

<!--
https://gist.github.com/rowanmanning/77f31b2392dda1b58674#file-readme-md
-->
