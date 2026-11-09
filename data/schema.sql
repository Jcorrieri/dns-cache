CREATE TABLE records (
    id        INTEGER PRIMARY KEY,
    owner     TEXT NOT NULL,
    type      TEXT NOT NULL,
    ttl       REAL DEFAULT 3600 NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
    target    TEXT,
    data      BLOB NOT NULL
);

CREATE INDEX records_owner_idx ON records(owner);

BEGIN TRANSACTION;

-- A 192.0.2.10
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.com',
    'A',
    3600,
    NULL,
    X'C000020A'
);

-- A 192.0.2.11
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.com',
    'A',
    3600,
    NULL,
    X'C000020B'
);

-- AAAA 2001:db8::10
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.com',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000000000000000000000010'
);

-- AAAA 2001:db8:22:12a:10ef:40:22:10
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.com',
    'AAAA',
    3600,
    NULL,
    X'20010DB80022012A10EF004000220010'
);

-- NS target: ns1.example.com
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.com',
    'NS',
    3600,
    'ns1.example.com',
    X''
);

-- A 192.0.2.53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.example.com',
    'A',
    3600,
    NULL,
    X'C0000235'
);

-- AAAA 2001:db8::53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.example.com',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000000000000000000000053'
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.com',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '10 100 "s" "SIP+D2U" "" _sip._udp.example.com'
        AS BLOB
    )
);

-- CNAME target: example.com
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'www.example.com',
    'CNAME',
    3600,
    'example.com',
    X''
);

-- A 203.0.113.15
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.example.com',
    'A',
    3600,
    NULL,
    X'CB00710F'
);

-- AAAA 2001:db8::20
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.example.com',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000000000000000000000020'
);

-- A 192.0.2.25
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mail.example.com',
    'A',
    3600,
    NULL,
    X'C0000219'
);

-- AAAA 2001:db8::25
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mail.example.com',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000000000000000000000025'
);

-- CNAME target: app-host.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'app.example.com',
    'CNAME',
    3600,
    'app-host.example.org',
    X''
);

-- CNAME target: shops.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'shop.example.com',
    'CNAME',
    3600,
    'shops.example.org',
    X''
);

-- CNAME target: stats.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'status.example.com',
    'CNAME',
    3600,
    'stats.example.org',
    X''
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'voice.example.com',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '20 100 "s" "SIPS+D2T" "" _sips._tcp.example.com'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'enum.example.com',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '100 10 "u" "E2U+sip" "!^.*$!sip:info@example.com!" .'
        AS BLOB
    )
);

-- A 192.0.2.100
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    '_dmarc.example.com',
    'A',
    3600,
    NULL,
    X'C0000264'
);

-- A 198.51.100.10
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.net',
    'A',
    3600,
    NULL,
    X'C633640A'
);

-- A 198.51.100.11
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.net',
    'A',
    3600,
    NULL,
    X'C633640B'
);

-- AAAA 2001:db8:a::10
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.net',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000A00000000000000000010'
);

-- NS target: ns1.example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'example.net',
    'NS',
    3600,
    'ns1.example.net',
    X''
);

-- A 198.51.100.53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.example.net',
    'A',
    3600,
    NULL,
    X'C6336435'
);

-- AAAA 2001:db8:a::53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.example.net',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000A00000000000000000053'
);

-- CNAME target: example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'www.example.net',
    'CNAME',
    3600,
    'example.net',
    X''
);

-- CNAME target: example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'blog.example.net',
    'CNAME',
    3600,
    'example.net',
    X''
);

-- A 198.51.100.25
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mail.example.net',
    'A',
    3600,
    NULL,
    X'C6336419'
);

-- AAAA 2001:db8:a::25
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mail.example.net',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000A00000000000000000025'
);

-- A 203.0.113.40
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.example.net',
    'A',
    3600,
    NULL,
    X'CB007128'
);

-- AAAA 2001:db8:b::40
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.example.net',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000B00000000000000000040'
);

-- CNAME target: hosting.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'docs.example.net',
    'CNAME',
    3600,
    'hosting.example.org',
    X''
);

-- CNAME target: stores.example.com
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'shop.example.net',
    'CNAME',
    3600,
    'stores.example.com',
    X''
);

-- CNAME target: edge.cdn.example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'cdn.example.net',
    'CNAME',
    3600,
    'edge.cdn.example.net',
    X''
);

-- CNAME target: pages.status.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'status.example.net',
    'CNAME',
    3600,
    'pages.status.example.org',
    X''
);

-- A 203.0.113.60
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'sip.example.net',
    'A',
    3600,
    NULL,
    X'CB00713C'
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'voice.example.net',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '10 100 "s" "SIP+D2U" "" _sip._udp.example.net'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'voice.example.net',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '20 100 "s" "SIPS+D2T" "" _sips._tcp.example.net'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'enum.example.net',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '100 10 "u" "E2U+sip" "!^.*$!sip:info@example.net!" .'
        AS BLOB
    )
);

