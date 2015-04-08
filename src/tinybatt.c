/*
  tinybatt: shows brief one-line battery information.
  Copyright (C) 2015 Sergey Frolov

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#define _GNU_SOURCE  // declare GNU extensions
#include <stdio.h>  // FILENAME_MAX
#include <stdlib.h>  // exit(), EXIT_SUCCESS
#include <string.h>  // strrchr()
#include <unistd.h>  // getopt()
#include <math.h>  // ceil()
#include "tinybatt.h"

char *program_name;
int debug = 0;  /**< debug flag */

void print_version()
/* Prints version information. */
{
	printf ("\n%s, version %s [-hvw] \n", program_name, VERSION);
	printf ("Copyright (C) 2015 Sergey Frolov\n");
	printf ("for more details please visit: https://github.com/aidaho/tinybatt\n");
	printf ("License: GPLv3: <http://www.gnu.org/licenses/gpl-3.0.html>\n");
	printf ("\n");
}

void print_help()
/* Prints help text. */
{
	printf ("\nusage: %s [-hvrd] \n", program_name);
	printf ("\n");
	printf ("%s shows brief one-line battery information.\n", program_name);
	printf ("The output is meant to be included in some text-based interface (like tmux)\n");
	printf ("\n");
	printf ("command line options:\n");
	printf (" -v : print version\n");
	printf (" -h : this help\n");
	printf (" -r[0..n] : display rate of discharge if it exceeds optional parameter\n");
	printf (" -d : debug\n");
	printf ("\n");
	printf ("for more details please visit: https://github.com/aidaho\n");
	printf ("(c) Sergey Frolov, 2015\n");
	printf ("License: GPLv3\n");
	printf ("\n");
}

int get_int_from_file(char *filepath) {
	/*
	  Reads first string from file with provided filepath,
	  attempts to interpret it as integer and return to caller.

	  \param filepath full file path to read from
	 */
	FILE *fp;
	int result;
	fp = fopen(filepath, "rt");
	fscanf(fp, "%d", &result);
	fclose(fp);
	if (debug)
		printf("%s\t%d\n", filepath, result);
	return result;
}

char * get_first_line_from_file(char *filepath) {
	/*
	  Reads and returns first line from file in path.

	  \param filepath full file path to read from
	*/
	size_t n = 0;
	FILE *fp;
	char *result = NULL;
	fp = fopen(filepath, "rt");
	getline(&result, &n, fp);
	fclose(fp);
	if (debug)
		printf("%s\t%s\n", filepath, result);
	return result;
}

char * squash_int_to_str(int n) {
	/* Converts integer to some (short and rounded) string representation. */
	char c = 'x', abbrevs[] = DRAW_ABBREVS, *result, *r_ptr, *a_ptr;
	int words = 0;
	result = (char *) malloc(MAX_ABBREV_LEN);
	a_ptr = abbrevs;

	while (words < n && c != '\0') {  // walk through abbrevs
		r_ptr = result;

		while ((c=*a_ptr++) != ' ' && c != '\0') {
			*r_ptr++ = c;  // copy abbrev char to result
		}
		*r_ptr = '\0';
		if (++words == n)  // is this an abbrev we need?
			return result;
	}

	// No corresponding abbrev found, fallback:
	sprintf(result, "%d", n);
	return result;
}

int main (int argc, char *argv[])
{
	// Figure out program name:
	if ((program_name = strrchr(argv[0], '/')) == NULL)
		program_name = argv[0];
	else
		++program_name;  // move pointer to char after '/'

	char option;
	int rate_output = 0, rate_threshold = 0;
	opterr = 0;
	while ((option = getopt(argc, argv, "hvdr::")) != EOF)
	{
		switch (option)
		{
		case 'v':
			print_version();
			exit(EXIT_SUCCESS);
		case 'h':
			print_help();
			exit(EXIT_SUCCESS);
		case 'd':
			debug = 1;
			break;
		case 'r':
			rate_output = 1;
			if (optarg)
				rate_threshold = strtol(optarg, NULL, 10);
			break;
		case '?':
			fprintf (stderr, "%s: invalid option: %c\n\n", program_name, optopt);
			print_help();
			exit(EXIT_FAILURE);
		}
	}

	char *fpath, *bat_status, output_part[32], output[255] = "";
	int remaining_capacity, last_capacity, percentage, rate, charging,
		discharging, warning;

	asprintf(&fpath, "%s/%s", BATTERY_PATH, BATTERY_REMAINING_CAPACITY);
	remaining_capacity = get_int_from_file(fpath) / CAPACITY_DIVIDER;

	asprintf(&fpath, "%s/%s", BATTERY_PATH, BATTERY_LAST_CAPACITY);
	last_capacity = get_int_from_file(fpath) / CAPACITY_DIVIDER;

	/* I've tried a couple of userspace notifiers and it seems the general
	 consensus is to round upwards. I guess pulling the cord and watching
	 indicator goes from 100 to 99 twice as fast is frustrating. */
	percentage = ceil(remaining_capacity / (last_capacity / 100));

	asprintf(&fpath, "%s/%s", BATTERY_PATH, BATTERY_STATUS);
	bat_status = get_first_line_from_file(fpath);

	asprintf(&fpath, "%s/%s", BATTERY_PATH, BATTERY_DISCHARGE_RATE);
	rate = ceil(get_int_from_file(fpath) / RATE_DIVIDER);

	charging = strstr(bat_status, CHARGING_STATUS) != NULL;
	discharging = strstr(bat_status, DISCHARGING_STATUS) != NULL;
	warning = percentage <= 10;


	/* Construct output */
	if (discharging) {
		if (rate_output && rate >= rate_threshold) {
			sprintf(output_part, "%s ", squash_int_to_str(rate));
			strcat(output, output_part);
		}
		if (warning) {
			strcat(output, WARNING_STRING);
		} else {
			strcat(output, DISCHARGING_STRING);
		}
	}
	if (charging) {
		strcat(output, CHARGING_STRING);
	}
	sprintf(output_part, "%d%%", percentage);
	strcat(output, output_part);

	printf("%s\n", output);
	exit(EXIT_SUCCESS);
}
