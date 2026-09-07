CREATE TABLE records (
    r_id      INTEGER  PRIMARY KEY,
    r_owner   TEXT     NOT NULL,
    r_type    TEXT     NOT NULL,
    ttl       REAL     DEFAULT 3600 NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
    r_data    TEXT     NOT NULL
);

INSERT INTO records (r_owner, r_type, r_data)
VALUES 
("example.com", "A", "192.0.2.10"),
("example.com", "AAAA", "2001:db8::10"),
("example.com", "TXT", "v=spf1 include:_spf.example.com ~all"),
("example.com", "SVCB", '1 . alpn="h3,h2" port="443"'),
("example.com", "NAPTR", '10 100 "s" "SIP+D2U" "" _sip._udp.example.com'),
("www.example.com", "CNAME", "example.com"),
("simplesite.com", "A", "164.38.55.1"),
("blog.simplesite.com", "CNAME", "simplesite.com"),
("api.example.com", "A", "203.0.113.15"),
("api.example.com", "AAAA", "2001:db8::20"),
("api.example.com", "SVCB", "0 pool.example.com"),
("example.herokuapp.com", "A", "52.204.31.10"),
("app.example.com", "CNAME", "example.herokuapp.com"),
("voice.example.com", "NAPTR", '20 100 "s" "SIPS+D2T" "" _sips._tcp.example.com'),
("_dmarc.example.com", "TXT", "v=DMARC1; p=reject; rua=mailto:dmarc@example.com"),
("mail.example.com", "A", "192.0.2.25"),
("mail.example.com", "AAAA", "2001:db8::25"),
("_domainkey.example.com", "TXT", "v=DKIM1; k=rsa; p=MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQ..."),
("enum.example.com", "NAPTR", '100 10 "u" "E2U+sip" "!^.*$!sip:info@example.com!" .'),
("cdn.example.com", "SVCB", '1 svcb.cdn-provider.net. alpn="h2"'),
("shops.myshopify.com", "A", "23.227.38.65"),
("shop.example.com", "CNAME", "shops.myshopify.com"),
("stats.betteruptime.com", "A", "116.203.116.108"),
("status.example.com", "CNAME", "stats.betteruptime.com");