-- A 192.0.2.5
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'tiny.example.org',
    'A',
    3600,
    NULL,
    X'C0000205'
);

-- AAAA 2001:db8:f::1
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'v6only.example.org',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000F00000000000000000001'
);

-- CNAME target: v6only.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'v6only-alias.example.org',
    'CNAME',
    3600,
    'v6only.example.org',
    X''
);

-- A 203.0.113.1
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'simplesite.example',
    'A',
    3600,
    NULL,
    X'CB007101'
);

-- NS target: ns1.simplesite.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'simplesite.example',
    'NS',
    3600,
    'ns1.simplesite.example',
    X''
);

-- A 203.0.113.53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.simplesite.example',
    'A',
    3600,
    NULL,
    X'CB007135'
);

-- AAAA 2001:db8:3::53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.simplesite.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000300000000000000000053'
);

-- CNAME target: simplesite.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'blog.simplesite.example',
    'CNAME',
    3600,
    'simplesite.example',
    X''
);

-- AAAA 2001:db8:3f29:25::
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'jimmy.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB83F2900250000000000000000'
);

-- AAAA 2001:db8::3f29:25:232a
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'jimmy2.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB80000000000003F290025232A'
);

-- A 192.0.2.21
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'acme.example',
    'A',
    3600,
    NULL,
    X'C0000215'
);

-- AAAA 2001:db8:1::21
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'acme.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000100000000000000000021'
);

-- NS target: ns1.acme.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'acme.example',
    'NS',
    3600,
    'ns1.acme.example',
    X''
);

-- A 192.0.2.121
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.acme.example',
    'A',
    3600,
    NULL,
    X'C0000279'
);

-- AAAA 2001:db8:1::121
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.acme.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000100000000000000000121'
);

-- CNAME target: acme.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'www.acme.example',
    'CNAME',
    3600,
    'acme.example',
    X''
);

-- CNAME target: acme.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'shop.acme.example',
    'CNAME',
    3600,
    'acme.example',
    X''
);

-- A 192.0.2.22
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mail.acme.example',
    'A',
    3600,
    NULL,
    X'C0000216'
);

-- AAAA 2001:db8:1::22
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mail.acme.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000100000000000000000022'
);

-- A 198.51.100.31
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.acme.example',
    'A',
    3600,
    NULL,
    X'C633641F'
);

-- A 198.51.100.32
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.acme.example',
    'A',
    3600,
    NULL,
    X'C6336420'
);

-- AAAA 2001:db8:1::31
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'api.acme.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000100000000000000000031'
);

-- CNAME target: docs-host.example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'docs.acme.example',
    'CNAME',
    3600,
    'docs-host.example.net',
    X''
);

-- CNAME target: helpdesk.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'support.acme.example',
    'CNAME',
    3600,
    'helpdesk.example.org',
    X''
);

-- CNAME target: edge.cdn.example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'assets.acme.example',
    'CNAME',
    3600,
    'edge.cdn.example.net',
    X''
);

-- A 192.0.2.60
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'sip.acme.example',
    'A',
    3600,
    NULL,
    X'C000023C'
);

-- AAAA 2001:db8:1::60
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'sip.acme.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000100000000000000000060'
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'voice.acme.example',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '10 100 "s" "SIP+D2U" "" _sip._udp.acme.example'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'voice.acme.example',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '20 100 "s" "SIP+D2T" "" _sip._tcp.acme.example'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'voice.acme.example',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '30 100 "s" "SIPS+D2T" "" _sips._tcp.acme.example'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'enum.acme.example',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '100 10 "u" "E2U+sip" "!^.*$!sip:sales@acme.example!" .'
        AS BLOB
    )
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'enum.acme.example',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '102 10 "u" "E2U+email:mailto" "!^.*$!mailto:info@acme.example!" .'
        AS BLOB
    )
);

-- A 192.0.2.70
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'globex.example',
    'A',
    3600,
    NULL,
    X'C0000246'
);

-- AAAA 2001:db8:4::70
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'globex.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000400000000000000000070'
);

-- NS target: ns1.globex.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'globex.example',
    'NS',
    3600,
    'ns1.globex.example',
    X''
);

-- A 192.0.2.73
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.globex.example',
    'A',
    3600,
    NULL,
    X'C0000249'
);

-- AAAA 2001:db8:4::73
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.globex.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000400000000000000000073'
);

-- CNAME target: globex.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'blog.globex.example',
    'CNAME',
    3600,
    'globex.example',
    X''
);

-- CNAME target: apps.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'portal.globex.example',
    'CNAME',
    3600,
    'apps.example.org',
    X''
);

