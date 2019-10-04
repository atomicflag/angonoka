## Angonoka

[![pipeline status](https://gitlab.com/signal9/angonoka/badges/master/pipeline.svg)](https://gitlab.com/signal9/angonoka/commits/master)
[![coverage report](https://gitlab.com/signal9/angonoka/badges/master/coverage.svg)](https://gitlab.com/signal9/angonoka/commits/master)

Find the makespan distribution of a set of tasks using the genetic algorithm.

### Usage

TODO

```bash
# TODO
```

### Development

Requirements:

* C++17
* Python 3
* Ninja

#### Building from source

```bash
# Install Meson & Conan
python3 -m pip install --user meson conan

# Add "meson" and "conan" to the search path
export PATH="$HOME/.local/bin:$PATH"

# Add Conan remotes
conan remote add signal9 https://api.bintray.com/conan/signal9/conan

mkdir build
cd build

# Install Conan dependencies
conan install -b missing ..

# Set pkg-config search path for Meson
export PKG_CONFIG_PATH=$(pwd)
# Set Boost root for Meson
export BOOST_ROOT=$(pkg-config --variable=prefix boost)

# Configure Meson
meson ..

# Build
ninja
```
