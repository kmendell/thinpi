

#include <time.h>
#include <iconv.h>

#ifndef _WIN32
#include <errno.h>
#include <unistd.h>
#endif
#include "../tprdp.h"
#include "../Protocol/ssl.h"


extern uint16 g_mcs_userid;
extern char *tpUsername;
extern char tpPassword[64];
extern char tpCodepage[16];
extern RD_BOOL g_orders;
extern RD_BOOL tpEncryption;
extern RD_BOOL tpDesktopSave;
extern RD_BOOL tpPolygonEllipseOrder;
extern RDP_VERSION tpRDPVersion;
extern uint16 g_server_rdp_version;
extern uint32 tpPerformanceFlags;
extern int tpServerDepth;
extern uint32 tpSessionWidth;
extern uint32 tpSessionHeight;
extern RD_BOOL tpBitmapCache;
extern RD_BOOL tpBitmapCachePersistEnable;
extern RD_BOOL tpNUMLCKSync;
extern RD_BOOL tpPendingResize;
extern RD_BOOL tpPendingResizeDefer;
extern struct timeval tpPendingResizeDeferTimer;
extern RD_BOOL tpNetworkError;
extern time_t g_wait_for_deactivate_ts;

extern RD_BOOL g_dynamic_session_resize;

RD_BOOL g_exit_mainloop = False;

size_t g_next_packet;
uint32 g_rdp_shareid;

extern RDPCOMP g_mppc_dict;

extern uint32 vc_chunk_size;

/* Session Directory support */
extern RD_BOOL tpRedirect;
extern char *tpRedirectServer;
extern uint32 tpRedirectServerLen;
extern char *tpRedirectDomain;
extern uint32 tpRedirectDomainLen;
extern char *tpRedirectUsername;
extern uint32 tpRedirectUsernameLen;
extern uint8 *tpRedirectLBInfo;
extern uint32 tpRedirectLBInforLen;
extern uint8 *tpRedirectCookie;
extern uint32 tpRedirectCookieLen;
extern uint32 tpRedirectFlags;
extern uint32 tpRedirectSessionID;

/* END Session Directory support */

extern uint32 tpReconnectID;
extern char tpReconnectRandom[16];
extern time_t tpReconnectRandomTS;
extern RD_BOOL tpReconnectHASRandom;
extern uint8 tpClientRandom[SEC_RANDOM_SIZE];
static uint32 g_packetno; 

extern RD_BOOL tpFullscreen;

/* holds the actual session size reported by server */
uint16 g_session_width;
uint16 g_session_height;

static void rdp_out_unistr(STREAM s, char *string, int len);

/* reads a TS_SHARECONTROLHEADER from stream, returns True of there is
   a PDU available otherwise False */
static RD_BOOL
rdp_ts_in_share_control_header(STREAM s, uint8 * type, uint16 * length)
{
	uint16 pdu_type;
	uint16 pdu_source;

	UNUSED(pdu_source);

	in_uint16_le(s, *length);	/* totalLength */

	/* If the totalLength field equals 0x8000, then the Share
	   Control Header and any data that follows MAY be interpreted
	   as a T.128 FlowPDU as described in [T128] section 8.5 (the
	   ASN.1 structure definition is detailed in [T128] section
	   9.1) and MUST be ignored.
	 */
	if (*length == 0x8000)
	{
		/* skip over this message in stream */
		g_next_packet += 8;
		return False;
	}

	in_uint16_le(s, pdu_type);	/* pduType */
	*type = pdu_type & 0xf;

	/* XP omits pduSource for PDUTYPE_DEACTIVATEALLPDU for some reason */
	if (*length == 4) {
		logger(Protocol, Debug,
		       "rdp_ts_in_share_control_header(), missing pduSource field for 0x%x PDU",
		       *type);
	} else {
		in_uint16(s, pdu_source);	/* pduSource */
	}

	/* Give just the size of the data */
	if (*length >= 6)
		*length -= 6;
	else
		*length = 0;

	return True;
}

/* Receive an RDP packet */
static STREAM
rdp_recv(uint8 * type)
{
	RD_BOOL is_fastpath;
	static STREAM rdp_s;
	uint16 length;

	while (1)
	{
		/* fill stream with data if needed for parsing a new packet */
		if (g_next_packet == 0)
		{
			rdp_s = sec_recv(&is_fastpath);
			if (rdp_s == NULL)
				return NULL;

			if (is_fastpath == True)
			{
				/* process_ts_fp_updates moves g_next_packet */
				process_ts_fp_updates(rdp_s);
				continue;
			}

			g_next_packet = s_tell(rdp_s);
		}
		else
		{
			s_seek(rdp_s, g_next_packet);
			if (s_check_end(rdp_s))
			{
				g_next_packet = 0;
				continue;
			}
		}

		/* parse a TS_SHARECONTROLHEADER */
		if (rdp_ts_in_share_control_header(rdp_s, type, &length) == False)
			continue;

		break;
	}

	logger(Protocol, Debug, "rdp_recv(), RDP packet #%d, type 0x%x", ++g_packetno, *type);

	if (!s_check_rem(rdp_s, length))
	{
		rdp_protocol_error("not enough data for PDU", rdp_s);
	}

	g_next_packet = s_tell(rdp_s) + length;

	return rdp_s;
}

/* Initialise an RDP data packet */
static STREAM
rdp_init_data(int maxlen)
{
	STREAM s;

	s = sec_init(tpEncryption ? SEC_ENCRYPT : 0, maxlen + 18);
	s_push_layer(s, rdp_hdr, 18);

	return s;
}

/* Send an RDP data packet */
static void
rdp_send_data(STREAM s, uint8 data_pdu_type)
{
	uint16 length;

	s_pop_layer(s, rdp_hdr);
	length = s_remaining(s);

	out_uint16_le(s, length);
	out_uint16_le(s, (RDP_PDU_DATA | 0x10));
	out_uint16_le(s, (g_mcs_userid + 1001));

	out_uint32_le(s, g_rdp_shareid);
	out_uint8(s, 0);	/* pad */
	out_uint8(s, 1);	/* streamid */
	out_uint16_le(s, (length - 14));
	out_uint8(s, data_pdu_type);
	out_uint8(s, 0);	/* compress_type */
	out_uint16(s, 0);	/* compress_len */

	sec_send(s, tpEncryption ? SEC_ENCRYPT : 0);
}

/* Output a string in Unicode with mandatory null termination. If
   string is NULL or len is 0, write an unicode null termination to
   stream. */
static void
rdp_out_unistr_mandatory_null(STREAM s, char *string, int len)
{
	/* LEGACY:
	 *
	 *  Do not write new code that uses this function, use the ones defined
	 *  in stream.h for writing utf16 strings to a stream.
	 *
	 */
	if (string && len > 0)
		rdp_out_unistr(s, string, len);
	else
		out_uint16_le(s, 0);
}

/* Output a string in Unicode */
static void
rdp_out_unistr(STREAM s, char *string, int len)
{
	/* LEGACY:
	 *
	 *  Do not write new code that uses this function, use the ones defined
	 *  in stream.h for writing utf16 strings to a stream.
	 *
	 */
	static iconv_t icv_local_to_utf16;
	size_t ibl, obl;
	char *pin;
	unsigned char *pout;


	if (string == NULL || len == 0)
		return;

	// if not already open
	if (!icv_local_to_utf16)
	{
		icv_local_to_utf16 = iconv_open(WINDOWS_CODEPAGE, tpCodepage);
		if (icv_local_to_utf16 == (iconv_t) - 1)
		{
			logger(Protocol, Error, "rdo_out_unistr(), iconv_open[%s -> %s] fail %p",
			       tpCodepage, WINDOWS_CODEPAGE, icv_local_to_utf16);
			abort();
		}
	}


	ibl = strlen(string);
	obl = len + 2;
	pin = string;
	out_uint8p(s, pout, len + 2);

	memset(pout, 0, len + 2);


	if (iconv(icv_local_to_utf16, (char **) &pin, &ibl, (char **)&pout, &obl) == (size_t) - 1)
	{
		logger(Protocol, Error, "rdp_out_unistr(), iconv(2) fail, errno %d", errno);
		abort();
	}
}

/* Input a string in Unicode
 *
 * Returns str_len of string
 */
