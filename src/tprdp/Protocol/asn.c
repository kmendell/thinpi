#include <gnutls/gnutls.h>
#include <libtasn1.h>
#include <stdlib.h>

#include "../tprdp.h"
#include "asn.h"

// Generated by asn1Parser
#include "pkix_asn1_tab.c"

static asn1_node *asn_defs = NULL;

#define MAX_ERROR_DESCRIPTION_SIZE	1024
char errstr[MAX_ERROR_DESCRIPTION_SIZE];

/* Parse an ASN.1 BER header */
RD_BOOL
ber_parse_header(STREAM s, int tagval, uint32 *length)
{
	int tag, len;

	if (tagval > 0xff)
	{
		if (!s_check_rem(s, 2)) {
			return False;
		}
		in_uint16_be(s, tag);
	}
	else
	{
		if (!s_check_rem(s, 1)) {
			return False;
		}
		in_uint8(s, tag);
	}

	if (tag != tagval)
	{
		logger(Core, Error, "ber_parse_header(), expected tag %d, got %d", tagval, tag);
		return False;
	}

	if (!s_check_rem(s, 1)) {
		return False;
	}
	in_uint8(s, len);

	if (len & 0x80)
	{
		len &= ~0x80;
		if (!s_check_rem(s, len)) {
			return False;
		}
		*length = 0;
		while (len--)
			next_be(s, *length);
	}
	else
		*length = len;

	return True;
}

void
ber_out_sequence(STREAM out, STREAM content)
{
	size_t length;
	length = (content ? s_length(content) : 0);
	ber_out_header(out, BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED, length);
	if (content)
		out_stream(out, content);
}


/* Output an ASN.1 BER header */
void
ber_out_header(STREAM s, int tagval, int length)
{
	if (tagval > 0xff)
	{
		out_uint16_be(s, tagval);
	}
	else
	{
		out_uint8(s, tagval);
	}

	if (length >= 0x80)
	{
		out_uint8(s, 0x82);
		out_uint16_be(s, length);
	}
	else
		out_uint8(s, length);
}

/* Output an ASN.1 BER integer */
void
ber_out_integer(STREAM s, int value)
{
	ber_out_header(s, BER_TAG_INTEGER, 2);
	out_uint16_be(s, value);
}

RD_BOOL
ber_in_header(STREAM s, int *tagval, int *decoded_len)
{
	in_uint8(s, *tagval);
	in_uint8(s, *decoded_len);

	if (*decoded_len < 0x80)
		return True;
	else if (*decoded_len == 0x81)
	{
		in_uint8(s, *decoded_len);
		return True;
	}
	else if (*decoded_len == 0x82)
	{
		in_uint16_be(s, *decoded_len);
		return True;
	}

	return False;
}


