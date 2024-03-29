

#include "../tprdp.h"
#include "rdpsnd.h"
#include <errno.h>
#include <dmedia/audio.h>
#include <unistd.h>

/* #define IRIX_DEBUG 1 */

#define IRIX_MAX_VOL     65535

ALconfig audioconfig;
ALport output_port;

static int g_snd_rate;
static int width = AL_SAMPLE_16;
static char *sgi_output_device = NULL;

double min_volume, max_volume, volume_range;
int resource, maxFillable;
int combinedFrameSize;

void sgi_play(void);

void
sgi_add_fds(int *n, fd_set * rfds, fd_set * wfds, struct timeval *tv)
{
	/* We need to be called rather often... */
	if (output_port != (ALport) 0 && !rdpsnd_queue_empty())
		FD_SET(0, wfds);
}

void
sgi_check_fds(fd_set * rfds, fd_set * wfds)
{
	if (output_port == (ALport) 0)
		return;

	if (!rdpsnd_queue_empty())
		sgi_play();
}

RD_BOOL
sgi_open(void)
{
	ALparamInfo pinfo;
	static int warned = 0;

#if (defined(IRIX_DEBUG))
	logger(Sound, Debug, "sgi_open()");
#endif

	if (!warned && sgi_output_device)
	{
		logger(Sound, Warning, "sgi_open(), device-options not supported for libao-driver");
		warned = 1;
	}

	if (alGetParamInfo(AL_DEFAULT_OUTPUT, AL_GAIN, &pinfo) < 0)
	{
		logger(Sound, Error, "sgi_open(), alGetParamInfo failed: %s",
		       alGetErrorString(oserror()));
	}
	min_volume = alFixedToDouble(pinfo.min.ll);
	max_volume = alFixedToDouble(pinfo.max.ll);
	volume_range = (max_volume - min_volume);

	logger(Sound, Debug, "sgi_open(), minvol = %lf, maxvol= %lf, range = %lf",
	       min_volume, max_volume, volume_range);


	audioconfig = alNewConfig();
	if (audioconfig == (ALconfig) 0)
	{
		logger(Sound, Error, "sgi_open(), alNewConfig failed: %s",
		       alGetErrorString(oserror()));
		return False;
	}

	output_port = alOpenPort("rdpsnd", "w", 0);
	if (output_port == (ALport) 0)
	{
		logger(Sound, Error, "sgi_open(), alOpenPort failed: %s",
		       alGetErrorString(oserror()));
		return False;
	}

	logger(Sound, Debug, "sgi_open(), done");
	return True;
}

void
sgi_close(void)
{
	/* Ack all remaining packets */
	logger(Sound, Debug, "sgi_close()");

	while (!rdpsnd_queue_empty())
		rdpsnd_queue_next(0);
	alDiscardFrames(output_port, 0);

	alClosePort(output_port);
	output_port = (ALport) 0;
	alFreeConfig(audioconfig);

	logger(Sound, Debug, "sgi_close(), done");
}

RD_BOOL
sgi_format_supported(RD_WAVEFORMATEX * pwfx)
{
	if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
		return False;
	if ((pwfx->nChannels != 1) && (pwfx->nChannels != 2))
		return False;
	if ((pwfx->wBitsPerSample != 8) && (pwfx->wBitsPerSample != 16))
		return False;

	return True;
}