void
rdp_in_unistr(STREAM s, int in_len, char **string, uint32 * str_size)
{
	static iconv_t icv_utf16_to_local;
	size_t ibl, obl;
	unsigned char *pin;
	char *pout;

	struct stream packet = *s;

	if ((in_len < 0) || ((uint32)in_len >= (RD_UINT32_MAX / 2)))
	{
		logger(Protocol, Error, "rdp_in_unistr(), length of unicode data is out of bounds.");
		abort();
	}

	/* Corner case. We still want to return a null terminated string... */
	if (in_len == 0) {
		if (*string == NULL)
		{
			*string = xmalloc(1);
		}
		**string = '\0';
		*str_size = 0;
		return;
	}

	if (!s_check_rem(s, in_len))
	{
		rdp_protocol_error("consume of unicode data from stream would overrun", &packet);
	}

	// if not already open
	if (!icv_utf16_to_local)
	{
		icv_utf16_to_local = iconv_open(tpCodepage, WINDOWS_CODEPAGE);
		if (icv_utf16_to_local == (iconv_t) - 1)
		{
			logger(Protocol, Error, "rdp_in_unistr(), iconv_open[%s -> %s] fail %p",
			       WINDOWS_CODEPAGE, tpCodepage, icv_utf16_to_local);
			abort();
		}
	}

	/* Dynamic allocate of destination string if not provided */
	if (*string == NULL)
	{

		*string = xmalloc(in_len * 2);
		*str_size = in_len * 2;
	}

	ibl = in_len;
	obl = *str_size - 1;
	in_uint8p(s, pin, in_len);
	pout = *string;

	if (iconv(icv_utf16_to_local, (char **) &pin, &ibl, &pout, &obl) == (size_t) - 1)
	{
		if (errno == E2BIG)
		{
			logger(Protocol, Warning,
			       "rdp_in_unistr(), server sent an unexpectedly long string, truncating");
		}
		else
		{
			logger(Protocol, Warning, "rdp_in_unistr(), iconv fail, errno %d", errno);

			free(*string);
			*string = NULL;
			*str_size = 0;
		}
		abort();
	}

	/* Always force the last byte to be a null */
	*pout = 0;

	if (*string)
		*str_size = pout - *string;
}


/* Send a Client Info PDU */
static void
rdp_send_client_info_pdu(uint32 flags, char *domain, char *user,
			 char *password, char *program, char *directory)
{
	char *ipaddr = tcp_get_address();
	/* length of string in TS_INFO_PACKET excludes null terminator */
	int len_domain = 2 * strlen(domain);
	int len_user = 2 * strlen(user);
	int len_password = 2 * strlen(password);
	int len_program = 2 * strlen(program);
	int len_directory = 2 * strlen(directory);

	/* length of strings in TS_EXTENDED_PACKET includes null terminator */
	int len_ip = 2 * strlen(ipaddr) + 2;
	int len_dll = 2 * strlen("C:\\WINNT\\System32\\mstscax.dll") + 2;

	int packetlen = 0;
	uint32 sec_flags = tpEncryption ? (SEC_INFO_PKT | SEC_ENCRYPT) : SEC_INFO_PKT;
	STREAM s;
	time_t t = time(NULL);
	time_t tzone;
	uint8 security_verifier[16];

	if (tpRDPVersion == RDP_V4 || 1 == g_server_rdp_version)
	{
		logger(Protocol, Debug, "rdp_send_logon_info(), sending RDP4-style Logon packet");

		s = sec_init(sec_flags, 18 + len_domain + len_user + len_password
			     + len_program + len_directory + 10);

		out_uint32(s, 0);
		out_uint32_le(s, flags);
		out_uint16_le(s, len_domain);
		out_uint16_le(s, len_user);
		out_uint16_le(s, len_password);
		out_uint16_le(s, len_program);
		out_uint16_le(s, len_directory);

		rdp_out_unistr_mandatory_null(s, domain, len_domain);
		rdp_out_unistr_mandatory_null(s, user, len_user);
		rdp_out_unistr_mandatory_null(s, password, len_password);
		rdp_out_unistr_mandatory_null(s, program, len_program);
		rdp_out_unistr_mandatory_null(s, directory, len_directory);
	}
	else
	{

		logger(Protocol, Debug, "rdp_send_logon_info(), sending RDP5-style Logon packet");

		if (tpRedirect == True && tpRedirectCookieLen > 0)
		{
			flags &= ~RDP_INFO_PASSWORD_IS_SC_PIN;
			flags |= RDP_INFO_AUTOLOGON;
			len_password = tpRedirectCookieLen;
			len_password -= 2;	/* subtract 2 bytes which is added below */
			logger(Protocol, Debug,
			       "rdp_send_logon_info(), Using %d bytes redirect cookie as password",
			       tpRedirectCookieLen);
		}

		packetlen =
			/* size of TS_INFO_PACKET */
			4 +	/* CodePage */
			4 +	/* flags */
			2 +	/* cbDomain */
			2 +	/* cbUserName */
			2 +	/* cbPassword */
			2 +	/* cbAlternateShell */
			2 +	/* cbWorkingDir */
			2 + len_domain +	/* Domain */
			2 + len_user +	/* UserName */
			2 + len_password +	/* Password */
			2 + len_program +	/* AlternateShell */
			2 + len_directory +	/* WorkingDir */
			/* size of TS_EXTENDED_INFO_PACKET */
			2 +	/* clientAddressFamily */
			2 +	/* cbClientAddress */
			len_ip +	/* clientAddress */
			2 +	/* cbClientDir */
			len_dll +	/* clientDir */
			/* size of TS_TIME_ZONE_INFORMATION */
			4 +	/* Bias, (UTC = local time + bias */
			64 +	/* StandardName, 32 unicode char array, Descriptive standard time on client */
			16 +	/* StandardDate */
			4 +	/* StandardBias */
			64 +	/* DaylightName, 32 unicode char array */
			16 +	/* DaylightDate */
			4 +	/* DaylightBias */
			4 +	/* clientSessionId */
			4 +	/* performanceFlags */
			2 +	/* cbAutoReconnectCookie, either 0 or 0x001c */
			/* size of ARC_CS_PRIVATE_PACKET */
			28;	/* autoReconnectCookie */


		s = sec_init(sec_flags, packetlen);

		logger(Protocol, Debug, "rdp_send_logon_info(), called sec_init with packetlen %d",
		       packetlen);

		/* TS_INFO_PACKET */
		out_uint32(s, 0);	/* Code Page */
		out_uint32_le(s, flags);
		out_uint16_le(s, len_domain);
		out_uint16_le(s, len_user);
		out_uint16_le(s, len_password);
		out_uint16_le(s, len_program);
		out_uint16_le(s, len_directory);

		rdp_out_unistr_mandatory_null(s, domain, len_domain);
		rdp_out_unistr_mandatory_null(s, user, len_user);

		if (tpRedirect == True && 0 < tpRedirectCookieLen)
		{
			out_uint8a(s, tpRedirectCookie, tpRedirectCookieLen);
		}
		else
		{
			rdp_out_unistr_mandatory_null(s, password, len_password);
		}


		rdp_out_unistr_mandatory_null(s, program, len_program);
		rdp_out_unistr_mandatory_null(s, directory, len_directory);

		/* TS_EXTENDED_INFO_PACKET */
		out_uint16_le(s, 2);	/* clientAddressFamily = AF_INET */
		out_uint16_le(s, len_ip);	/* cbClientAddress */
		rdp_out_unistr_mandatory_null(s, ipaddr, len_ip - 2);	/* clientAddress */
		out_uint16_le(s, len_dll);	/* cbClientDir */
		rdp_out_unistr_mandatory_null(s, "C:\\WINNT\\System32\\mstscax.dll", len_dll - 2);	/* clientDir */

		/* TS_TIME_ZONE_INFORMATION */
		tzone = (mktime(gmtime(&t)) - mktime(localtime(&t))) / 60;
		out_uint32_le(s, tzone);
		rdp_out_unistr(s, "GTB, normaltid", 2 * strlen("GTB, normaltid"));
		out_uint8s(s, 62 - 2 * strlen("GTB, normaltid"));
		out_uint32_le(s, 0x0a0000);
		out_uint32_le(s, 0x050000);
		out_uint32_le(s, 3);
		out_uint32_le(s, 0);
		out_uint32_le(s, 0);
		rdp_out_unistr(s, "GTB, sommartid", 2 * strlen("GTB, sommartid"));
		out_uint8s(s, 62 - 2 * strlen("GTB, sommartid"));
		out_uint32_le(s, 0x30000);
		out_uint32_le(s, 0x050000);
		out_uint32_le(s, 2);
		out_uint32(s, 0);
		out_uint32_le(s, 0xffffffc4);	/* DaylightBias */

		/* Rest of TS_EXTENDED_INFO_PACKET */
		out_uint32_le(s, 0);	/* clientSessionId (Ignored by server MUST be 0) */
		out_uint32_le(s, tpPerformanceFlags);

		/* Client Auto-Reconnect */
		if (tpReconnectHASRandom)
		{
			logger(Protocol, Debug,
			       "rdp_send_logon_info(), Sending auto-reconnect cookie.");
			out_uint16_le(s, 28);	/* cbAutoReconnectLen */
			/* ARC_CS_PRIVATE_PACKET */
			out_uint32_le(s, 28);	/* cbLen */
			out_uint32_le(s, 1);	/* Version */
			out_uint32_le(s, tpReconnectID);	/* LogonId */
			rdssl_hmac_md5(tpReconnectRandom, sizeof(tpReconnectRandom),
				       tpClientRandom, SEC_RANDOM_SIZE, security_verifier);
			out_uint8a(s, security_verifier, sizeof(security_verifier));
		}
		else
		{
			out_uint16_le(s, 0);	/* cbAutoReconnectLen */
		}

	}
	s_mark_end(s);

	/* clear the redirect flag */
	tpRedirect = False;

	sec_send(s, sec_flags);
	s_free(s);
}

