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
headers in the same directory. 

### Script

Run `./scripts/sqlite_download` to automatically set up the sqlite3 dependencies.

### Manual download

Download the current SQLite amalgamation archive from the
[official SQLite download page](https://www.sqlite.org/download.html), then extract at least
these files into `external/sqlite/`:

```text
external/sqlite/sqlite3.c
external/sqlite/sqlite3.h
external/sqlite/sqlite3ext.h
```

## Build and run

Run all commands from the repository root.

Configure and build both the client and the server with CMake:

```sh
cmake -S . -B build
cmake --build build
```

Start the server:

```sh
./build/Server
```

In another terminal, start the client:

```sh
./build/Client
```

On its first run, the server creates `data/dns.db` and loads the sample records from
`data/schema.sql`. To use different records, edit the schema before the database is created,
or update the `records` table with a SQLite-compatible tool.

[cloudflare-article]: https://blog.cloudflare.com/dns-cache-memory-optimization-1111/
