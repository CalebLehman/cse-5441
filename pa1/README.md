# Serial AMR (Lab 1)

Code to build a program for Adaptive Mesh Refinement (AMR) computation (more
specifically, stencil code).

# Files

```
|
+-Makefile - makefile for building the amr program and report from source
|
+-include/ - directory with headers
|  |
|  +-amr.h - header declaring some structs and functions used for running AMR
|  |
|  +-common.h - header declaring some structs and functions used to parse and output results
|
+-src/
|  |
|  +-amr.c - source for main AMR code
|  |
|  +-common.c - source for parsing and outputing results
|  |
|  +-report.tex - source for final report
|
+-tests/ - directory with input to testing scripts
|  |
|  +-README - description of the test files
|  |
|  +-testgrid_* - test files
|
+-tests.cfg - some configuration for the parameter_sweep.sh script
|
+-parameter_sweep.sh - shell script to run the tests
|
+-process_parameter_sweep.py - python script to process results of tests and generate plots
|
+-results/ - directory with results output from process_parameter_sweep.py script
|  |
|  +-testgrid_*_results.png - plot
|  |
|  +-testgrid_*_results.txt - data
|
+-report.pdf - report for submission, generated from TeX source
```

# Building

The program is built with `make amr`.

After tests have been run and processed, the report is generated with `make report`.

# Running

The syntax to run the program is `./amr [affect-rate] [epsilon] <[test-file]`.