int init_asn1_lib(void)
{
	int asn1_rv;

	if (asn_defs) {
		return 0;
	}

	asn_defs = malloc(sizeof(*asn_defs));

	if (!asn_defs) {
		logger(Core, Error, "%s:%s:%d Failed to allocate memory for  ASN.1 parser\n",
				__FILE__, __func__, __LINE__);
		return 1;
	}

	*asn_defs = NULL;

	if (ASN1_SUCCESS != (asn1_rv = asn1_array2tree(pkix_asn1_tab, asn_defs, errstr))) {
		logger(Core, Error, "%s:%s:%d Failed to init ASN.1 parser. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));

		return 1;
	}

	return 0;
}

/* Encode RSA public key into DER PKCS#1 */
/* Returns; 0 - success, 1 - fatal error, 2 - insufficient space in buffer */
int write_pkcs1_der_pubkey(const gnutls_datum_t *m, const gnutls_datum_t *e, uint8_t *out, int *out_len)
{
	int asn1_rv;
	asn1_node asn_cert;

	if (!asn_defs) {
		if (init_asn1_lib() != 0) {
			return 1;
		}
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_create_element(*asn_defs, "PKIX1Implicit88.RSAPublicKey", &asn_cert))) {
		logger(Core, Error, "%s:%s:%d Failed to create ASN.1 parser element for RSAPublicKey. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_write_value(asn_cert, "modulus", m->data, m->size))) {
		logger(Core, Error, "%s:%s:%d Failed to write modulus. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));

		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_write_value(asn_cert, "publicExponent", e->data, e->size))) {
		logger(Core, Error, "%s:%s:%d Failed to write publicExponent. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_der_coding(asn_cert, "", out, out_len, errstr))) {
		logger(Core, Error, "%s:%s:%d Failed to encode PKIX1Implicit88.RSAPublicKey. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));

		if (asn1_rv ==  ASN1_MEM_ERROR) {
			return 2;
		}

		return 1;
	}

	return 0;
}

int libtasn_read_cert_pk_oid(uint8_t *data, size_t len, char *oid, size_t *oid_size)
{
	int asn1_rv;
	asn1_node asn_cert;

	/* Parse DER encoded x.509 certificate */
	if (!asn_defs) {
		if (init_asn1_lib() != 0) {
			return 1;
		}
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_create_element(*asn_defs, "PKIX1Implicit88.Certificate", &asn_cert))) {
		logger(Core, Error, "%s:%s:%d Failed to create ASN.1 parser element. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_der_decoding(&asn_cert, data, len, errstr))) {
		logger(Core, Error, "%s:%s:%d Failed to decode certificate object. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_read_value(asn_cert, "tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm",
												   oid, (int *)oid_size)))
	{
		logger(Core, Error, "%s:%s:%d Failed to get cert's public key algorithm. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	return 0;
}

int libtasn_read_cert_pk_parameters(uint8_t *data, size_t len, gnutls_datum_t *m, gnutls_datum_t *e)
{
	int asn1_rv;
	asn1_node asn_cert;

	int buflen;
	uint8_t buf[16384];

	asn1_node asn_key;
	int nblen;
	uint8_t newbuf[16384];

	/* Parse DER encoded x.509 certificate */
	init_asn1_lib();

	if (ASN1_SUCCESS != (asn1_rv = asn1_create_element(*asn_defs, "PKIX1Implicit88.Certificate", &asn_cert))) {
		logger(Core, Error, "%s:%s:%d Failed to create ASN.1 parser element. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_der_decoding(&asn_cert, data, len, errstr))) {
		logger(Core, Error, "%s:%s:%d Failed to decode certificate object. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	buflen = sizeof(buf) - 1;
	if (ASN1_SUCCESS != (asn1_rv = asn1_read_value(asn_cert, "tbsCertificate.subjectPublicKeyInfo.subjectPublicKey", buf, &buflen))) {
		logger(Core, Error, "%s:%s:%d Failed to get cert's public key. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	if (ASN1_SUCCESS != (asn1_rv = asn1_create_element(*asn_defs, "PKIX1Implicit88.RSAPublicKey", &asn_key))) {
		logger(Core, Error, "%s:%s:%d Failed to create ASN.1 parser element. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	// As it' a BIT STRING the len constitutes the number of BITS, not BYTES
	if (ASN1_SUCCESS != (asn1_rv = asn1_der_decoding(&asn_key, buf, buflen / 8, errstr))) {
		logger(Core, Error, "%s:%s:%d Failed to decode public key  object. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	/* Get RSA public key's modulus and exponent */
	nblen = sizeof(newbuf);

	if (ASN1_SUCCESS != (asn1_rv = asn1_read_value(asn_key, "modulus", newbuf, &nblen))) {
		logger(Core, Error, "%s:%s:%d Failed to get RSA public key's modulus. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	m->size = nblen;

	if (!(m->data = malloc(m->size))) {
		logger(Core, Error, "%s:%s:%d Failed to allocate memory for modulus.\n", __FILE__, __func__, __LINE__);
		return 1;
	}

	memcpy((void *)m->data, newbuf, m->size);

	nblen = sizeof(newbuf);

	if (ASN1_SUCCESS != (asn1_rv = asn1_read_value(asn_key, "publicExponent", newbuf, &nblen))) {
		logger(Core, Error, "%s:%s:%d Failed to get RSA public key's exponent. Error = 0x%x (%s)\n",
				__FILE__, __func__, __LINE__, asn1_rv, asn1_strerror(asn1_rv));
		return 1;
	}

	e->size = nblen;

	if (!(e->data = malloc(e->size))) {
		logger(Core, Error, "%s:%s:%d Failed to allocate memory for exponent.\n", __FILE__, __func__, __LINE__);
		if (m->data) {
			free(m->data);
		}
		return 1;
	}

	memcpy((void *)e->data, newbuf, e->size);

	return 0;
}
