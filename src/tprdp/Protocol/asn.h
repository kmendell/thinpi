#ifndef _RDASN_H
#define _RDASN_H

#include <gnutls/gnutls.h>
#include <libtasn1.h>
#include <stdint.h>

#include "../utilities/utils.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define OID_SHA_WITH_RSA_SIGNATURE	"1.3.14.3.2.15"
#define OID_MD5_WITH_RSA_SIGNATURE	"1.3.14.3.2.25"

int init_asn1_lib(void);
int write_pkcs1_der_pubkey(const gnutls_datum_t *m, const gnutls_datum_t *e, uint8_t *out, int *out_len);

int libtasn_read_cert_pk_oid(uint8_t *data, size_t len, char *oid, size_t *oid_size);
int libtasn_read_cert_pk_parameters(uint8_t *data, size_t len, gnutls_datum_t *m, gnutls_datum_t *e);

#ifdef __cplusplus
}
#endif

#endif /* _RDASN_H */
