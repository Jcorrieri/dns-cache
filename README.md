# DNS Cache
A multithreaded C++20 DNS caching service using Unix domain socket IPC, SQLite-backed storage, and a shared in-memory cache.

I was originally inspired by Cloudflare's article on
[reducing DNS cache memory usage][cloudflare-article] and its memory optimizations, 
but I eventually fell down a rabbit hole of learning C++ concepts and about unix IPC.

## Current state

The repository contains a working local client/server MVP:

- The server listens on the Unix domain socket /tmp/dns-cache.sock.
- A fixed pool of worker threads processes accepted connections through a bounded producer/consumer queue.
- Cache misses are resolved against a SQLite-backed repository and inserted into a shared, thread-safe in-memory key/value cache.
- Worker threads reuse their repository, SQLite connection, and prepared statement state across requests.
- The database is initialized from data/schema.sql the first time the server runs if dns.db does not already exist.
- Queries use the text format <owner> <type>, such as example.com A.
- A, AAAA, CNAME, and NAPTR records are supported.
- The included sample client creates multiple threads and issues concurrent requests to the server.

## Limitations

This is an experimental cache rather than a complete DNS resolver.

The current implementation:

- Only returns records available in the local SQLite database.
- Does not recursively resolve CNAME chains.
- Does not perform external DNS queries.
- Does not implement TTL expiration or cache eviction.
- Uses an intentionally unbounded in-memory cache.
- Uses a simple text-based request/response protocol intended for local experimentation rather than production use.

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

The required amalgamation files are already present in this repository, so this step is only
needed when setting them up again or updating SQLite. For background, see SQLite's
[amalgamation documentation](https://www.sqlite.org/amalgamation.html) and
[compilation guide](https://www.sqlite.org/howtocompile.html).

No separate system-wide SQLite installation or linker flag is needed for the server build.

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