-- CNAME target: status-page.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'status.globex.example',
    'CNAME',
    3600,
    'status-page.example.org',
    X''
);

-- A 192.0.2.71
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mx1.globex.example',
    'A',
    3600,
    NULL,
    X'C0000247'
);

-- A 192.0.2.72
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mx2.globex.example',
    'A',
    3600,
    NULL,
    X'C0000248'
);

-- AAAA 2001:db8:4::72
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'mx2.globex.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000400000000000000000072'
);

-- CNAME target: files.example.net
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ftp.globex.example',
    'CNAME',
    3600,
    'files.example.net',
    X''
);

-- A 198.51.100.50
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'initech.example',
    'A',
    3600,
    NULL,
    X'C6336432'
);

-- NS target: ns1.initech.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'initech.example',
    'NS',
    3600,
    'ns1.initech.example',
    X''
);

-- A 198.51.100.153
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.initech.example',
    'A',
    3600,
    NULL,
    X'C6336499'
);

-- AAAA 2001:db8:6::53
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'ns1.initech.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000600000000000000000053'
);

-- CNAME target: initech.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'www.initech.example',
    'CNAME',
    3600,
    'initech.example',
    X''
);

-- A 192.0.2.88
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'intranet.initech.example',
    'A',
    3600,
    NULL,
    X'C0000258'
);

-- AAAA 2001:db8:6::1
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'v6.initech.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000600000000000000000001'
);

-- CNAME target: v6.initech.example
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'v6-alias.initech.example',
    'CNAME',
    3600,
    'v6.initech.example',
    X''
);

-- A 198.51.100.55
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'vpn.initech.example',
    'A',
    3600,
    NULL,
    X'C6336437'
);

-- AAAA 2001:db8:6::55
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'vpn.initech.example',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000600000000000000000055'
);

-- CNAME target: code-host.example.org
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'git.initech.example',
    'CNAME',
    3600,
    'code-host.example.org',
    X''
);

-- NAPTR payload is stored as opaque BLOB
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'tel.initech.example',
    'NAPTR',
    3600,
    NULL,
    CAST(
        '50 50 "u" "E2U+sip" "!^.*$!sip:ops@initech.example!" .'
        AS BLOB
    )
);

-- A 192.0.2.99
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'sip.initech.example',
    'A',
    3600,
    NULL,
    X'C0000263'
);

-- A 192.0.2.80
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'hosting.example.org',
    'A',
    3600,
    NULL,
    X'C0000250'
);

-- AAAA 2001:db8:c::80
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'hosting.example.org',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000C00000000000000000080'
);

-- A 203.0.113.77
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'stores.example.com',
    'A',
    3600,
    NULL,
    X'CB00714D'
);

-- A 198.51.100.200
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'edge.cdn.example.net',
    'A',
    3600,
    NULL,
    X'C63364C8'
);

-- A 198.51.100.201
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'edge.cdn.example.net',
    'A',
    3600,
    NULL,
    X'C63364C9'
);

-- AAAA 2001:db8:d::200
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'edge.cdn.example.net',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000D00000000000000000200'
);

-- A 192.0.2.150
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'pages.status.example.org',
    'A',
    3600,
    NULL,
    X'C0000296'
);

-- A 203.0.113.11
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'docs-host.example.net',
    'A',
    3600,
    NULL,
    X'CB00710B'
);

-- AAAA 2001:db8:2::11
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'docs-host.example.net',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000200000000000000000011'
);

-- A 203.0.113.12
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'helpdesk.example.org',
    'A',
    3600,
    NULL,
    X'CB00710C'
);

-- A 203.0.113.90
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'apps.example.org',
    'A',
    3600,
    NULL,
    X'CB00715A'
);

-- AAAA 2001:db8:5::90
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'apps.example.org',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000500000000000000000090'
);

-- A 198.51.100.90
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'status-page.example.org',
    'A',
    3600,
    NULL,
    X'C633645A'
);

-- A 203.0.113.95
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'files.example.net',
    'A',
    3600,
    NULL,
    X'CB00715F'
);

-- A 203.0.113.120
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'code-host.example.org',
    'A',
    3600,
    NULL,
    X'CB007178'
);

-- AAAA 2001:db8:7::120
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'code-host.example.org',
    'AAAA',
    3600,
    NULL,
    X'20010DB8000700000000000000000120'
);

-- A 198.51.100.77
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'app-host.example.org',
    'A',
    3600,
    NULL,
    X'C633644D'
);

-- A 203.0.113.38
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'shops.example.org',
    'A',
    3600,
    NULL,
    X'CB007126'
);

-- A 198.51.100.108
INSERT INTO records (owner, type, ttl, target, data)
VALUES (
    'stats.example.org',
    'A',
    3600,
    NULL,
    X'C633646C'
);

COMMIT;
