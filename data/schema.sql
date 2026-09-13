CREATE TABLE records (
    id        INTEGER  PRIMARY KEY,
    owner     TEXT     NOT NULL,
    type      TEXT     NOT NULL,
    ttl       REAL     DEFAULT 3600 NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
    data      TEXT     NOT NULL
);

INSERT INTO records (owner, type, data)
VALUES 
("example.com", "A", "192.0.2.10"),
("example.com", "AAAA", "2001:db8::10"),
("example.com", "A", "192.0.2.11"),
("example.com", "CNAME", "svcb-replacement.example.com"),
("example.com", "NAPTR", '10 100 "s" "SIP+D2U" "" _sip._udp.example.com'),
("www.example.com", "CNAME", "example.com"),
("simplesite.com", "A", "164.38.55.1"),
("blog.simplesite.com", "CNAME", "simplesite.com"),
("api.example.com", "A", "203.0.113.15"),
("api.example.com", "AAAA", "2001:db8::20"),
("api.example.com", "CNAME", "pool.example.com"),
("example.herokuapp.com", "A", "52.204.31.10"),
("app.example.com", "CNAME", "example.herokuapp.com"),
("voice.example.com", "NAPTR", '20 100 "s" "SIPS+D2T" "" _sips._tcp.example.com'),
("_dmarc.example.com", "A", "192.0.2.100"),
("mail.example.com", "A", "192.0.2.25"),
("mail.example.com", "AAAA", "2001:db8::25"),
("jimmy.com", "AAAA", "2001:db8:3f29:25::"),
("jimmy2.com", "AAAA", "::2001:db8:3f29:25:232a"),
("_domainkey.example.com", "CNAME", "dkim.mosaico.example.com"),
("enum.example.com", "NAPTR", '100 10 "u" "E2U+sip" "!^.*$!sip:info@example.com!" .'),
("cdn.example.com", "CNAME", "svcb.cdn-provider.net."),
("shops.myshopify.com", "A", "23.227.38.65"),
("shop.example.com", "CNAME", "shops.myshopify.com"),
("stats.betteruptime.com", "A", "116.203.116.108"),
("status.example.com", "CNAME", "stats.betteruptime.com");
