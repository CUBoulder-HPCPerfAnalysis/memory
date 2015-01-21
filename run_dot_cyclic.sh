#!/bin/bash

# This script runs my modified STREAM benchmark for cache line
# sizes of 1, 2, ..., N, where N = 64 is the actual cache line
# size for my Intel Core 2 Duo T6500.

results_file="results/ryan_Add_BlkCyc.csv"

echo 'Stream Skip Length, Best Rate MB/s, Avg. Time, Min. Time, Max. Time' | tee $results_file

for i in {1..64}; do
   ./stream $i |
        grep 'Using cache_line_size of\|Add_BlkCyc' |
        awk 'BEGIN { RS="" } { print $4, $6, $7, $8, $9 }' |
        sed 's/\s/, /g' |
        tee -a $results_file
done
