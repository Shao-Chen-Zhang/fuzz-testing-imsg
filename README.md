# Simple BGP Fuzzer

This project was tested on **FreeBSD 14.3**.

## Overview

- `simple_bgp.c`: Based on [OpenBGPD](https://www.openbgpd.org/), demonstrating how it uses the `imsg` API.
- `fuzz--simple_bgp.c`: A modified version of `simple_bgp.c` for experimenting with fuzzing the compartment interface of the parent process.

## Requirements

Install **AFL++** on FreeBSD:

```sh
pkg install security/afl++ vim
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
afl-clang-lto -DNO_MAIN -g -O2 fuzz--simple_bgp.c imsg.c imsg-buffer.c compat.c -o fuzz--simple_bgp
```

## Running AFL++

### Fuzz with initial valid seed:

```sh
afl-fuzz -i seeds -o out -- ./fuzz--simple_bgp
```

### Fuzz with minimal/random seed:

```sh
afl-fuzz -i seeds_empty -o out--seeds_empty -- ./fuzz--simple_bgp
```

## Notes

- `seeds/`: Contains at least one file with a valid `imsg`.
- `seeds_empty/`: Contains minimal/random input (e.g., a single `a` character).
- `(gdb) p imsgbuf->w.rbufs->bufs->tqh_first->buf` used to print the contents in an imsg buffer with GDB
