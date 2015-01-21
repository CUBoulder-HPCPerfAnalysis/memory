#!/bin/bash
# This script runs my modified STREAM benchmark for cache line
# sizes of 1, 2, ..., N, where N = 64 is the actual cache line
# size for my Intel Core 2 Duo T6500.

'Cache Line Size, Best Rate MB/s, Avg. Time, Min. Time, Max. Time ' > text.txt

for i in {1..1}; do
   ./stream $i | grep 'Using cache_line_size of\|Add_BlkCyc' | tee -a test.txt
done
