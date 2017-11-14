# Computer-Architecture-Cache

The lab has two parts. I need to understand the organization and the behavior of a cache memory and its impact on C programs and data structures.
* Part A: Writing a Cache Simulator
In this part, it is required to write a cache simulator in C which simulates the hit/miss/evict behaviour of a cache memory on valgrind memory traces. The structure of the cache, including the set size, the associativity and the block size, is arbitrary and designated by the command-line arguments. And it should use LRU replacement policy. It should output the exact total number of hits, misses, and evictions.
* Part B: Optimizing Matrix Transpose
In this part, it is required to write a matrix transpose function with no more than 12 local variables in C as few cache misses as possible in directed mapped cache memory with 1KB cache size and 32-byte block size. The function should be correct and effective for three different-sized matrices: 32*32, 64*64 and 61*67.