/* Send a control PDU */
static void
rdp_send_control(uint16 action)
{
	STREAM s;

	s = rdp_init_data(8);

	out_uint16_le(s, action);
	out_uint16(s, 0);	/* userid */
	out_uint32(s, 0);	/* control id */

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_CONTROL);
	s_free(s);
}

/* Send a synchronisation PDU */
static void
rdp_send_synchronise(void)
{
	STREAM s;

	logger(Protocol, Debug, "%s()", __func__);

	s = rdp_init_data(4);

	out_uint16_le(s, 1);	/* type */
	out_uint16_le(s, 1002);

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_SYNCHRONISE);
	s_free(s);
}

/* Send a single input event */
void
rdp_send_input(uint32 time, uint16 message_type, uint16 device_flags, uint16 param1, uint16 param2)
{
	STREAM s;

	logger(Protocol, Debug, "%s()", __func__);

	s = rdp_init_data(16);

	out_uint16_le(s, 1);	/* number of events */
	out_uint16(s, 0);	/* pad */

	out_uint32_le(s, time);
	out_uint16_le(s, message_type);
	out_uint16_le(s, device_flags);
	out_uint16_le(s, param1);
	out_uint16_le(s, param2);

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_INPUT);
	s_free(s);
}

/* Send a Suppress Output PDU */
void
rdp_send_suppress_output_pdu(enum RDP_SUPPRESS_STATUS allowupdates)
{
	STREAM s;
	static enum RDP_SUPPRESS_STATUS current_status = ALLOW_DISPLAY_UPDATES;

	logger(Protocol, Debug, "%s()", __func__);

	if (current_status == allowupdates)
		return;

	s = rdp_init_data(12);

	out_uint8(s, allowupdates);	/* allowDisplayUpdates */
	out_uint8s(s, 3);	/* pad3Octets */

	switch (allowupdates)
	{
		case SUPPRESS_DISPLAY_UPDATES:	/* shut the server up */
			break;

		case ALLOW_DISPLAY_UPDATES:	/* receive data again */
			out_uint16_le(s, 0);	/* left */
			out_uint16_le(s, 0);	/* top */
			out_uint16_le(s, g_session_width);	/* right */
			out_uint16_le(s, g_session_height);	/* bottom */
			break;
	}

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_CLIENT_WINDOW_STATUS);
	s_free(s);
	current_status = allowupdates;
}

/* Send persistent bitmap cache enumeration PDUs */
static void
rdp_enum_bmpcache2(void)
{
	STREAM s;
	HASH_KEY keylist[BMPCACHE2_NUM_PSTCELLS];
	uint32 num_keys, offset, count, flags;

	logger(Protocol, Debug, "%s()", __func__);

	offset = 0;
	num_keys = pstcache_enumerate(2, keylist);

	while (offset < num_keys)
	{
		count = MIN(num_keys - offset, 169);

		s = rdp_init_data(24 + count * sizeof(HASH_KEY));

		flags = 0;
		if (offset == 0)
			flags |= PDU_FLAG_FIRST;
		if (num_keys - offset <= 169)
			flags |= PDU_FLAG_LAST;

		/* header */
		out_uint32_le(s, 0);
		out_uint16_le(s, count);
		out_uint16_le(s, 0);
		out_uint16_le(s, 0);
		out_uint16_le(s, 0);
		out_uint16_le(s, 0);
		out_uint16_le(s, num_keys);
		out_uint32_le(s, 0);
		out_uint32_le(s, flags);

		/* list */
		out_uint8a(s, keylist[offset], count * sizeof(HASH_KEY));

		s_mark_end(s);
		rdp_send_data(s, 0x2b);
		s_free(s);

		offset += 169;
	}
}

/* Send an (empty) font information PDU */
static void
rdp_send_fonts(uint16 seq)
{
	STREAM s;

	logger(Protocol, Debug, "%s()", __func__);

	s = rdp_init_data(8);

	out_uint16(s, 0);	/* number of fonts */
	out_uint16_le(s, 0);	/* pad? */
	out_uint16_le(s, seq);	/* unknown */
	out_uint16_le(s, 0x32);	/* entry size */

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_FONT2);
	s_free(s);
}

/* Output general capability set (TS_GENERAL_CAPABILITYSET) */
static void
rdp_out_ts_general_capabilityset(STREAM s)
{
	uint16 extraFlags = 0;
	if (tpRDPVersion >= RDP_V5)
	{
		extraFlags |= NO_BITMAP_COMPRESSION_HDR;
		extraFlags |= AUTORECONNECT_SUPPORTED;
		extraFlags |= LONG_CREDENTIALS_SUPPORTED;
		extraFlags |= FASTPATH_OUTPUT_SUPPORTED;
	}

	out_uint16_le(s, RDP_CAPSET_GENERAL);
	out_uint16_le(s, RDP_CAPLEN_GENERAL);
	out_uint16_le(s, OSMAJORTYPE_WINDOWS);	/* osMajorType */
	out_uint16_le(s, OSMINORTYPE_WINDOWSNT);	/* osMinorType */
	out_uint16_le(s, TS_CAPS_PROTOCOLVERSION);	/* protocolVersion (must be TS_CAPS_PROTOCOLVERSION) */
	out_uint16_le(s, 0);	/* pad2OctetsA */
	out_uint16_le(s, 0);	/* generalCompressionTypes (must be 0) */
	out_uint16_le(s, extraFlags);	/* extraFlags */
	out_uint16_le(s, 0);	/* updateCapabilityFlag (must be 0) */
	out_uint16_le(s, 0);	/* remoteUnshareFlag (must be 0) */
	out_uint16_le(s, 0);	/* generalCompressionLevel (must be 0) */
	out_uint8(s, 0);	/* refreshRectSupport */
	out_uint8(s, 0);	/* suppressOutputSupport */
}

/* Output bitmap capability set */
static void
rdp_out_ts_bitmap_capabilityset(STREAM s)
{
	logger(Protocol, Debug, "rdp_out_ts_bitmap_capabilityset(), %dx%d",
	       g_session_width, g_session_height);
	out_uint16_le(s, RDP_CAPSET_BITMAP);
	out_uint16_le(s, RDP_CAPLEN_BITMAP);
	out_uint16_le(s, tpServerDepth);	/* preferredBitsPerPixel */
	out_uint16_le(s, 1);	/* receive1BitPerPixel (ignored, should be 1) */
	out_uint16_le(s, 1);	/* receive4BitPerPixel (ignored, should be 1) */
	out_uint16_le(s, 1);	/* receive8BitPerPixel (ignored, should be 1) */
	out_uint16_le(s, g_session_width);	/* desktopWidth */
	out_uint16_le(s, g_session_height);	/* desktopHeight */
	out_uint16_le(s, 0);	/* pad2Octets */
	out_uint16_le(s, 1);	/* desktopResizeFlag */
	out_uint16_le(s, 1);	/* bitmapCompressionFlag (must be 1) */
	out_uint8(s, 0);	/* highColorFlags (ignored, should be 0) */
	out_uint8(s, 0);	/* drawingFlags */
	out_uint16_le(s, 1);	/* multipleRectangleSupport (must be 1) */
	out_uint16_le(s, 0);	/* pad2OctetsB */
}

