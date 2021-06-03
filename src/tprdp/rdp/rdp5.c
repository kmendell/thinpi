

#include "../tprdp.h"

extern size_t g_next_packet;

extern RDPCOMP g_mppc_dict;


static void
process_ts_fp_update_by_code(STREAM s, uint8 code)
{
	uint16 count, x, y;

	switch (code)
	{
		case FASTPATH_UPDATETYPE_ORDERS:
			in_uint16_le(s, count);
			process_orders(s, count);
			break;
		case FASTPATH_UPDATETYPE_BITMAP:
			in_uint8s(s, 2);	/* part length */
			process_bitmap_updates(s);
			break;
		case FASTPATH_UPDATETYPE_PALETTE:
			in_uint8s(s, 2);	/* uint16 = 2 */
			process_palette(s);
			break;
		case FASTPATH_UPDATETYPE_SYNCHRONIZE:
			break;
		case FASTPATH_UPDATETYPE_PTR_NULL:
			ui_set_null_cursor();
			break;
		case FASTPATH_UPDATETYPE_PTR_DEFAULT:
			set_system_pointer(SYSPTR_DEFAULT);
			break;
		case FASTPATH_UPDATETYPE_PTR_POSITION:
			in_uint16_le(s, x);
			in_uint16_le(s, y);
			ui_move_pointer(x, y);
			break;
		case FASTPATH_UPDATETYPE_COLOR:
			process_colour_pointer_pdu(s);
			break;
		case FASTPATH_UPDATETYPE_CACHED:
			process_cached_pointer_pdu(s);
			break;
		case FASTPATH_UPDATETYPE_POINTER:
			process_new_pointer_pdu(s);
			break;
		default:
			logger(Protocol, Warning,
			       "process_ts_fp_updates_by_code(), unhandled opcode %d", code);
	}
}

void
process_ts_fp_updates(STREAM s)
{
	uint16 length;
	uint8 hdr, code, frag, comp, ctype = 0;
	size_t next;

	uint8 *buf;
	uint32 roff, rlen;
	struct stream *ns = &(g_mppc_dict.ns);
	struct stream *ts;

	static STREAM assembled[16] = { 0 };

	ui_begin_update();
	while (!s_check_end(s))
	{
		/* Reading a number of TS_FP_UPDATE structures from the stream here.. */
		in_uint8(s, hdr);	/* updateHeader */
		code = hdr & 0x0F;	/*  |- updateCode */
		frag = hdr & 0x30;	/*  |- fragmentation */
		comp = hdr & 0xC0;	/*  `- compression */

		if (comp & FASTPATH_OUTPUT_COMPRESSION_USED)
			in_uint8(s, ctype);	/* compressionFlags */

		in_uint16_le(s, length);	/* length */

		g_next_packet = next = s_tell(s) + length;

		if (ctype & RDP_MPPC_COMPRESSED)
		{
			in_uint8p(s, buf, length);
			if (mppc_expand(buf, length, ctype, &roff, &rlen) == -1)
				logger(Protocol, Error,
				       "process_ts_fp_update_pdu(), error while decompressing packet");

			/* allocate memory and copy the uncompressed data into the temporary stream */
			s_realloc(ns, rlen);
			s_reset(ns);

			out_uint8a(ns, (unsigned char *) (g_mppc_dict.hist + roff), rlen);

			s_mark_end(ns);
			s_seek(ns, 0);
			s_push_layer(ns, rdp_hdr, 0);

			length = rlen;
			ts = ns;
		}
		else
			ts = s;

		if (frag == FASTPATH_FRAGMENT_SINGLE)
		{
			process_ts_fp_update_by_code(ts, code);
		}
		else		/* Fragmented packet, we must reassemble */
		{
			if (assembled[code] == NULL)
			{
				assembled[code] = s_alloc(RDESKTOP_FASTPATH_MULTIFRAGMENT_MAX_SIZE);
			}

			if (frag == FASTPATH_FRAGMENT_FIRST)
			{
				s_reset(assembled[code]);
			}

			out_uint8stream(assembled[code], ts, length);

			if (frag == FASTPATH_FRAGMENT_LAST)
			{
				s_mark_end(assembled[code]);
				s_seek(assembled[code], 0);
				process_ts_fp_update_by_code(assembled[code], code);
			}
		}

		s_seek(s, next);
	}
	ui_end_update();
}
