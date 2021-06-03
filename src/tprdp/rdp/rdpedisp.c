#include "../tprdp.h"

#define DISPLAYCONTROL_PDU_TYPE_CAPS 0x5
#define DISPLAYCONTROL_PDU_TYPE_MONITOR_LAYOUT 0x2

#define DISPLAYCONTROL_MONITOR_PRIMARY 0x1
#define RDPEDISP_CHANNEL_NAME "Microsoft::Windows::RDS::DisplayControl"

extern int tpDPI;
extern RD_BOOL tpPendingResizeDefer;
extern struct timeval tpPendingResizeDeferTimer;

static void rdpedisp_send(STREAM s);
static void rdpedisp_init_packet(STREAM s, uint32 type, uint32 length);

static void
rdpedisp_process_caps_pdu(STREAM s)
{
	uint32 tmp[3];

	in_uint32_le(s, tmp[0]);	/* MaxNumMonitors */
	in_uint32_le(s, tmp[1]);	/* MaxMonitorAreaFactorA */
	in_uint32_le(s, tmp[2]);	/* MaxMonitorAreaFactorB */

	logger(Protocol, Debug,
	       "rdpedisp_process_caps_pdu(), Max supported monitor area (square pixels) is %d",
	       tmp[0] * tmp[1] * tmp[2]);

	/* When the RDPEDISP channel is established, we allow dynamic
	   session resize straight away by clearing the defer flag and
	   the defer timer. This lets process_pending_resize() start
	   processing pending resizes immediately. We expect that
	   process_pending_resize will prefer RDPEDISP resizes over
	   disconnect/reconnect resizes. */
	tpPendingResizeDefer = False;
	tpPendingResizeDeferTimer.tv_sec = 0;
	tpPendingResizeDeferTimer.tv_usec = 0;
}

static void
rdpedisp_process_pdu(STREAM s)
{
	uint32 type;

	/* Read DISPLAYCONTROL_HEADER */
	in_uint32_le(s, type);	/* type */
	in_uint8s(s, 4);	/* length */

	logger(Protocol, Debug, "rdpedisp_process_pdu(), Got PDU type %d", type);

	switch (type)
	{
		case DISPLAYCONTROL_PDU_TYPE_CAPS:
			rdpedisp_process_caps_pdu(s);
			break;

		default:
			logger(Protocol, Warning, "rdpedisp_process_pdu(), Unhandled PDU type %d",
			       type);
			break;
	}
}

static void
rdpedisp_send_monitor_layout_pdu(uint32 width, uint32 height)
{
	struct stream s;
	uint32 physwidth, physheight, desktopscale, devicescale;

	memset(&s, 0, sizeof(s));

	logger(Protocol, Debug, "rdpedisp_send_monitor_layout_pdu(), width = %d, height = %d",
	       width, height);

	rdpedisp_init_packet(&s, DISPLAYCONTROL_PDU_TYPE_MONITOR_LAYOUT, 16 + 1 * 40);

	out_uint32_le(&s, 40);	/* MonitorLayoutSize - spec mandates 40 */
	out_uint32_le(&s, 1);	/* NumMonitors */

	out_uint32_le(&s, DISPLAYCONTROL_MONITOR_PRIMARY);	/* flags */
	out_uint32_le(&s, 0);	/* left */
	out_uint32_le(&s, 0);	/* top */
	out_uint32_le(&s, width);	/* width */
	out_uint32_le(&s, height);	/* height */

	utils_calculate_dpi_scale_factors(width, height, tpDPI,
					  &physwidth, &physheight, &desktopscale, &devicescale);

	out_uint32_le(&s, physwidth);	/* physicalwidth */
	out_uint32_le(&s, physheight);	/* physicalheight */
	out_uint32_le(&s, ORIENTATION_LANDSCAPE);	/* Orientation */
	out_uint32_le(&s, desktopscale);	/* DesktopScaleFactor */
	out_uint32_le(&s, devicescale);	/* DeviceScaleFactor */
	s_mark_end(&s);

	rdpedisp_send(&s);
}

static void
rdpedisp_init_packet(STREAM s, uint32 type, uint32 length)
{
	s_realloc(s, length);
	s_reset(s);

	out_uint32_le(s, type);
	out_uint32_le(s, length);
}

static void
rdpedisp_send(STREAM s)
{
	dvc_send(RDPEDISP_CHANNEL_NAME, s);
}

RD_BOOL
rdpedisp_is_available()
{
	return dvc_channels_is_available(RDPEDISP_CHANNEL_NAME);
}

void
rdpedisp_set_session_size(uint32 width, uint32 height)
{
	if (rdpedisp_is_available() == False)
		return;

	/* monitor width MUST be even number */
	utils_apply_session_size_limitations(&width, &height);

	rdpedisp_send_monitor_layout_pdu(width, height);
}

void
rdpedisp_init(void)
{
	dvc_channels_register(RDPEDISP_CHANNEL_NAME, rdpedisp_process_pdu);
}