/* Output order capability set */
static void
rdp_out_ts_order_capabilityset(STREAM s)
{
	uint8 order_caps[32];
	uint16 orderflags = 0;
	uint32 cachesize = 0;

	orderflags |= (NEGOTIATEORDERSUPPORT | ZEROBOUNDSDELTASSUPPORT);	/* mandatory flags */
	orderflags |= COLORINDEXSUPPORT;

	memset(order_caps, 0, 32);

	order_caps[TS_NEG_DSTBLT_INDEX] = 1;
	order_caps[TS_NEG_PATBLT_INDEX] = 1;
	order_caps[TS_NEG_SCRBLT_INDEX] = 1;
	order_caps[TS_NEG_LINETO_INDEX] = 1;
	order_caps[TS_NEG_MULTI_DRAWNINEGRID_INDEX] = 1;
	order_caps[TS_NEG_POLYLINE_INDEX] = 1;
	order_caps[TS_NEG_INDEX_INDEX] = 1;

	if (tpBitmapCache)
		order_caps[TS_NEG_MEMBLT_INDEX] = 1;

	if (tpDesktopSave)
	{
		cachesize = 230400;
		order_caps[TS_NEG_SAVEBITMAP_INDEX] = 1;
	}

	if (tpPolygonEllipseOrder)
	{
		order_caps[TS_NEG_POLYGON_SC_INDEX] = 1;
		order_caps[TS_NEG_POLYGON_CB_INDEX] = 1;
		order_caps[TS_NEG_ELLIPSE_SC_INDEX] = 1;
		order_caps[TS_NEG_ELLIPSE_CB_INDEX] = 1;
	}

	out_uint16_le(s, RDP_CAPSET_ORDER);
	out_uint16_le(s, RDP_CAPLEN_ORDER);
	out_uint8s(s, 16);	/* terminalDescriptor (ignored, should be 0) */
	out_uint8s(s, 4);	/* pad4OctetsA */
	out_uint16_le(s, 1);	/* desktopSaveXGranularity (ignored, assumed to be 1) */
	out_uint16_le(s, 20);	/* desktopSaveYGranularity (ignored, assumed to be 20) */
	out_uint16_le(s, 0);	/* Pad */
	out_uint16_le(s, 1);	/* maximumOrderLevel (ignored, should be 1) */
	out_uint16_le(s, 0);	/* numberFonts (ignored, should be 0) */
	out_uint16_le(s, orderflags);	/* orderFlags */
	out_uint8a(s, order_caps, 32);	/* orderSupport */
	out_uint16_le(s, 0);	/* textFlags (ignored) */
	out_uint16_le(s, 0);	/* orderSupportExFlags */
	out_uint32_le(s, 0);	/* pad4OctetsB */
	out_uint32_le(s, cachesize);	/* desktopSaveSize */
	out_uint16_le(s, 0);	/* pad2OctetsC */
	out_uint16_le(s, 0);	/* pad2OctetsD */
	out_uint16_le(s, 1252);	/* textANSICodePage */
	out_uint16_le(s, 0);	/* pad2OctetsE */
}

/* Output bitmap cache capability set */
static void
rdp_out_bmpcache_caps(STREAM s)
{
	int Bpp;

	logger(Protocol, Debug, "%s()", __func__);

	out_uint16_le(s, RDP_CAPSET_BMPCACHE);
	out_uint16_le(s, RDP_CAPLEN_BMPCACHE);

	Bpp = (tpServerDepth + 7) / 8;	/* bytes per pixel */
	out_uint8s(s, 24);	/* unused */
	out_uint16_le(s, 0x258);	/* entries */
	out_uint16_le(s, 0x100 * Bpp);	/* max cell size */
	out_uint16_le(s, 0x12c);	/* entries */
	out_uint16_le(s, 0x400 * Bpp);	/* max cell size */
	out_uint16_le(s, 0x106);	/* entries */
	out_uint16_le(s, 0x1000 * Bpp);	/* max cell size */
}

/* Output bitmap cache v2 capability set */
static void
rdp_out_bmpcache2_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BMPCACHE2);
	out_uint16_le(s, RDP_CAPLEN_BMPCACHE2);

	out_uint16_le(s, tpBitmapCachePersistEnable ? 2 : 0);	/* version */

	out_uint16_be(s, 3);	/* number of caches in this set */

	/* max cell size for cache 0 is 16x16, 1 = 32x32, 2 = 64x64, etc */
	out_uint32_le(s, BMPCACHE2_C0_CELLS);
	out_uint32_le(s, BMPCACHE2_C1_CELLS);
	if (pstcache_init(2))
	{
		out_uint32_le(s, BMPCACHE2_NUM_PSTCELLS | BMPCACHE2_FLAG_PERSIST);
	}
	else
	{
		out_uint32_le(s, BMPCACHE2_C2_CELLS);
	}
	out_uint8s(s, 20);	/* other bitmap caches not used */
}

/* Output control capability set */
static void
rdp_out_control_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_CONTROL);
	out_uint16_le(s, RDP_CAPLEN_CONTROL);

	out_uint16(s, 0);	/* Control capabilities */
	out_uint16(s, 0);	/* Remote detach */
	out_uint16_le(s, 2);	/* Control interest */
	out_uint16_le(s, 2);	/* Detach interest */
}

/* Output activation capability set */
static void
rdp_out_activate_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_ACTIVATE);
	out_uint16_le(s, RDP_CAPLEN_ACTIVATE);

	out_uint16(s, 0);	/* Help key */
	out_uint16(s, 0);	/* Help index key */
	out_uint16(s, 0);	/* Extended help key */
	out_uint16(s, 0);	/* Window activate */
}

/* Output pointer capability set */
static void
rdp_out_pointer_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_POINTER);
	out_uint16_le(s, RDP_CAPLEN_POINTER);

	out_uint16(s, 0);	/* Colour pointer */
	out_uint16_le(s, 20);	/* Cache size */
}

/* Output new pointer capability set */
static void
rdp_out_newpointer_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_POINTER);
	out_uint16_le(s, RDP_CAPLEN_NEWPOINTER);

	out_uint16_le(s, 1);	/* Colour pointer */
	out_uint16_le(s, 20);	/* Cache size */
	out_uint16_le(s, 20);	/* Cache size for new pointers */
}

/* Output share capability set */
static void
rdp_out_share_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_SHARE);
	out_uint16_le(s, RDP_CAPLEN_SHARE);

	out_uint16(s, 0);	/* userid */
	out_uint16(s, 0);	/* pad */
}

/* Output colour cache capability set */
static void
rdp_out_colcache_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_COLCACHE);
	out_uint16_le(s, RDP_CAPLEN_COLCACHE);

	out_uint16_le(s, 6);	/* cache size */
	out_uint16(s, 0);	/* pad */
}

/* Output brush cache capability set */
static void
rdp_out_brushcache_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BRUSHCACHE);
	out_uint16_le(s, RDP_CAPLEN_BRUSHCACHE);
	out_uint32_le(s, 1);	/* cache type */
}

/* 2.2.7.1.10 MS-RDPBCGR */
/* Output virtual channel capability set */
static void
rdp_out_virtchan_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_VC);
	out_uint16_le(s, RDP_CAPLEN_VC);
	/* VCCAPS_COMPR_SC */
	out_uint32_le(s, 0x00000001);	/* compression flags */
}

static void
rdp_process_virtchan_caps(STREAM s)
{
	uint32 flags, chunk_size;

	in_uint32_le(s, flags);
	in_uint32_le(s, chunk_size);

	UNUSED(flags);
	vc_chunk_size = chunk_size;
}

/* Output Input Capability Set */
static void
rdp_out_ts_input_capabilityset(STREAM s)
{
	uint16 inputflags = 0;
	inputflags |= INPUT_FLAG_SCANCODES;

	out_uint16_le(s, RDP_CAPSET_INPUT);
	out_uint16_le(s, RDP_CAPLEN_INPUT);

	out_uint16_le(s, inputflags);	/* inputFlags */
	out_uint16_le(s, 0);	/* pad2OctetsA */
	out_uint32_le(s, 0x409);	/* keyboardLayout */
	out_uint32_le(s, 0x4);	/* keyboardType */
	out_uint32_le(s, 0);	/* keyboardSubtype */
	out_uint32_le(s, 0xC);	/* keyboardFunctionKey */
	out_utf16s_padded(s, "", 64, 0);	/* imeFileName */
}

/* Output Sound Capability Set */
static void
rdp_out_ts_sound_capabilityset(STREAM s)
{
	uint16 soundflags = SOUND_BEEPS_FLAG;

	out_uint16_le(s, RDP_CAPSET_SOUND);
	out_uint16_le(s, RDP_CAPLEN_SOUND);

	out_uint16_le(s, soundflags);	/* soundFlags */
	out_uint16_le(s, 0);	/* pad2OctetsA */
}

/* Output Font Capability Set */
static void
rdp_out_ts_font_capabilityset(STREAM s)
{
	uint16 flags = FONTSUPPORT_FONTLIST;

	out_uint16_le(s, RDP_CAPSET_FONT);
	out_uint16_le(s, RDP_CAPLEN_FONT);

	out_uint16_le(s, flags);	/* fontSupportFlags */
	out_uint16_le(s, 0);	/* pad2octets */
}

static void
rdp_out_ts_cache_definition(STREAM s, uint16 entries, uint16 maxcellsize)
{
	out_uint16_le(s, entries);
	out_uint16_le(s, maxcellsize);
}

