# HPC Performance Analysis - Memory

This repository will be used for experiments with memory performance.

Modern CPU architectures have several levels of cache, each with different semantics.

## Experiments

#### 0. Setup and clone

Create a GitHub account, clone this repository, and configure your Git environment

    git clone https://yourname@github.com/CUBoulder-HPCPerfAnalysis/memory.git
    git config --global user.name 'Your Name'
    git config --global user.email your.email@colorado.edu

Feel free to use SSH instead on HTTPS.
If you use bash, I recommend downloading and sourcing [git-prompt](https://raw.githubusercontent.com/git/git/master/contrib/completion/git-prompt.sh) and [git-completion.bash](https://raw.githubusercontent.com/git/git/master/contrib/completion/git-completion.bash).
Git resources:

* [Official Git Documentation](http://git-scm.com/documentation)
* [Git for Computer Scientists](http://eagain.net/articles/git-for-computer-scientists/)
* [Learn Git Branching (interactive)](https://pcottle.github.io/learnGitBranching/)
* https://try.github.io/
* [Interactive Cheat Sheet](http://ndpsoftware.com/git-cheatsheet.html)

#### 1. Compile and run the included [STREAM benchmark](http://www.cs.virginia.edu/stream/).

    make CC=gcc CFLAGS='-O3 -march=native' stream
    ./stream

#### 2. Add Dot to the results

Implement a "dot product" test (x ⋅ y = sum_i x_i y_i) and commit your changes.

#### 3. Create a CSV file at `results/yourname.csv` with the format

```
    # Username, Machinename, CPU name, CPU GHz, CPU Cores, CPU Cores used, Private cache/core (MB), Shared cache (MB), Array Length (MB), Peak MB/s, Copy MB/s, Scale MB/s, Add MB/s, Triad MB/s, Dot MB/s
    jed, batura, i7-4500U, 1.8, 2, 1, 0.256, 4.096, 76.3, 25600, 19555, 12784, 13483, 13677, ?
    ...
```

Report the "best rate".
On Linux systems, look in `/proc/cpuinfo`.
For theoretical peak bandwidth, try http://ark.intel.com.
Leave a `?` for missing data (but try not to have missing data).
Commit this new file:

    git add results/yourname.csv
    git commit

Use the commit message to explain your workflow creating the results file.
We're going to add more tests in the future, so automation would be good here (collaboration encouraged).

#### 4. Submit your changes as a pull request

Make a GitHub Fork [this repository](https://github.com/CUBoulder-HPCPerfAnalysis/memory/fork) on GitHub, push changes to your fork, and submit a pull request to the main repository.

#### 5. Open issues
There may be ambiguities in the specification.
If you spot any, [open an issue](https://github.com/CUBoulder-HPCPerfAnalysis/memory/issues).
Also open issues for ways to improve the workflow and provenance for running experiments and reporting data.

## References

* [STREAM Benchmark](http://www.cs.virginia.edu/stream/)
* [Ulrich Drepper, *What Every Programmer Should Know About Memory*, 2007](http://www.akkadia.org/drepper/cpumemory.pdf)
* [Gustavo Duartes, *Cache: A Place for Concealment and Safekeeping*, 2009](http://duartes.org/gustavo/blog/post/intel-cpu-caches/)
* [Gustavo Duartes, *Getting Physical With Memory*, 2009](http://duartes.org/gustavo/blog/post/getting-physical-with-memory/)
* [John McCalpin's blog](http://sites.utexas.edu/jdm4372/) (mostly about memory performance)

## Tools

* [hwloc](http://www.open-mpi.org/projects/hwloc/): Portable Hardware Locality
* [likwid](https://code.google.com/p/likwid/): x86 performance tools