RD_BOOL
sgi_set_format(RD_WAVEFORMATEX * pwfx)
{
	int channels;
	int frameSize, channelCount;
	ALpv params;

	logger(Sound, Debug, "sgi_set_format()");

	if (pwfx->wBitsPerSample == 8)
		width = AL_SAMPLE_8;
	else if (pwfx->wBitsPerSample == 16)
		width = AL_SAMPLE_16;

	/* Limited support to configure an opened audio port in IRIX.  The
	   number of channels is a static setting and can not be changed after
	   a port is opened.  So if the number of channels remains the same, we
	   can configure other settings; otherwise we have to reopen the audio
	   port, using same config. */

	channels = pwfx->nChannels;
	g_snd_rate = pwfx->nSamplesPerSec;

	alSetSampFmt(audioconfig, AL_SAMPFMT_TWOSCOMP);
	alSetWidth(audioconfig, width);
	if (channels != alGetChannels(audioconfig))
	{
		alClosePort(output_port);
		alSetChannels(audioconfig, channels);
		output_port = alOpenPort("rdpsnd", "w", audioconfig);

		if (output_port == (ALport) 0)
		{
			logger(Sound, Error, "sgi_set_format(), alOpenPort failed: %s",
			       alGetErrorString(oserror()));
			return False;
		}

	}

	resource = alGetResource(output_port);
	maxFillable = alGetFillable(output_port);
	channelCount = alGetChannels(audioconfig);
	frameSize = alGetWidth(audioconfig);

	if (frameSize == 0 || channelCount == 0)
	{
		logger(Sound, Error, "sgi_set_format(), bad frameSize or channelCount");
		return False;
	}
	combinedFrameSize = frameSize * channelCount;

	params.param = AL_RATE;
	params.value.ll = (long long) g_snd_rate << 32;

	if (alSetParams(resource, &params, 1) < 0)
	{
		logger(Sound, Error, "sgi_set_format(), alSetParams failed: %s",
		       alGetErrorString(oserror()));
		return False;
	}
	if (params.sizeOut < 0)
	{
		logger(Sound, Error, "sgi_set_format(), invalid rate %d", g_snd_rate);
		return False;
	}

	logger(Sound, Debug, "sgi_set_format(), done");

	return True;
}

void
sgi_volume(uint16 left, uint16 right)
{
	double gainleft, gainright;
	ALpv pv[1];
	ALfixed gain[8];


	logger(Sound, Debug, "sgi_volume(), left=%d, right=%d", left, right);

	gainleft = (double) left / IRIX_MAX_VOL;
	gainright = (double) right / IRIX_MAX_VOL;

	gain[0] = alDoubleToFixed(min_volume + gainleft * volume_range);
	gain[1] = alDoubleToFixed(min_volume + gainright * volume_range);

	pv[0].param = AL_GAIN;
	pv[0].value.ptr = gain;
	pv[0].sizeIn = 8;
	if (alSetParams(AL_DEFAULT_OUTPUT, pv, 1) < 0)
	{
		logger(Sound, Error, "sgi_volume(), alSetParams failed: %s",
		       alGetErrorString(oserror()));
		return;
	}

	logger(Sound, Debug, "sgi_volume(), done");
}

void
sgi_play(void)
{
	struct audio_packet *packet;
	ssize_t len;
	STREAM out;
	unsigned char *data;
	int gf;

	while (1)
	{
		if (rdpsnd_queue_empty())
			return;

		packet = rdpsnd_queue_current_packet();
		out = packet->s;

		len = s_remaining(out);
		in_uint8p(out, data, len);

		alWriteFrames(output_port, data, len / combinedFrameSize);

		if (s_check_end(out))
		{
			gf = alGetFilled(output_port);
			if (gf < (4 * maxFillable / 10))
			{
				rdpsnd_queue_next(0);
			}
			else
			{
/*  				logger(Sound,Debug, "sgi_play(), busy playing..."); */
				usleep(10);
				return;
			}
		}
	}
}

struct audio_driver *
sgi_register(char *options)
{
	static struct audio_driver sgi_driver;

	memset(&sgi_driver, 0, sizeof(sgi_driver));

	sgi_driver.name = "sgi";
	sgi_driver.description = "SGI output driver";

	sgi_driver.add_fds = sgi_add_fds;
	sgi_driver.check_fds = sgi_check_fds;

	sgi_driver.wave_out_open = sgi_open;
	sgi_driver.wave_out_close = sgi_close;
	sgi_driver.wave_out_format_supported = sgi_format_supported;
	sgi_driver.wave_out_set_format = sgi_set_format;
	sgi_driver.wave_out_volume = sgi_volume;

	sgi_driver.need_byteswap_on_be = 1;
	sgi_driver.need_resampling = 0;

	if (options)
	{
		sgi_output_device = xstrdup(options);
	}
	return &sgi_driver;
}