/* Output Glyph Cache Capability Set */
static void
rdp_out_ts_glyphcache_capabilityset(STREAM s)
{
	uint16 supportlvl = GLYPH_SUPPORT_FULL;
	uint32 fragcache = 0x01000100;
	out_uint16_le(s, RDP_CAPSET_GLYPHCACHE);
	out_uint16_le(s, RDP_CAPLEN_GLYPHCACHE);

	/* GlyphCache - 10 TS_CACHE_DEFINITION structures */
	rdp_out_ts_cache_definition(s, 254, 4);
	rdp_out_ts_cache_definition(s, 254, 4);
	rdp_out_ts_cache_definition(s, 254, 8);
	rdp_out_ts_cache_definition(s, 254, 8);
	rdp_out_ts_cache_definition(s, 254, 16);
	rdp_out_ts_cache_definition(s, 254, 32);
	rdp_out_ts_cache_definition(s, 254, 64);
	rdp_out_ts_cache_definition(s, 254, 128);
	rdp_out_ts_cache_definition(s, 254, 256);
	rdp_out_ts_cache_definition(s, 64, 2048);

	out_uint32_le(s, fragcache);	/* FragCache */
	out_uint16_le(s, supportlvl);	/* GlyphSupportLevel */
	out_uint16_le(s, 0);	/* pad2octets */
}

static void
rdp_out_ts_multifragmentupdate_capabilityset(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_MULTIFRAGMENTUPDATE);
	out_uint16_le(s, RDP_CAPLEN_MULTIFRAGMENTUPDATE);
	out_uint32_le(s, RDESKTOP_FASTPATH_MULTIFRAGMENT_MAX_SIZE);	/* MaxRequestSize */
}

static void
rdp_out_ts_large_pointer_capabilityset(STREAM s)
{
	uint16 flags = LARGE_POINTER_FLAG_96x96;

	out_uint16_le(s, RDP_CAPSET_LARGE_POINTER);
	out_uint16_le(s, RDP_CAPLEN_LARGE_POINTER);
	out_uint16_le(s, flags);	/* largePointerSupportFlags */
}

#define RDP5_FLAG 0x0030
/* Send a confirm active PDU */
static void
rdp_send_confirm_active(void)
{
	STREAM s;
	uint32 sec_flags = tpEncryption ? (RDP5_FLAG | SEC_ENCRYPT) : RDP5_FLAG;
	uint16 caplen =
		RDP_CAPLEN_GENERAL +
		RDP_CAPLEN_BITMAP +
		RDP_CAPLEN_ORDER +
		RDP_CAPLEN_COLCACHE +
		RDP_CAPLEN_ACTIVATE +
		RDP_CAPLEN_CONTROL +
		RDP_CAPLEN_SHARE +
		RDP_CAPLEN_BRUSHCACHE +
		RDP_CAPLEN_INPUT +
		RDP_CAPLEN_FONT +
		RDP_CAPLEN_SOUND +
		RDP_CAPLEN_GLYPHCACHE +
		RDP_CAPLEN_MULTIFRAGMENTUPDATE +
		RDP_CAPLEN_LARGE_POINTER +
		RDP_CAPLEN_VC + 4 /* w2k fix, sessionid */ ;

	logger(Protocol, Debug, "%s()", __func__);

	if (tpRDPVersion >= RDP_V5)
	{
		caplen += RDP_CAPLEN_BMPCACHE2;
		caplen += RDP_CAPLEN_NEWPOINTER;
	}
	else
	{
		caplen += RDP_CAPLEN_BMPCACHE;
		caplen += RDP_CAPLEN_POINTER;
	}

	s = sec_init(sec_flags, 6 + 14 + caplen + sizeof(RDP_SOURCE));

	out_uint16_le(s, 2 + 14 + caplen + sizeof(RDP_SOURCE));
	out_uint16_le(s, (RDP_PDU_CONFIRM_ACTIVE | 0x10));	/* Version 1 */
	out_uint16_le(s, (g_mcs_userid + 1001));

	out_uint32_le(s, g_rdp_shareid);
	out_uint16_le(s, 0x3ea);	/* userid */
	out_uint16_le(s, sizeof(RDP_SOURCE));
	out_uint16_le(s, caplen);

	out_uint8a(s, RDP_SOURCE, sizeof(RDP_SOURCE));
	out_uint16_le(s, 17);	/* num_caps */
	out_uint8s(s, 2);	/* pad */

	rdp_out_ts_general_capabilityset(s);
	rdp_out_ts_bitmap_capabilityset(s);
	rdp_out_ts_order_capabilityset(s);
	if (tpRDPVersion >= RDP_V5)
	{
		rdp_out_bmpcache2_caps(s);
		rdp_out_newpointer_caps(s);
	}
	else
	{
		rdp_out_bmpcache_caps(s);
		rdp_out_pointer_caps(s);
	}
	rdp_out_colcache_caps(s);
	rdp_out_activate_caps(s);
	rdp_out_control_caps(s);
	rdp_out_share_caps(s);
	rdp_out_brushcache_caps(s);
	rdp_out_virtchan_caps(s);

	rdp_out_ts_input_capabilityset(s);
	rdp_out_ts_sound_capabilityset(s);
	rdp_out_ts_font_capabilityset(s);
	rdp_out_ts_glyphcache_capabilityset(s);
	rdp_out_ts_multifragmentupdate_capabilityset(s);
	rdp_out_ts_large_pointer_capabilityset(s);

	s_mark_end(s);
	sec_send(s, sec_flags);
	s_free(s);
}

/* Process a general capability set */
static void
rdp_process_general_caps(STREAM s)
{
	uint16 pad2octetsB;	/* rdp5 flags? */

	logger(Protocol, Debug, "%s()", __func__);

	in_uint8s(s, 10);
	in_uint16_le(s, pad2octetsB);

	if (!pad2octetsB)
		tpRDPVersion = RDP_V4;
}

static RD_BOOL g_first_bitmap_caps = True;

/* Process a bitmap capability set */
static void
rdp_process_bitmap_caps(STREAM s)
{

	uint16 depth;

	logger(Protocol, Debug, "%s()", __func__);

	in_uint16_le(s, depth);
	in_uint8s(s, 6);

	in_uint16_le(s, g_session_width);
	in_uint16_le(s, g_session_height);

	logger(Protocol, Debug,
	       "rdp_process_bitmap_caps(), setting desktop size and depth to: %dx%dx%d",
	       g_session_width, g_session_height, depth);

	/* Detect if we can have dynamic session resize enabled, only once. */
	if (g_first_bitmap_caps == True && !(g_session_width == tpSessionWidth
					     && g_session_height == tpSessionHeight))
	{
		logger(Core, Notice, "Disabling dynamic session resize");
		g_dynamic_session_resize = False;
	}
	g_first_bitmap_caps = False;

	/*
	 * The server may limit depth and change the size of the desktop (for
	 * example when shadowing another session).
	 */
	if (tpServerDepth != depth)
	{
		logger(Core, Verbose,
		       "Remote desktop does not support colour depth %d; falling back to %d",
		       tpServerDepth, depth);
		tpServerDepth = depth;
	}

	/* Resize window size to match session size, except when we're in
	   fullscreen, where we want the window to always cover the entire
	   screen. */

	if (tpFullscreen == True)
		return;

	/* If dynamic session resize is disabled, set window size hints to
	   fixed session size */
	if (g_dynamic_session_resize == False)
	{
		ui_update_window_sizehints(g_session_width, g_session_height);
		return;
	}

	ui_resize_window(g_session_width, g_session_height);
}

/* Process server capabilities */
static void
rdp_process_server_caps(STREAM s, uint16 length)
{
	int n;
	size_t next, start;
	uint16 ncapsets, capset_type, capset_length;

	logger(Protocol, Debug, "%s()", __func__);

	start = s_tell(s);

	in_uint16_le(s, ncapsets);
	in_uint8s(s, 2);	/* pad */

	for (n = 0; n < ncapsets; n++)
	{
		if (s_tell(s) > start + length)
			return;

		in_uint16_le(s, capset_type);
		in_uint16_le(s, capset_length);

		next = s_tell(s) + capset_length - 4;

		switch (capset_type)
		{
			case RDP_CAPSET_GENERAL:
				rdp_process_general_caps(s);
				break;

			case RDP_CAPSET_BITMAP:
				rdp_process_bitmap_caps(s);
				break;
			case RDP_CAPSET_VC:
				/* Parse only if we got VCChunkSize */
				if (capset_length > 8) {
					rdp_process_virtchan_caps(s);
				}
				break;
		}

		s_seek(s, next);
	}
}

