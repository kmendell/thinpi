

#include "../tprdp.h"
#include <sys/types.h>
#include <unistd.h>

static VCHANNEL *lspci_channel;

typedef struct _pci_device
{
	uint16 klass;
	uint16 vendor;
	uint16 device;
	uint16 subvendor;
	uint16 subdevice;
	uint8 revision;
	uint8 progif;
} pci_device;

static pci_device current_device;

static void lspci_send(const char *output);

/* Handle one line of output from the lspci subprocess */
static RD_BOOL
handle_child_line(const char *line, void *data)
{
	UNUSED(data);
	const char *val;
	char buf[1024];

	if (str_startswith(line, "Class:"))
	{
		val = line + sizeof("Class:");
		/* Skip whitespace and second Class: occurrence */
		val += strspn(val, " \t") + sizeof("Class");
		current_device.klass = strtol(val, NULL, 16);
	}
	else if (str_startswith(line, "Vendor:"))
	{
		val = line + sizeof("Vendor:");
		current_device.vendor = strtol(val, NULL, 16);
	}
	else if (str_startswith(line, "Device:"))
	{
		val = line + sizeof("Device:");
		/* Sigh, there are *two* lines tagged as Device:. We
		   are not interested in the domain/bus/slot/func */
		if (!strchr(val, ':'))
			current_device.device = strtol(val, NULL, 16);
	}
	else if (str_startswith(line, "SVendor:"))
	{
		val = line + sizeof("SVendor:");
		current_device.subvendor = strtol(val, NULL, 16);
	}
	else if (str_startswith(line, "SDevice:"))
	{
		val = line + sizeof("SDevice:");
		current_device.subdevice = strtol(val, NULL, 16);
	}
	else if (str_startswith(line, "Rev:"))
	{
		val = line + sizeof("Rev:");
		current_device.revision = strtol(val, NULL, 16);
	}
	else if (str_startswith(line, "ProgIf:"))
	{
		val = line + sizeof("ProgIf:");
		current_device.progif = strtol(val, NULL, 16);
	}
	else if (strspn(line, " \t") == strlen(line))
	{
		/* Blank line. Send collected information over channel */
		snprintf(buf, sizeof(buf), "%04x,%04x,%04x,%04x,%04x,%02x,%02x\n",
				 current_device.klass, current_device.vendor,
				 current_device.device, current_device.subvendor,
				 current_device.subdevice, current_device.revision, current_device.progif);
		lspci_send(buf);
		memset(&current_device, 0, sizeof(current_device));
	}
	else
	{
		logger(Core, Warning, "handle_child_line(), Unrecognized lspci line '%s'", line);
	}
	return True;
}

/* Process one line of input from virtual channel */
static RD_BOOL
lspci_process_line(const char *line, void *data)
{
	UNUSED(data);
	char *lspci_command[5] = {"lspci", "-m", "-n", "-v", NULL};

	if (!strcmp(line, "LSPCI"))
	{
		memset(&current_device, 0, sizeof(current_device));
		subprocess(lspci_command, handle_child_line, NULL);
		/* Send single dot to indicate end of enumeration */
		lspci_send(".\n");
	}
	else
	{
		logger(Core, Error, "lspci_process_line(), invalid line '%s'", line);
	}
	return True;
}

/* Process new data from the virtual channel */
static void
lspci_process(STREAM s)
{
	unsigned int pkglen;
	static char *rest = NULL;
	char *buf;

	pkglen = s_remaining(s);
	/* str_handle_lines requires null terminated strings */
	buf = xmalloc(pkglen + 1);
	in_uint8a(s, buf, pkglen);
	buf[pkglen] = '\0';
	str_handle_lines(buf, &rest, lspci_process_line, NULL);
	xfree(buf);
}

/* Initialize this module: Register the lspci channel */
RD_BOOL
lspci_init(void)
{
	lspci_channel =
		channel_register("lspci", CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,
						 lspci_process);
	return (lspci_channel != NULL);
}

/* Send data to channel */
static void
lspci_send(const char *output)
{
	STREAM s;
	size_t len;

	len = strlen(output);
	s = channel_init(lspci_channel, len);
	out_uint8a(s, output, len) s_mark_end(s);
	channel_send(s, lspci_channel);
	s_free(s);
}
