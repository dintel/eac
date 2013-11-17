# Entropy Adaptive Coding #

## Overview ##
This is an experimental implementation of Entropy Adaptive Coding algrorithm on
top of simplest Lempel Ziv 77 algorithm. This project is intended for testing
EAC and comparing it against simple Lempel Ziv 77 compression.

## Requirements ##
To compile and run this project following are required:

1. gcc version >= 4.8
2. Embedded GNU C Library >= 2.17
3. phtread support
4. doxygen >= 1.8.5 (for documentation generation only)

## Compilation ##
To compile this project first clone it using git. After that enter into
directory tests and run there `./update_tests.sh` to download latest test files
from S3.
After that run `make` in project directory to compile all binaries.

## Usage ##
There are 2 binaries that are provided are compiled:

1. `eac_encode` - this is encoder that can compress files using both LZ77 and
   EAC algorithms.
2. `eac_decode` - this is decoder that can decode files encoded with eac_encode.
Additionally there are a few scripts in tests directory:
1. `sanity.sh` - sanity tests for both LZ77 and EAC encoding and
   decoding. Validates that files are properly encoded and decoded.
2. `perf_single.sh` - this script runs test on single file by running eac_encode
   and eac_decode cycle on it with specified parameters. Output is written into
   file named after parameters in `results/` directory.
3. `performance.sh` - script that runs `perf_single.sh` on all test files with
   all possible configuration parameters. After running all CSV report is
   generated.

## Documentation ##
Internal documentation can be compiled using command `make doc`. For generating
documentation doxygen version `1.8.5` or later is needed.
After running `make doc` documentation will be accessible in

* `doc/html` - in HTML format
* `doc/latex` - in Latex format (can be compiled into PDF by running `make pdf`
  in that directory)
