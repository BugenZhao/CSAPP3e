A simple **Implicit Free Lists** implementation with next-fit strategy and reallocation optimization.

TODO: **Segregated Fits** implementation.

## Results
```
Team Name:bzteam
Member 1 :Bugen Zhao:bugenzhao
Using default tracefiles in tracefiles/
Measuring performance with gettimeofday().

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   91%    5694  0.002018  2821
 1       yes   91%    5848  0.001358  4307
 2       yes   95%    6648  0.003033  2192
 3       yes   97%    5380  0.002975  1808
 4       yes   66%   14400  0.000078185328
 5       yes   90%    4800  0.003139  1529
 6       yes   88%    4800  0.002923  1642
 7       yes   55%   12000  0.013951   860
 8       yes   51%   24000  0.006161  3896
 9       yes   98%   14401  0.000079181602
10       yes   87%   14401  0.000085169623
Total          83%  112372  0.035800  3139

Perf index = 50 (util) + 40 (thru) = 90/100
```

