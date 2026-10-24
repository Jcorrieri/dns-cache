# DNS Cache
A multithreaded C++20 DNS caching service using Unix domain socket IPC, SQLite-backed storage, and a shared in-memory cache.

I was originally inspired by Cloudflare's article on
[reducing DNS cache memory usage][cloudflare-article] and its memory optimizations, 
but I eventually fell down a rabbit hole of learning C++ concepts and about unix IPC.

Planned ideas and remaining work are tracked in the
[Project Scope issue](https://github.com/Jcorrieri/dns-cache/issues/1).

## Requirements

- Linux or another environment that supports Unix domain sockets
- A C++20 compiler
- CMake 3.16 or newer
- SQLite's C amalgamation source (see below)

## SQLite setup

The CMake target compiles SQLite directly from `external/sqlite/sqlite3.c` and expects its
headers in the same directory. Download the current SQLite amalgamation archive from the
[official SQLite download page](https://www.sqlite.org/download.html), then extract at least
these files into `external/sqlite/`:

```text
external/sqlite/sqlite3.c
external/sqlite/sqlite3.h
external/sqlite/sqlite3ext.h
```

## Build and run

Run all commands from the repository root. The server uses repo-relative paths for
`data/schema.sql` and `dns.db`.

Configure and build the server with CMake:

```sh
cmake -S . -B build
cmake --build build
```

Start the server:

```sh
./build/app
```

In another terminal, compile the sample client with `g++` or an equivalent C++ compiler:

```sh
g++ -std=c++20 -pthread src/client.cpp -Iincludes -o client
```

Then run it while the server is listening:

```sh
./client
```

On its first run, the server creates `dns.db` and loads the sample records from
`data/schema.sql`. To use different records, edit the schema before the database is created,
or update the `records` table with a SQLite-compatible tool.

[cloudflare-article]: https://blog.cloudflare.com/dns-cache-memory-optimization-1111/