/* Respond to a demand active PDU */
static void
process_demand_active(STREAM s)
{
	uint8 type;
	uint16 len_src_descriptor, len_combined_caps;
	struct stream packet = *s;

	/* at this point we need to ensure that we have ui created */
	rd_create_ui();

	in_uint32_le(s, g_rdp_shareid);
	in_uint16_le(s, len_src_descriptor);
	in_uint16_le(s, len_combined_caps);

	if (!s_check_rem(s, len_src_descriptor))
	{
		rdp_protocol_error("consume of source descriptor from stream would overrun", &packet);
	}
	in_uint8s(s, len_src_descriptor);

	logger(Protocol, Debug, "process_demand_active(), shareid=0x%x", g_rdp_shareid);

	rdp_process_server_caps(s, len_combined_caps);

	rdp_send_confirm_active();
	rdp_send_synchronise();
	rdp_send_control(RDP_CTL_COOPERATE);
	rdp_send_control(RDP_CTL_REQUEST_CONTROL);
	rdp_recv(&type);	/* RDP_PDU_SYNCHRONIZE */
	rdp_recv(&type);	/* RDP_CTL_COOPERATE */
	rdp_recv(&type);	/* RDP_CTL_GRANT_CONTROL */
	rdp_send_input(0, RDP_INPUT_SYNCHRONIZE, 0,
		       tpNUMLCKSync ? ui_get_numlock_state(read_keyboard_state()) : 0, 0);

	if (tpRDPVersion >= RDP_V5)
	{
		rdp_enum_bmpcache2();
		rdp_send_fonts(3);
	}
	else
	{
		rdp_send_fonts(1);
		rdp_send_fonts(2);
	}

	rdp_recv(&type);	/* RDP_PDU_UNKNOWN 0x28 (Fonts?) */
	reset_order_state();
}

/* Process a colour pointer PDU */
static void
process_colour_pointer_common(STREAM s, int bpp)
{
	extern RD_BOOL tpLocalCursor;
	uint16 width, height, cache_idx, masklen, datalen;
	uint16 x, y;
	uint8 *mask;
	uint8 *data;
	RD_HCURSOR cursor;

	in_uint16_le(s, cache_idx);
	in_uint16_le(s, x);
	in_uint16_le(s, y);
	in_uint16_le(s, width);
	in_uint16_le(s, height);
	in_uint16_le(s, masklen);
	in_uint16_le(s, datalen);
	in_uint8p(s, data, datalen);
	in_uint8p(s, mask, masklen);

	logger(Protocol, Debug,
	       "process_colour_pointer_common(), new pointer %d with width %d and height %d",
	       cache_idx, width, height);

	/* keep hotspot within cursor bounding box */
	x = MIN(x, width - 1);
	y = MIN(y, height - 1);
	if (tpLocalCursor)
		return;		/* don't bother creating a cursor we won't use */
	cursor = ui_create_cursor(x, y, width, height, mask, data, bpp);
	ui_set_cursor(cursor);
	cache_put_cursor(cache_idx, cursor);
}

/* Process a colour pointer PDU */
void
process_colour_pointer_pdu(STREAM s)
{
	logger(Protocol, Debug, "%s()", __func__);

	process_colour_pointer_common(s, 24);
}

/* Process a New Pointer PDU - these pointers have variable bit depth */
void
process_new_pointer_pdu(STREAM s)
{
	int xor_bpp;
	logger(Protocol, Debug, "%s()", __func__);


	in_uint16_le(s, xor_bpp);
	process_colour_pointer_common(s, xor_bpp);
}

/* Process a cached pointer PDU */
void
process_cached_pointer_pdu(STREAM s)
{
	uint16 cache_idx;
	logger(Protocol, Debug, "%s()", __func__);


	in_uint16_le(s, cache_idx);
	ui_set_cursor(cache_get_cursor(cache_idx));
}

/* Process a system pointer PDU */
void
process_system_pointer_pdu(STREAM s)
{
	uint32 system_pointer_type;
	logger(Protocol, Debug, "%s()", __func__);

	in_uint32_le(s, system_pointer_type);

	set_system_pointer(system_pointer_type);
}

/* Set a given system pointer */
void
set_system_pointer(uint32 ptr)
{
	switch (ptr)
	{
		case SYSPTR_NULL:
			ui_set_null_cursor();
			break;
		case SYSPTR_DEFAULT:
			ui_set_standard_cursor();
			break;
		default:
			logger(Protocol, Warning,
			       "set_system_pointer(), unhandled pointer type 0x%x", ptr);
	}
}

/* Process a pointer PDU */
static void
process_pointer_pdu(STREAM s)
{
	uint16 message_type;
	uint16 x, y;

	logger(Protocol, Debug, "%s()", __func__);

	in_uint16_le(s, message_type);
	in_uint8s(s, 2);	/* pad */

	switch (message_type)
	{
		case RDP_POINTER_MOVE:
			in_uint16_le(s, x);
			in_uint16_le(s, y);
			ui_move_pointer(x, y);
			break;

		case RDP_POINTER_COLOR:
			process_colour_pointer_pdu(s);
			break;

		case RDP_POINTER_CACHED:
			process_cached_pointer_pdu(s);
			break;

		case RDP_POINTER_SYSTEM:
			process_system_pointer_pdu(s);
			break;

		case RDP_POINTER_NEW:
			process_new_pointer_pdu(s);
			break;

		default:
			logger(Protocol, Warning,
			       "process_pointer_pdu(), unhandled message type 0x%x", message_type);
	}
}

/* Process TS_BITMAP_DATA */
static void
process_bitmap_data(STREAM s)
{
	uint16 left, top, right, bottom, width, height;
	uint16 cx, cy, bpp, Bpp, flags, bufsize, size;
	uint8 *data, *bmpdata;
	
	logger(Protocol, Debug, "%s()", __func__);

	struct stream packet = *s;

	in_uint16_le(s, left); /* destLeft */
	in_uint16_le(s, top); /* destTop */
	in_uint16_le(s, right); /* destRight */
	in_uint16_le(s, bottom); /* destBottom */
	in_uint16_le(s, width); /* width */
	in_uint16_le(s, height); /* height */
	in_uint16_le(s, bpp); /*bitsPerPixel */
	Bpp = (bpp + 7) / 8;
	in_uint16_le(s, flags); /* flags */
	in_uint16_le(s, bufsize); /* bitmapLength */

	cx = right - left + 1;
	cy = bottom - top + 1;

	/* FIXME: There are a assumtion that we do not consider in
		this code. The value of bpp is not passed to
		ui_paint_bitmap() which relies on g_server_bpp for drawing
		the bitmap data.

		Does this means that we can sanity check bpp with g_server_bpp ?
	*/

	if (Bpp == 0 || width == 0 || height == 0)
	{
        logger(Protocol, Warning, "%s(), [%d,%d,%d,%d], [%d,%d], bpp=%d, flags=%x", __func__,
				left, top, right, bottom, width, height, bpp, flags);
		rdp_protocol_error("TS_BITMAP_DATA, unsafe size of bitmap data received from server", &packet);
	}

	if ((RD_UINT32_MAX / Bpp) <= (width * height))
	{
		logger(Protocol, Warning, "%s(), [%d,%d,%d,%d], [%d,%d], bpp=%d, flags=%x", __func__,
				left, top, right, bottom, width, height, bpp, flags);
		rdp_protocol_error("TS_BITMAP_DATA, unsafe size of bitmap data received from server", &packet);
	}
 
	if (flags == 0)
	{
		/* read uncompressed bitmap data */
		int y;
		bmpdata = (uint8 *) xmalloc(width * height * Bpp);
		for (y = 0; y < height; y++)
		{
			in_uint8a(s, &bmpdata[(height - y - 1) * (width * Bpp)], width * Bpp);
		}
		
		ui_paint_bitmap(left, top, cx, cy, width, height, bmpdata);
		xfree(bmpdata);
		return;
	}

	if (flags & NO_BITMAP_COMPRESSION_HDR)
	{
		size = bufsize;
	}
	else
	{
		/* Read TS_CD_HEADER */
		in_uint8s(s, 2);        /* skip cbCompFirstRowSize (must be 0x0000) */
		in_uint16_le(s, size);  /* cbCompMainBodySize */
		in_uint8s(s, 2);        /* skip cbScanWidth */
		in_uint8s(s, 2);        /* skip cbUncompressedSize */
	}

	/* read compressed bitmap data */
	if (!s_check_rem(s, size))
	{
		rdp_protocol_error("consume of bitmap data from stream would overrun", &packet);
	}
	in_uint8p(s, data, size);
	bmpdata = (uint8 *) xmalloc(width * height * Bpp);
	if (bitmap_decompress(bmpdata, width, height, data, size, Bpp))
	{
		ui_paint_bitmap(left, top, cx, cy, width, height, bmpdata);
	}
	else
	{
		logger(Protocol, Warning, "%s(), failed to decompress bitmap", __func__);
	}

	xfree(bmpdata);
}

/* Process TS_UPDATE_BITMAP_DATA */
void
process_bitmap_updates(STREAM s)
{
	int i;
	uint16 num_updates;
	
	in_uint16_le(s, num_updates);   /* rectangles */

	for (i = 0; i < num_updates; i++)
	{
		process_bitmap_data(s);
	}
}

