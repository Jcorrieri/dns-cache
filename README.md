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
- SQLite libraries

## SQLite setup

This project links against the SQLite3 shared library, see download instructions below:

Ubuntu/Debian
```
sudo apt install libsqlite3-dev
```

Fedora/RHEL
```
sudo dnf install sqlite-devel
```

Arch Linux
```
sudo pacman -S sqlite
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
./build/server
```

In another terminal, start the client:

```sh
./build/client
```

On its first run, the server creates `data/dns.db` and loads the sample records from
`data/schema.sql`. To use different records, edit the schema before the database is created,
or update the `records` table with a SQLite-compatible tool.

[cloudflare-article]: https://blog.cloudflare.com/dns-cache-memory-optimization-1111/

### Run the test suite

```sh
ctest --test-dir build
```

If desired, run tests before each commit:

```sh
git config --local core.hooksPath .githooks
```

## Config

Change parameters within `config.toml`. Note that the program does not include a full-spec toml parser and expects
the existing format (no comments or additional entries).