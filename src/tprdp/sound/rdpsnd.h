

struct audio_packet
{
	STREAM s;
	uint16 tick;
	uint8 index;

	struct timeval arrive_tv;
	struct timeval completion_tv;
};

struct audio_driver
{
	void (*add_fds) (int *n, fd_set * rfds, fd_set * wfds, struct timeval * tv);
	void (*check_fds) (fd_set * rfds, fd_set * wfds);

	  RD_BOOL(*wave_out_open) (void);
	void (*wave_out_close) (void);
	  RD_BOOL(*wave_out_format_supported) (RD_WAVEFORMATEX * pwfx);
	  RD_BOOL(*wave_out_set_format) (RD_WAVEFORMATEX * pwfx);
	void (*wave_out_volume) (uint16 left, uint16 right);

	  RD_BOOL(*wave_in_open) (void);
	void (*wave_in_close) (void);
	  RD_BOOL(*wave_in_format_supported) (RD_WAVEFORMATEX * pwfx);
	  RD_BOOL(*wave_in_set_format) (RD_WAVEFORMATEX * pwfx);
	void (*wave_in_volume) (uint16 left, uint16 right);

	char *name;
	char *description;
	int need_byteswap_on_be;
	int need_resampling;
	struct audio_driver *next;
};

/* Driver register functions */
struct audio_driver *pulse_register(char *options);
struct audio_driver *alsa_register(char *options);
struct audio_driver *libao_register(char *options);
struct audio_driver *oss_register(char *options);
struct audio_driver *sgi_register(char *options);
struct audio_driver *sun_register(char *options);