/* Process a palette update */
void
process_palette(STREAM s)
{
	COLOURENTRY *entry;
	COLOURMAP map;
	RD_HCOLOURMAP hmap;
	int i;

	in_uint8s(s, 2);	/* pad */
	in_uint16_le(s, map.ncolours);
	in_uint8s(s, 2);	/* pad */

	map.colours = (COLOURENTRY *) xmalloc(sizeof(COLOURENTRY) * map.ncolours);

	logger(Graphics, Debug, "process_palette(), colour count %d", map.ncolours);

	for (i = 0; i < map.ncolours; i++)
	{
		entry = &map.colours[i];
		in_uint8(s, entry->red);
		in_uint8(s, entry->green);
		in_uint8(s, entry->blue);
	}

	hmap = ui_create_colourmap(&map);
	ui_set_colourmap(hmap);

	xfree(map.colours);
}

/* Process an update PDU */
static void
process_update_pdu(STREAM s)
{
	uint16 update_type, count;

	in_uint16_le(s, update_type);

	ui_begin_update();
	switch (update_type)
	{
		case RDP_UPDATE_ORDERS:
			logger(Protocol, Debug, "%s(), RDP_UPDATE_ORDERS", __func__);

			in_uint8s(s, 2);	/* pad */
			in_uint16_le(s, count);
			in_uint8s(s, 2);	/* pad */
			process_orders(s, count);
			break;

		case RDP_UPDATE_BITMAP:
			logger(Protocol, Debug, "%s(), RDP_UPDATE_BITMAP", __func__);
			process_bitmap_updates(s);
			break;

		case RDP_UPDATE_PALETTE:
			logger(Protocol, Debug, "%s(), RDP_UPDATE_PALETTE", __func__);
			process_palette(s);
			break;

		case RDP_UPDATE_SYNCHRONIZE:
			logger(Protocol, Debug, "%s(), RDP_UPDATE_SYNCHRONIZE", __func__);
			break;

		default:
			logger(Protocol, Warning, "process_update_pdu(), unhandled update type %d",
			       update_type);
	}
	ui_end_update();
}


/* Process TS_LOGIN_INFO_EXTENDED data structure */
static void
process_ts_logon_info_extended(STREAM s)
{
	uint32 fieldspresent;
	uint32 len;
	uint32 version;

	logger(Protocol, Debug, "%s()", __func__);

	in_uint8s(s, 2);	/* Length */
	in_uint32_le(s, fieldspresent);
	if (fieldspresent & LOGON_EX_AUTORECONNECTCOOKIE)
	{
		/* TS_LOGON_INFO_FIELD */
		in_uint8s(s, 4);	/* cbFieldData */

		/* ARC_SC_PRIVATE_PACKET */
		in_uint32_le(s, len);
		if (len != 28)
		{
			logger(Protocol, Error,
			       "process_ts_logon_info_extended(), invalid length in Auto-Reconnect packet");
			return;
		}

		in_uint32_le(s, version);
		if (version != 1)
		{
			logger(Protocol, Error,
			       "process_ts_logon_info_extended(), unsupported version of Auto-Reconnect packet");
			return;
		}

		in_uint32_le(s, tpReconnectID);
		in_uint8a(s, tpReconnectRandom, 16);
		tpReconnectHASRandom = True;
		tpReconnectRandomTS = time(NULL);
		logger(Protocol, Debug,
		       "process_ts_logon_info_extended(), saving Auto-Reconnect cookie, id=%u",
		       tpReconnectID);

		gettimeofday(&tpPendingResizeDeferTimer, NULL);
	}
}

/* Process TS_SAVE_SESSION_INFO_PDU_DATA data structure */
void
process_pdu_logon(STREAM s)
{
	uint32 infotype;
	in_uint32_le(s, infotype);

	switch (infotype)
	{
		case INFOTYPE_LOGON_PLAINNOTIFY:	/* TS_PLAIN_NOTIFY */
			logger(Protocol, Debug,
			       "process_pdu_logon(), Received TS_LOGIN_PLAIN_NOTIFY");
			in_uint8s(s, 576);	/* pad */
			break;

		case INFOTYPE_LOGON_EXTENDED_INF:	/* TS_LOGON_INFO_EXTENDED */
			logger(Protocol, Debug,
			       "process_pdu_logon(), Received TS_LOGIN_INFO_EXTENDED");
			process_ts_logon_info_extended(s);
			break;

		default:
			logger(Protocol, Warning,
			       "process_pdu_logon(), Unhandled login infotype %d", infotype);
	}
}


/* Process a Set Error Info PDU */
static void
process_ts_set_error_info_pdu(STREAM s, uint32 * ext_disc_reason)
{
	in_uint32_le(s, *ext_disc_reason);

	logger(Protocol, Debug, "process_ts_set_error_info_pdu(), error info = %d",
	       *ext_disc_reason);
}

/* Process data PDU */
static RD_BOOL
process_data_pdu(STREAM s, uint32 * ext_disc_reason)
{
	uint8 data_pdu_type;
	uint8 ctype;
	uint16 clen;
	uint32 len;

	uint8 *buf;
	uint32 roff, rlen;

	struct stream *ns = &(g_mppc_dict.ns);

	in_uint8s(s, 6);	/* shareid, pad, streamid */
	in_uint16_le(s, len);
	in_uint8(s, data_pdu_type);
	in_uint8(s, ctype);
	in_uint16_le(s, clen);
	clen -= 18;

	if (ctype & RDP_MPPC_COMPRESSED)
	{
		if (len > RDP_MPPC_DICT_SIZE)
			logger(Protocol, Error,
			       "process_data_pdu(), error decompressed packet size exceeds max");
		in_uint8p(s, buf, clen);
		if (mppc_expand(buf, clen, ctype, &roff, &rlen) == -1)
			logger(Protocol, Error,
			       "process_data_pdu(), error while decompressing packet");

		/* len -= 18; */

		/* allocate memory and copy the uncompressed data into the temporary stream */
		s_realloc(ns, rlen);
		s_reset(ns);

		out_uint8a(ns, (unsigned char *) (g_mppc_dict.hist + roff), rlen);

		s_mark_end(ns);
		s_seek(ns, 0);
		s_push_layer(ns, rdp_hdr, 0);

		s = ns;
	}

	switch (data_pdu_type)
	{
		case RDP_DATA_PDU_UPDATE:
			process_update_pdu(s);
			break;

		case RDP_DATA_PDU_CONTROL:
			logger(Protocol, Debug, "process_data_pdu(), received Control PDU");
			break;

		case RDP_DATA_PDU_SYNCHRONISE:
			logger(Protocol, Debug, "process_data_pdu(), received Sync PDU");
			break;

		case RDP_DATA_PDU_POINTER:
			process_pointer_pdu(s);
			break;

		case RDP_DATA_PDU_BELL:
			ui_bell();
			break;

		case RDP_DATA_PDU_LOGON:
			logger(Protocol, Debug, "process_data_pdu(), received Logon PDU");
			/* User logged on */
			process_pdu_logon(s);
			break;

		case RDP_DATA_PDU_SET_ERROR_INFO:
			process_ts_set_error_info_pdu(s, ext_disc_reason);

			/* We used to return true and disconnect immediately here, but
			 * Windows Vista sends a disconnect PDU with reason 0 when
			 * reconnecting to a disconnected session, and MSTSC doesn't
			 * drop the connection.  I think we should just save the status.
			 */
			break;

		case RDP_DATA_PDU_AUTORECONNECT_STATUS:
			logger(Protocol, Warning,
			       "process_data_pdu(), automatic reconnect using cookie, failed");
			break;

		default:
			logger(Protocol, Warning, "process_data_pdu(), unhandled data PDU type %d",
			       data_pdu_type);
	}
	return False;
}

