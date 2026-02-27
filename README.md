# Sweepmine.TM

A fast Minesweeper AI with success rate on par with frontier research models. The solver abstracts the Minesweeper grid into a constraint satisfaction problem (CSP), while solving forced risk positions with probabilistic strategies. The solver also supports toroidal maps (wrap-around) and weighted mines. 

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
Requirements for usage:

- g++ with C++20
- make
- Python 3.6+

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

A benchmarker with more custom parameters (`run/benchmarker.py`) is also available. After benchmarking, visualise the final game:

``` bash
cd run
python visualiser.py
```

<p align="center">
  <img src="images/visualiser.png" width="500"><br>
</p>

Note that individual game spots can also be solved using the ungraded programs (see below).

## Usage Notes
For `visualiser.py` usage information, see the final page of attached [`statement.pdf`](statement.pdf).

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
The solver always tries to find a certain move (clicking a guaranteed safe square, or flagging a certain mine), before resorting to probabilistic risk-taking.

### (1) Simple Logic
We try to apply the 2 simplest logical rules:

- If any square has # surrounding mines = # surrounding flags, all other surrounding squares are safe
- If any square has # surrounding mines = # surrounding uncovered squares, they all have mines

### (2) Gaussian Elimination
We try more complicated logic to find certain moves. The grid is abstracted into a CSP, which can be represented as a system of linear equations. To find if any variable must have a fixed value, we run Gaussian Elimination.

### (3) Backtracking + Guessing
If (1) and (2) do not yield certain moves, we are forced to take risk. First, all possible solutions to the CSP are generated using (heavily optimised) recursive backtracking. 

Using combinatorics allows us to calculate the number of ways each solution could occur (# bomb arrangements on rest of board). Thus, we can assign probabilities for each square being a bomb. The lowest probability square is chosen with [heuristic preference for corners and edges](https://cdn.aaai.org/ocs/ws/ws0294/15091-68459-1-PB.pdf).

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

Our benchmark was measured by running `run/simple_benchmarker.py` for $10^4$ games on each board size. The rest of the table was supplied by [this paper](https://cdn.aaai.org/ocs/ws/ws0294/15091-68459-1-PB.pdf).

Our solver averages 6.03 seconds per 100 games on the Expert board size (16-30-T99). For comparison, the human world record for a singular Expert game is [26.59 seconds](https://minesweepergame.com/world-records.php).
