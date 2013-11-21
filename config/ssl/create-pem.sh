#!/bin/sh

openssl genrsa 1024 > host.key
chmod 400 host.key
openssl req -new -x509 -nodes -sha1 -days 365 -key host.key > host.cert
openssl x509 -noout -fingerprint -text < host.cert > host.info
cat host.cert host.key > host.pem
rm host.key
chmod 400 host.pem
