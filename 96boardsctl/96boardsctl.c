#include <err.h>
#include <ftdi.h>
#include <getopt.h>
#include <usb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

static const char *progname;
static struct ftdi_context ftdi;
static char cbus_shadow = 0;

static int power_pin = 2;
static int reset_pin = 3;
static unsigned long pulse_ms = 1000;

static const char *optstring = "-hlLop:s:";
static const struct option long_options[] = {
	{ "serial", 1, NULL, 's' },
	{ "help", 0, NULL, 'h' },
	{ "list", 0, NULL, 'l' },
	{ "old", 0, NULL, 'o' },
	{ "pulse-width", 1, NULL, 'p' },
	{ NULL, 0, NULL, 0 },
};

static void usage(void)
{
	fprintf(stderr,
		"Usage: %s [OPTION]... command\n"
		"Control the power button and reset lines with a 96boards USB console adaptor\n"
		"\n"
		"  -h, --help			Display this help and exit\n"
		"  -o, --old			Using old v0.3 prototype board\n"
		"  -l, --list			List available devices and exit\n"
		"  -L, --long			Use long 5s pulse, useful to force board to power off\n"
		"  -p, --pulse-width=TIME	Length of signal pulse in ms\n"
		"  -s, --serial <serial>	Specify device to open by serial number\n"
		"\n"
		"Commands:\n"
		"  power			Pulse the power button signal\n"
		"  reset			Pulse the reset button signal\n"
		"\n"
		"This program causes the kernel's ftdi_sio driver to disconnect from the UART device\n"
		"It can be reconnected by echoing the device name into /sys/bus/usb/drivers/ftdi_sio/bind\n"
		"", progname);
}

void list_devices(void)
{
	struct ftdi_device_list *list, *cur;
	char manufacturer[128], description[128], serial[128];
	int rc;

	rc = ftdi_usb_find_all(&ftdi, &list, 0x0403, 0x6015);
	if (rc < 1)
		err(3, "Cannot find any devices");

	for (cur = list; cur; cur = cur->next) {
		rc = ftdi_usb_get_strings(&ftdi, cur->dev, manufacturer, 128,
					  description, 128, serial, 128);
		if (rc < 0)
			err(4, "Cannot read device configuration");
		printf("%10s mfg='%s' description='%s'\n",
			serial, manufacturer, description);
	}

	ftdi_list_free(&list);
	exit(EXIT_SUCCESS);
}

int set_cbus(int pin, int direction, int val)
{
	cbus_shadow &= ~(0x11 << pin);
	cbus_shadow |= ((direction ? 0x10 : 0) | (val ? 0x01 : 0)) << pin;
	return ftdi_set_bitmode(&ftdi, cbus_shadow, BITMODE_CBUS);
}

int pulse_cbus(int pin, int usecs)
{
	int rc;

	/* Pulse pin by setting as output low, and then returning to input */
	rc = set_cbus(pin, 1, 0);
	if (rc)
		return rc;
	usleep(usecs);
	return set_cbus(pin, 0, 0);
}

int main(int argc, char *argv[])
{
	char *cmd = NULL, *serialnum = NULL;
	int rc;
	char c;

	progname = argv[0];

	ftdi_init(&ftdi);

	while ((c = getopt_long(argc, argv, optstring, long_options, NULL)) >= 0) {
		switch (c) {
		case 1:
			cmd = optarg;
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'l':
			list_devices();
			break;
		case 'L':
			pulse_ms = 1000;
			break;
		case 'o':
			power_pin = 1;
			reset_pin = 0;
			break;
		case 'p':
			pulse_ms = strtoul(optarg, NULL, 10);
			break;
		case 's':
			serialnum = optarg;
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}

	if (!cmd) {
		usage();
		exit(EXIT_FAILURE);
	}

	if (serialnum) {
		char devname[60];
		snprintf(devname, 60, "s:0x0403:0x6015:%s", serialnum);
		rc = ftdi_usb_open_string(&ftdi, devname);
	} else {
		rc = ftdi_usb_open(&ftdi, 0x0403, 0x6015);
	}
	if (rc < 0) {
		fprintf(stderr, "Unable to open ftdi device: %d (%s)\n",
			rc, ftdi_get_error_string(&ftdi));
		exit(EXIT_FAILURE);
	}

	if (strcmp(cmd, "reset") == 0) {
		printf("Pressing reset\n");
		pulse_cbus(reset_pin, pulse_ms * 1000);
	} else if (strcmp(cmd, "power") == 0) {
		printf("Pressing power\n");
		pulse_cbus(power_pin, pulse_ms * 1000);
	} else {
		fprintf(stderr, "Unknown command '%s'\n", cmd);
		usage();
		exit(EXIT_FAILURE);
	}

	ftdi_disable_bitbang(&ftdi);
	ftdi_usb_close(&ftdi);
	ftdi_deinit(&ftdi);
	exit(EXIT_SUCCESS);
}
