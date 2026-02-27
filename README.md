# Sweepmine.TM

A fast Minesweeper AI with success rate near frontier research models. The solver abstracts the Minesweeper grid into a constraint satisfaction problem (CSP), which is solved using logical deduction, Gaussian-elimination, and recursive backtracking. The solutions to the CSP are used to determine a garunteed safe move (if any exists) or a probabilistically optimal move otherwise. 

The solver also supports toroidal maps (wrap-around) and weighted mines. This project was originally intended as a problem for a contest setting (see `statement.pdf`). 

## Directory

| Folder | Contents |
|---------|-----------|
| `/build` | Executables |
| `/data` | Input/output files for solver |
| `/draft` | Previous iteration |
| `/run` | Scripts to benchmark and visualise |
| `/src` | Underlying C++ |

## Usage (Local)

- Clone repository locally
- Navigate to `/run` directory
- Compile the binary with solver/grader logic:
    - If only running normal games (no torus/weighted variations), execute `make MODE=normal`
    - If testing modded variant, execute `make MODE=modded`
- Now:
    - Run `python3 benchmarker.py` to benchmark the bot
    - Populate `/data/sweepmine.out` by running `../bin/sweepmine` (run a game via sample grader)
    - Visualise a game using `python3 visualiser.py` 

## Mechanics

Documentation of the grader (`sweepmine`), `benchmark.py`, and `visualiser.py` systems can be found in the original [problem statement](/statement.pdf) from page 3 onwards. The PDF demonstrates how each expects input, and what the purpose of each is.

Note: This statement was originally written to introduce the interface in a contest setting (Australian Informatics Olympiad Committee Student-Run Contest).

## Technical Details

### Map Generation

When `sweepmine` is not in debug mode (i.e. provided a grid), pseudo-adaptive automatic map generation kicks into play. The first cell revealed by the algorithm will always be a non-mine. However, it is **not** guaranteed that the map is solvable from there using pure logic.

Mines are placed via picking a set of random non-

### (1) Logical Reduction

This is only perfomed when mines are non-weighted.

TODO

### (2) Gaussian Elimination

This is only performed when mines are non-weighted.

TODO

### (3) Probabilistic Backtrack

TODO