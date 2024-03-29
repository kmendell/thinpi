

#include "../tprdp.h"
#include "rdpsnd.h"
#include "rdpsnd_dsp.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ao/ao.h>
#include <sys/time.h>

#define WAVEOUTLEN	16

static ao_device *o_device = NULL;
static int default_driver;
static RD_BOOL reopened;
static char *libao_device = NULL;

void libao_play(void);

void
libao_add_fds(int *n, fd_set * rfds, fd_set * wfds, struct timeval *tv)
{
	UNUSED(n);
	UNUSED(rfds);
	UNUSED(tv);

	/* We need to be called rather often... */
	if (o_device != NULL && !rdpsnd_queue_empty())
		FD_SET(0, wfds);
}

void
libao_check_fds(fd_set * rfds, fd_set * wfds)
{
	UNUSED(rfds);
	UNUSED(wfds);

	if (o_device == NULL)
		return;

	if (!rdpsnd_queue_empty())
		libao_play();
}

RD_BOOL
libao_open(void)
{
	ao_sample_format format;

	ao_initialize();

	if (libao_device)
	{
		default_driver = ao_driver_id(libao_device);
	}
	else
	{
		default_driver = ao_default_driver_id();
	}

	memset(&format, 0, sizeof(format));
	format.bits = 16;
	format.channels = 2;
	format.rate = 44100;
	format.byte_format = AO_FMT_NATIVE;

	o_device = ao_open_live(default_driver, &format, NULL);
	if (o_device == NULL)
	{
		return False;
	}

	reopened = True;

	return True;
}

void
libao_close(void)
{
	/* Ack all remaining packets */
	while (!rdpsnd_queue_empty())
	{
		rdpsnd_queue_next(0);
	}

	if (o_device != NULL)
		ao_close(o_device);

	o_device = NULL;

	ao_shutdown();
}

RD_BOOL
libao_set_format(RD_WAVEFORMATEX * pwfx)
{
	ao_sample_format format;

	memset(&format, 0, sizeof(format));
	format.bits = pwfx->wBitsPerSample;
	format.channels = pwfx->nChannels;
	format.rate = 44100;
	format.byte_format = AO_FMT_NATIVE;

	if (o_device != NULL)
		ao_close(o_device);

	o_device = ao_open_live(default_driver, &format, NULL);
	if (o_device == NULL)
	{
		return False;
	}

	if (rdpsnd_dsp_resample_set(44100, pwfx->wBitsPerSample, pwfx->nChannels) == False)
	{
		return False;
	}

	reopened = True;

	return True;
}

void
libao_play(void)
{
	struct audio_packet *packet;
	STREAM out;
	unsigned char *data;
	int len;
	static long prev_s, prev_us;
	int duration;
	struct timeval tv;
	int next_tick;

	if (reopened)
	{
		reopened = False;
		gettimeofday(&tv, NULL);
		prev_s = tv.tv_sec;
		prev_us = tv.tv_usec;
	}

	/* We shouldn't be called if the queue is empty, but still */
	if (rdpsnd_queue_empty())
		return;

	packet = rdpsnd_queue_current_packet();
	out = packet->s;

	next_tick = rdpsnd_queue_next_tick();

	len = MIN(WAVEOUTLEN, s_remaining(out));
	in_uint8p(out, data, len);
	ao_play(o_device, (char *) data, len);

	gettimeofday(&tv, NULL);

	duration = ((tv.tv_sec - prev_s) * 1000000 + (tv.tv_usec - prev_us)) / 1000;

	if (packet->tick > next_tick)
		next_tick += 65536;

	if (s_check_end(out) || duration > next_tick - packet->tick + 500)
	{
		unsigned int delay_us;

		prev_s = tv.tv_sec;
		prev_us = tv.tv_usec;

		if (abs((next_tick - packet->tick) - duration) > 20)
		{
			logger(Sound, Debug,
			       "libao_play(), duration: %d, calc: %d, last: %d, is: %d, should: %d",
			       duration, next_tick - packet->tick,
			       (packet->tick + duration) % 65536, next_tick % 65536);

		}

		delay_us = ((out->size / 4) * (1000000 / 44100));

		rdpsnd_queue_next(delay_us);
	}
}

struct audio_driver *
libao_register(char *options)
{
	static struct audio_driver libao_driver;

	memset(&libao_driver, 0, sizeof(libao_driver));

	libao_driver.name = "libao";
	libao_driver.description = "libao output driver, default device: system dependent";

	libao_driver.add_fds = libao_add_fds;
	libao_driver.check_fds = libao_check_fds;

	libao_driver.wave_out_open = libao_open;
	libao_driver.wave_out_close = libao_close;
	libao_driver.wave_out_format_supported = rdpsnd_dsp_resample_supported;
	libao_driver.wave_out_set_format = libao_set_format;
	libao_driver.wave_out_volume = rdpsnd_dsp_softvol_set;

	libao_driver.need_byteswap_on_be = 1;
	libao_driver.need_resampling = 1;

	if (options)
	{
		libao_device = xstrdup(options);
	}

	return &libao_driver;
}
