### Ungraded Solvers

Both solvers (normal and weighted torus) have ungraded versions, with which you can determine the best move at any board position. 

First input a position into `data/input.txt`. 

For `normal_ungraded.exe`, this should be of the form:

{number of rows} {number of columns} {number of bombs}
{state of each grid cell}

For `weighted_torus_ungraded.exe`:

{number of rows} {number of columns} {number of bombs} {is it a torus?}
{state of each grid cell}
{weight of each grid cell}

Then run:

```bash
./normal_ungraded.exe
./weighted_torus_ungraded.exe
```

And read output from `data/output.txt`.