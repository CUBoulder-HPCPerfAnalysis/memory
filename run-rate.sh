for n in {0..11}; do
    gcc -O3 -march=native -mno-avx -fopenmp stream.c -DSTREAM_BLOCK_SIZE=$((2**n)) -DSTREAM_NUM_BLOCKS=$(((65536*1024)/(2**n)))  -o stream 
    OMP_NUM_THREADS=4 ./stream \
      | grep '\(Copy\|Scale\|Add\|Triad\|Dot\)' \
      | awk '{print $2}' \
      | xargs echo fdkong `hostname -f` Intel Core i5  2.6 2 $((2**n))  0.256 3  76.3 25600 \
      | sed 's/ /,/g'
    done | tee >> results/fdkong-rate.csv
