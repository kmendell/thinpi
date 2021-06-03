

/* Software volume control */
void rdpsnd_dsp_softvol_set(uint16 left, uint16 right);

/* Endian conversion */
void rdpsnd_dsp_swapbytes(unsigned char *buffer, unsigned int size, RD_WAVEFORMATEX * format);

/* Resample control */
RD_BOOL rdpsnd_dsp_resample_set(uint32 device_srate, uint16 device_bitspersample,
				uint16 device_channels);
RD_BOOL rdpsnd_dsp_resample_supported(RD_WAVEFORMATEX * pwfx);

STREAM rdpsnd_dsp_process(unsigned char *data, unsigned int size,
			  struct audio_driver *current_driver, RD_WAVEFORMATEX * format);