/* Process redirect PDU from Session Directory */
static RD_BOOL
process_redirect_pdu(STREAM s, RD_BOOL enhanced_redirect /*, uint32 * ext_disc_reason */ )
{
	uint32 len;
	uint16 redirect_identifier;

	logger(Protocol, Debug, "%s()", __func__);

	/* reset any previous redirection information */
	tpRedirect = True;
	free(tpRedirectServerLen);
	free(tpRedirectUsername);
	free(tpRedirectDomain);
	free(tpRedirectLBInfo);
	free(tpRedirectCookie);

	tpRedirectServerLen = NULL;
	tpRedirectUsername = NULL;
	tpRedirectDomain = NULL;
	tpRedirectLBInfo = NULL;
	tpRedirectCookie = NULL;

	/* these 2 bytes are unknown, seem to be zeros */
	in_uint8s(s, 2);

	/* FIXME: Previous implementation only reads 4 bytes which has been working
	   but todays spec says something different. Investigate and retest
	   server redirection using WTS 2003 cluster.
	 */

	if (enhanced_redirect)
	{
		/* read identifier */
		in_uint16_le(s, redirect_identifier);
		if (redirect_identifier != 0x0400)
			logger(Protocol, Error, "unexpected data in server redirection packet");

		/* FIXME: skip total length */
		in_uint8s(s, 2);

		/* read session_id */
		in_uint32_le(s, tpRedirectSessionID);
	}

	/* read connection flags */
	in_uint32_le(s, tpRedirectFlags);

	if (tpRedirectFlags & LB_TARGET_NET_ADDRESS)
	{
		/* read length of ip string */
		in_uint32_le(s, len);

		/* read ip string */
		rdp_in_unistr(s, len, &tpRedirectServerLen, &tpRedirectServerLen);
	}

	if (tpRedirectFlags & LB_LOAD_BALANCE_INFO)
	{
		/* read length of load balance info blob */
		in_uint32_le(s, tpRedirect);

		/* reallocate a loadbalance info blob */
		if (tpRedirectLBInfo != NULL)
			free(tpRedirectLBInfo);

		tpRedirectLBInfo = xmalloc(tpRedirect);

		/* read load balance info blob */
		in_uint8p(s, tpRedirectLBInfo, tpRedirect);
	}

	if (tpRedirectFlags & LB_USERNAME)
	{
		/* read length of username string */
		in_uint32_le(s, len);

		/* read username string */
		rdp_in_unistr(s, len, &tpRedirectUsername, &tpRedirectUsernameLen);
	}

	if (tpRedirectFlags & LB_DOMAIN)
	{
		/* read length of domain string */
		in_uint32_le(s, len);

		/* read domain string */
		rdp_in_unistr(s, len, &tpRedirectDomain, &tpRedirectDomainLen);
	}

	if (tpRedirectFlags & LB_PASSWORD)
	{
		/* the information in this blob is either a password or a cookie that
		   should be passed though as blob and not parsed as a unicode string */

		/* read blob length */
		in_uint32_le(s, tpRedirectCookieLen);

		/* reallocate cookie blob */
		if (tpRedirectCookie != NULL)
			free(tpRedirectCookie);

		tpRedirectCookie = xmalloc(tpRedirectCookieLen);

		/* read cookie as is */
		in_uint8a(s, tpRedirectCookie, tpRedirectCookieLen);

		logger(Protocol, Debug, "process_redirect_pdu(), Read %d bytes redirection cookie",
		       tpRedirectCookieLen);
	}

	if (tpRedirectFlags & LB_DONTSTOREUSERNAME)
	{
		logger(Protocol, Warning,
		       "process_redirect_pdu(), unhandled LB_DONTSTOREUSERNAME set");
	}

	if (tpRedirectFlags & LB_SMARTCARD_LOGON)
	{
		logger(Protocol, Warning,
		       "process_redirect_pdu(), unhandled LB_SMARTCARD_LOGON set");
	}

	if (tpRedirectFlags & LB_NOREDIRECT)
	{
		/* By spec this is only for information and doesn't mean that an actual
		   redirect should be performed. How it should be used is not mentioned. */
		tpRedirect = False;
	}

	if (tpRedirectFlags & LB_TARGET_FQDN)
	{
		in_uint32_le(s, len);

		/* Let target FQDN replace target IP address */
		if (tpRedirectServerLen)
		{
			free(tpRedirectServerLen);
			tpRedirectServerLen = NULL;
		}

		/* read FQDN string */
		rdp_in_unistr(s, len, &tpRedirectServerLen, &tpRedirectServerLen);
	}

	if (tpRedirectFlags & LB_TARGET_NETBIOS)
	{
		logger(Protocol, Warning, "process_redirect_pdu(), unhandled LB_TARGET_NETBIOS");
	}

	if (tpRedirectFlags & LB_TARGET_NET_ADDRESSES)
	{
		logger(Protocol, Warning,
		       "process_redirect_pdu(), unhandled LB_TARGET_NET_ADDRESSES");
	}

	if (tpRedirectFlags & LB_CLIENT_TSV_URL)
	{
		logger(Protocol, Warning, "process_redirect_pdu(), unhandled LB_CLIENT_TSV_URL");
	}

	if (tpRedirectFlags & LB_SERVER_TSV_CAPABLE)
	{
		logger(Protocol, Warning, "process_redirect_pdu(), unhandled LB_SERVER_TSV_URL");
	}

	if (tpRedirectFlags & LB_PASSWORD_IS_PK_ENCRYPTED)
	{
		logger(Protocol, Warning,
		       "process_redirect_pdu(), unhandled LB_PASSWORD_IS_PK_ENCRYPTED ");
	}

	if (tpRedirectFlags & LB_REDIRECTION_GUID)
	{
		logger(Protocol, Warning, "process_redirect_pdu(), unhandled LB_REDIRECTION_GUID ");
	}

	if (tpRedirectFlags & LB_TARGET_CERTIFICATE)
	{
		logger(Protocol, Warning,
		       "process_redirect_pdu(), unhandled LB_TARGET_CERTIFICATE");
	}

	return tpRedirect;
}

/* Process incoming packets */
void
rdp_main_loop(RD_BOOL * deactivated, uint32 * ext_disc_reason)
{
	do
	{
		if (rdp_loop(deactivated, ext_disc_reason) == False)
		{
			g_exit_mainloop = True;
		}
	}
	while (g_exit_mainloop == False);
}

/* used in uiports and rdp_main_loop, processes the RDP packets waiting */
RD_BOOL
rdp_loop(RD_BOOL * deactivated, uint32 * ext_disc_reason)
{
	uint8 type;
	RD_BOOL cont = True;
	STREAM s;

	while (g_exit_mainloop == False && cont)
	{
		s = rdp_recv(&type);
		if (s == NULL)
			return False;
		switch (type)
		{
			case RDP_PDU_DEMAND_ACTIVE:
				process_demand_active(s);
				*deactivated = False;
				break;
			case RDP_PDU_DEACTIVATE:
				logger(Protocol, Debug,
				       "rdp_loop(), RDP_PDU_DEACTIVATE packet received");
				*deactivated = True;
				g_wait_for_deactivate_ts = 0;
				break;
			case RDP_PDU_REDIRECT:
			case RDP_PDU_ENHANCED_REDIRECT:
				if (process_redirect_pdu(s, !(type == RDP_PDU_REDIRECT)) == True)
				{
					g_exit_mainloop = True;
					continue;
				}
				break;
			case RDP_PDU_DATA:
				/* If we got a data PDU, we don't need to keep the password in memory
				   anymore and therefor we should clear it for security reasons. */
				if (tpPassword[0] != '\0')
					memset(tpPassword, 0, sizeof(tpPassword));

				process_data_pdu(s, ext_disc_reason);
				break;
			default:
				logger(Protocol, Warning,
				       "rdp_loop(), unhandled PDU type %d received", type);
		}
		cont = g_next_packet < s_length(s);
	}
	return True;
}

/* Establish a connection up to the RDP layer */
RD_BOOL
rdp_connect(char *server, uint32 flags, char *domain, char *password,
	    char *command, char *directory, RD_BOOL reconnect)
{
	RD_BOOL deactivated = False;
	uint32 ext_disc_reason = 0;

	if (!sec_connect(server, tpUsername, domain, password, reconnect))
		return False;

	rdp_send_client_info_pdu(flags, domain, tpUsername, password, command, directory);

	/* run RDP loop until first licence demand active PDU */
	while (!g_rdp_shareid)
	{
		if (tpNetworkError)
			return False;

		if (!rdp_loop(&deactivated, &ext_disc_reason))
			return False;

		if (tpRedirect)
			return True;
	}
	return True;
}

/* Called during redirection to reset the state to support redirection */
void
rdp_reset_state(void)
{
	logger(Protocol, Debug, "%s()", __func__);
	g_next_packet = 0;	/* reset the packet information */
	g_rdp_shareid = 0;
	g_exit_mainloop = False;
	g_first_bitmap_caps = True;
	sec_reset_state();
}

/* Disconnect from the RDP layer */
void
rdp_disconnect(void)
{
	logger(Protocol, Debug, "%s()", __func__);
	sec_disconnect();
}

/* Abort rdesktop upon protocol error

   A protocol error is defined as:

    - A value is outside specified range for example;
      bpp for a bitmap is not allowed to be greater than the
      value 32 but is represented by a byte in protocol.

*/
void
_rdp_protocol_error(const char *file, int line, const char *func,
		    const char *message, STREAM s)
{
	logger(Protocol, Error, "%s:%d: %s(), %s", file, line, func, message);
	if (s)
		hexdump(s->data, s_length(s));
	exit(0);
}
