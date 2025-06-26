# Fuzzing a Simple Example of OpenBSD's imsg on FreeBSD

This project was tested on **FreeBSD 14.3**.

## Overview

- `simple_bgp.c`: Based on [OpenBGPD](https://www.openbgpd.org/), demonstrating how it uses the `imsg` API.
- `fuzz--simple_bgp.c`: A modified version of `simple_bgp.c` for experimenting with fuzzing the compartment interface of the parent process.

## Install AFL++ 

### Requirements

Install [AFL++](https://github.com/AFLplusplus/AFLplusplus/tree/stable) on FreeBSD:

```sh
pkg install security/afl++
```

Add AFL++ to your `PATH`:

```sh
echo "export PATH=$PATH:/usr/local/afl++-llvm/bin" >> ~/.profile
. ~/.profile
```

## Compilation

### Compile `simple_bgp.c` (for baseline testing):

```sh
cc -g3 -O0 -Wall -o simple_bgp simple_bgp.c imsg.c imsg-buffer.c compat.c
```

### Compile `fuzz--simple_bgp.c` with AddressSanitizer and AFL++:

```sh
export AFL_USE_ASAN=1
afl-clang-lto -g -O2 fuzz--simple_bgp.c imsg.c imsg-buffer.c compat.c -o fuzz--simple_bgp
```

## Running AFL++

### Fuzz with a valid imsg as the seed:

```sh
afl-fuzz -i seeds -o out -- ./fuzz--simple_bgp
```

### Fuzz with a minimal/random seed:

```sh
afl-fuzz -i seeds_empty -o out--seeds_empty -- ./fuzz--simple_bgp
```

## Install afl-cov

[afl-cov](https://github.com/AFLplusplus/afl-cov?tab=readme-ov-file) 
provides information on the code coverage achieved during fuzzing.  

### Requirements

Install:
- lcov, gcov, genhtml
- python

```sh
pkg install lcov
pkg install python
```

`afl-cov` expects the `lcov`, `genhtml` and `geninfo` binaries in `/usr/bin`.  
Create a symlink to those binaries in `/usr/bin`:

```sh
ln -s /usr/local/bin/lcov /usr/bin/lcov
ln -s /usr/local/bin/genhtml /usr/bin/genhtml
ln -s /usr/local/bin/geninfo /usr/bin/geninfo
```

Clone the
[afl-cov](https://github.com/AFLplusplus/afl-cov?tab=readme-ov-file) 
repository.

Install `afl-cov` by running the following command inside the `afl-cov` directory:

```sh
make install
```

## Running afl-cov

### Compiling with code coverage instrumentation

Ideally create a separate copy of the source code directory
to maintain the two separate compilations of the source code.
One using fuzzing instrumentation (as demonstrated earlier) 
and another using coverage instrumentation.

```sh
cc -g -O0 --coverage fuzz--simple_bgp.c imsg.c imsg-buffer.c compat.c -o fuzz--simple_bgp
```

### Generate coverage report from AFL++ test cases

```sh
afl-cov -d /path/to/afl-fuzz-output --coverage-cmd "./fuzz--simple_bgp < AFL_FILE" --code-dir .
```

Replace `/path/to/afl-fuzz-output` to the `out` or `out--seeds_empty` directory used 
during fuzzing.

The coverage information will be stored in `/path/to/afl-fuzz-output/cov`.

The generated `html` files are stored in `/path/to/afl-fuzz-output/cov/web` and
can be viewed with a web browser.

## Additional Notes

- `seeds/`: Contains at least one file with a valid `imsg`.
- `seeds_empty/`: Contains minimal/random input (e.g., a single `a` character).
- `(gdb) p imsgbuf->w.rbufs->bufs->tqh_first->buf` used to print the contents in an imsg buffer with GDB
