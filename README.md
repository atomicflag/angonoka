## Angonoka

[![pipeline status](http://gitlab.cod.sz.rt.ru/dronx/angonoka/badges/master/pipeline.svg)](http://gitlab.cod.sz.rt.ru/dronx/angonoka/commits/master)
[![coverage report](http://gitlab.cod.sz.rt.ru/dronx/angonoka/badges/master/coverage.svg)](http://gitlab.cod.sz.rt.ru/dronx/angonoka/commits/master)

Minimum makespan task scheduling solver using the genetic algorithm.

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
