The source files were taken from the Amazon FreeRTOS package from 9.12.2017 and are probably mbedTLS V2.6 sources.
The minimum files were takes to develop MQTT client operation with Amazon AWS.

1. For public key generation the source files ecp.c (for main routine calls), bignum.c and ecp_curves.c are needed.
mbedtls_calloc() and mbedtls_free() are referenced and these are set temporarily to uMalloc() and uFree().

2. However, for 1 the server's certificate must first be extracted, needing x509_crt.c, which required the following modeules to be able to operate:
- asn1parse.c
- x509.c
- pem.c
- pk.c
- pk_wrap.c
- base64.c
- md.c
- md_wrap.c
- oid.c
- sha1.c
- sha256.c
- ecdsa.c
- asn1write.c
- hmac_drbg.c
- pkparse.c

3. Also, for the Diffie-Hellman server exchange parameter extraction ecdh.c is needed


It was decided that ssl_cli.c should be used due to the fact that there are various decisions made during parsing that require accurate knowledge of what is allowed and what not depending on the exact mode of operation. It also generates SSL alerts at appropriate points (errors).
This pulled in 
- ssl_ciphersuites.c

which then pulled in
- ssl_tls.c
- ciper.c
- ciper_wrap.c
- gcm.c
- aes.c
- dhm.c




