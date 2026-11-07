DROP TABLE IF EXISTS records;

CREATE TABLE records (
    id        INTEGER PRIMARY KEY,
    owner     TEXT NOT NULL,
    type      TEXT NOT NULL,
    ttl       REAL DEFAULT 3600 NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
    data      TEXT NOT NULL
);

INSERT INTO records (owner, type, data)
VALUES
    ('example.com', 'A', '192.0.2.10'),
    ('example.com', 'NS', 'ns1.example.com'),
    ('ns1.example.com', 'A', '192.0.2.53'),
    ('ns1.example.com', 'AAAA', '2001:db8::53');
