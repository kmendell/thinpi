#ifndef _utils_h
#define _utils_h

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
  
#include "../include/types.h"

uint32 utils_djb2_hash(const char *str);
char *utils_string_escape(const char *str);
char *utils_string_unescape(const char *str);
int utils_locale_to_utf8(const char *src, size_t is, char *dest, size_t os);
int utils_mkdir_safe(const char *path, int mask);
int utils_mkdir_p(const char *path, int mask);
void utils_calculate_dpi_scale_factors(uint32 width, uint32 height, uint32 dpi,
									   uint32 *physwidth, uint32 *physheight,
									   uint32 *desktopscale, uint32 *devicescale);
void utils_apply_session_size_limitations(uint32 *width, uint32 *height);

const char *util_dialog_choice(const char *message, ...);

int utils_cert_handle_exception(gnutls_session_t session, unsigned int status,
								RD_BOOL hostname_mismatch, const char *hostname);

typedef enum log_level_t
{
	Debug = 0,
	Verbose,
	Warning,
	Error,
	Notice /* special message level for end user messages with prefix */
} log_level_t;

typedef enum log_subject_t
{
	GUI = 0,
	Keyboard,
	Clipboard,
	Sound,
	Protocol,
	Graphics,
	Core,
	SmartCard,
	Disk
} log_subject_t;

void logger(log_subject_t c, log_level_t lvl, char *format, ...);
void logger_set_verbose(int verbose);
void logger_set_subjects(char *subjects);

#endif /* _utils_h */
