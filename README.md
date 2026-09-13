# Simple DNS Cache Experiment

This project is a small C++ experiment inspired by Cloudflare's article on
[reducing DNS cache memory usage][cloudflare-article].
It explores DNS record storage, caching, and concurrency without trying to reproduce
Cloudflare's data structures or caching algorithm exactly.

## Current state

The repository currently contains a working local client/server prototype:

- The server listens on the Unix domain socket `/tmp/dns-cache.sock`.
- Eight worker threads process requests through a bounded producer/consumer queue.
- Records are read from a SQLite database and stored in a shared, in-memory key/value cache.
- The database is initialized from `data/schema.sql` the first time the server runs if
  `dns.db` does not already exist.
- Queries use the text format `<owner> <type>`, such as `example.com A`.
- `A`, `AAAA`, `CNAME`, and `NAPTR` records are supported.
- The sample client starts ten threads and sends a fixed set of example queries.

This is an experimental cache rather than a DNS resolver: it only returns records present in
the local database. The cache is currently unbounded and has no TTL expiration or eviction.
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
