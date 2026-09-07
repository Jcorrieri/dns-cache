# Simple DNS Cache Experiment

Taking inspiration from [CloudFlare's Blog Post](https://blog.cloudflare.com/dns-cache-memory-optimization-1111/#what-we-cache) on 
their optimizations to save 100TB of cache memory across DNS platform (Big Pineapple), I've decided to implement a small program and try 
and emulate the performance gains. Their example uses Rust, but here I use C++ since I'm learning it for GPU programming.

This does not aim to replicate their data structures and caching algorithm exactly, but I hope to see similar benefits and
learn about caching and performance optimizations along the way.

