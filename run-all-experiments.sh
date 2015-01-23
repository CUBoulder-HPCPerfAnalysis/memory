#/bin/bash

echo "#SKIP Value, Thread Number, Best Rate MB/s, Avg. Time, Min. Time, Max. Time"

OMP_NUM_THREADS=4

for i in 1 2 4 8 16 32 64 128 254 512 ; do
  # Delete old executable
  rm stream > /dev/null 2>&1

  # Compile with a new value of SKIP, no output
  make CC=gcc CFLAGS="-O3 -march=native -fopenmp -DTUNED -DSKIP=$i" stream > /dev/null 2>&1

  for tn in 1 4 8 16 32 ; do
    # Run and get the Dot line
    line=`OMP_NUM_THREADS=$tn ./stream | grep Dot`

    # Clean line: no "Dot_BLOCK_CYCLIC:", no spaces at the beginning, no multiple spaces, commas for separators
    cline=`echo $line | sed "s/Dot_BLOCK_CYCLIC://" | sed "s/^\ *//" | sed "s/\ \ */\ /g" | sed "s/\ /, /g"`

    # Output
    echo "$i, $tn, $cline"

  done
done
