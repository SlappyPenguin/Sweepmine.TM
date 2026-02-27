### Ungraded Solvers

Both solvers (normal and weighted torus) have ungraded versions, with which you can determine the best move at any board position. 

First input a position into `data/input.txt`. 

For `normal_ungraded.exe`, this should be of the form:

$R$ $C$ $N$

$grid[i][j] for all (i, j)$

For `weighted_torus_ungraded.exe`:

$R$ $C$ $N$ $T$

$grid[i][j] for all (i, j)$

$charge[i][j] for all (i, j)$

Then run:

```bash
./normal_ungraded.exe
./weighted_torus_ungraded.exe
```

And read output from `data/output.txt`.