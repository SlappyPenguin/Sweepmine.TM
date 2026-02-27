# Sweepmine.TM

A fast Minesweeper algorithm with success rate on par with frontier research models. The solver abstracts the Minesweeper grid into a constraint satisfaction problem (CSP), while solving forced risk positions with probabilistic strategies. The solver also supports toroidal maps (wrap-around) and weighted mines. 

This project was originally intended as a problem for a contest setting (see [`statement.pdf`](statement.pdf)). 

## Directory

| Folder | Contents |
|---------|-----------|
| `/build` | Executables |
| `/data` | Input/output files for solver |
| `/draft` | Previous iteration |
| `/images` | Visuals |
| `/include` | Header files for C++ |
| `/run` | Scripts to benchmark and visualise |
| `/src` | Underlying C++ |

## Installation

### Requirements

- g++ with C++20
- make
- Python 3.6+

### Instructions

First, clone the repository:

``` bash
git clone https://github.com/SlappyPenguin/Sweepmine.TM.git
cd Sweepmine.TM
```

Compile all programs using the top-level Makefile:

``` bash
cd run
make
```

To run a benchmark on the solver:

``` bash
cd run
python simple_benchmarker.py
# Example
Enter level (easy/medium/hard): hard
Enter number of games to play: 1000
```

A benchmarker with more custom parameters (`run/benchmarker.py`) is also available.

``` bash
cd run
python visualiser.py
```

<p align="center">
  <img src="images/visualiser.png" width="500"><br>
</p>

Note that individual game spots can also be solved using the ungraded programs (see below).

## Usage Notes
For `visualiser.py` UI and usage information, see the final page of attached [`statement.pdf`](statement.pdf).

The graded programs `normal_graded` and `weighted_torus_graded` accept input via `stdin` and write to `data/sweepmine.out`. The input to these programs (executed alone by running `../build/normal_graded` etc.) matches the grader expected input in the original problem statement.

The non-graded programs `normal_ungraded` and `weighted_torus_ungraded` accept input the same way that `next_move()` in problem statement accepts input. Input is from `data/input.txt` and output is written to `data/output.txt`.

For `normal_ungraded`, this should be of the form:
- Line 1: $R$ $C$ $N$
- Lines 2 to $R+1$: A grid with the same format as `vector<vector<int>> grid`
  
For `weighted_torus_ungraded`, this should be of the form:
- Line 1: $R$ $C$ $N$ $T$
- Lines 2 to $R+1$: A grid with the same format as `vector<vector<int>> grid`
- Lines $R+2$ to $2R+1$: A grid of charges with the same format as `vector<vector<int>> charge`

## Technical Details
The solver always tries to find a certain move (revealing a guaranteed safe cell or flagging a guaranteed mine), before resorting to probabilistic guesswork.

### (1) Simple Logic
We try to apply the 2 simplest logical rules:

- If any square's # surrounding mines = # surrounding flags, all other surrounding squares are safe
- If any square's # surrounding mines = # surrounding uncovered squares, they are all mines

### (2) Gaussian Elimination
We try more complex logic to find certain moves. The bordering cells are abstracted into a CSP, which can be represented as a system of linear equations.

Specifically, each unknown cell forms a variable $x_{ij} \in \{0, 1\}$, and each known tile combines its adjacent unknown cells into an equation. Say a known cell showing value $v_{k}$ has neighbours $\{x_1,x_2,\dots,x_n\}$. Then, $v_k = \sum_{i=1}^nc_ix_i$.

By building a system of these linear equations, we have a general way to deterministically solve all logic-based game situations. Solving implements row-reduction via Gaussian elimination, and determining fixed-value variables via back-substitution.

### (3) Backtracking + Guessing
If (1) and (2) do not yield certain moves, we are forced to make probabilistic decisions. First, all possible solutions to the CSP (taking valid variables as all unrevealed cells bordering revealed ones) are generated using recursive backtracking.

This is heavily optimised using a combination of early-exit conditions and heuristic state prioritisations. Furthermore, we reduce state-space by splitting variables into independent sets. These optimisations make this theoretically exponential-time algorithm run extremely fast in practice.

Basic combinatorics allows us to assign an 'entropy' (moreso the microscopic configuration number $\Omega$) to each cell state. The probability of a cell being in said state then directly correlates to this entropic value. Ties are broken with [heuristic preference for corners and edges](https://cdn.aaai.org/ocs/ws/ws0294/15091-68459-1-PB.pdf).

## Performance
The success rate of the solver (probability of finishing a game without clicking a mine) is on par with frontier research models for all 3 standard board sizes:

| Strategy                        | 8-8-T10       | 16-16-T40     | 30-16-T99     |
|---------------------------------|---------------|---------------|---------------|
| *Sweepmine (this project)*    | *81.62%*        | *77.48%*        | *38.20%*     | 
| PSEQ-D256 (Tu et al. 2017)      | 81.79%        | 78.2%         | 40.06%        |
| OH (Buffet et al. 2013)        | 80.2%        | 74.4%        | 38.7%        |
| cSimEnuLoClf (Legendre et al. 2012) | 80%    | 75.6%        | 37.5%        |
| CSP (Studholme 2000)           | 80.0%        | 44.3%        | 33.9%        |
| CSCSP (Becerra 2015)           |               | 75.94%       | 32.90%       |
| LSWPE (Pedersen 2004)          |               | 67.7%        | 25.0%        |
| LBP-MC (Kamenetsky and Teo 2007) | 78.6%      | 44.8%        |               |
| PGMS (Ramsdell 1995)           | 71%          | 36 %          | 26%          |

Our benchmark was measured by running `run/simple_benchmarker.py` for $10^4$ games on each board size. The rest of the table was supplied by [Tu et al. (2017)](https://cdn.aaai.org/ocs/ws/ws0294/15091-68459-1-PB.pdf).

Our solver averages 6.03 seconds per 100 games on the Expert board size (16-30-T99). For comparison, the human world record for a singular Expert game is [26.59 seconds](https://minesweepergame.com/world-records.php).
