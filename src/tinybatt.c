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
#include <getopt.h>  // getopt()
#include <math.h>  // ceil()
#include <dirent.h>  // scandir()
#include <stdbool.h>
#include "tinybatt.h"
#include "config.h"

char *program_name;
bool debug = false;  ///< debug flag

typedef struct battery_t
/* Represents a single battery. */
{
	int remaining_capacity;
	int last_capacity;
	int percentage;
	int rate;  ///< rate of discharge
	int charging;
	int discharging;
	int warning;
	char *status;  ///< OS-provided status string
	char output[255];  ///< will be included in program output
} battery_t;

battery_t *batteries[MAX_BATTERY_COUNT];
bool rate_output = false;  ///< controls display of discharge rate
int rate_threshold = 0;  ///< rate indicator will appear when value exceeded
char rate_abbrevs[MAX_ABBREVS_LIST_LEN] = RATE_ABBREVS;
char charging_string[MAX_STATUS_STRING_LEN] = CHARGING_STRING;
char discharging_string[MAX_STATUS_STRING_LEN] = DISCHARGING_STRING;
char warning_string[MAX_STATUS_STRING_LEN] = WARNING_STRING;
int warn_threshold = 10;  ///< threshold for warning indicator

void print_version()
/* Prints version information. */
{
	printf ("\n%s, version %s\n", program_name, VERSION);
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
	printf (" -v, --version    : print version\n");
	printf (" -h, --help       : this help\n");
	printf (" -r[0..n], --rate : display rate of discharge if it exceeds optional parameter\n");
	printf (" -w[0..n], --warn-threshold : percentage at which to display warning indicator\n");
	printf (" --charging="",\n");
	printf (" --discharging="",\n");
	printf (" --warning=""     : redefine status symbols. Assign empty string to disable.\n");
	printf (" --rate-abbrevs=\"a b c\" : sequential space-delimeted list of draw abbrevs.\n");
	printf ("                            Pass an empty list for digital draw representation.\n");
	printf (" --debug          : for developers only.\n");
	printf ("\n");
	printf ("for more details please visit: https://github.com/aidaho/tinybatt\n");
	printf ("(c) Sergey Frolov, 2015\n");
	printf ("License: GPLv3\n");
	printf ("\n");
}

int get_int_from_file(char *filepath)
/*
  Reads first string from file with provided filepath,
  attempts to interpret it as integer and return to caller.

  \param filepath full file path to read from
*/
{
	FILE *fp;
	int result;
	fp = fopen(filepath, "rt");
	fscanf(fp, "%d", &result);
	fclose(fp);
	if (debug)
		printf("%s\t%d\n", filepath, result);
	return result;
}

char * get_first_line_from_file(char *filepath)
/*
  Reads and returns first line from file in path.

  \param filepath full file path to read from
*/
{
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

char * squash_int_to_str(int n)
/* Converts integer to some (short and rounded) string representation. */
{
	char c = 'x', *result, *r_ptr, *a_ptr;
	int words = 0;
	result = (char *) malloc(MAX_ABBREV_LEN);
	a_ptr = rate_abbrevs;

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
	snprintf(result, MAX_ABBREV_LEN, "%d%s", n, RATE_DIVIDED_UNIT);
	return result;
}

int bat_dir_filter(const struct dirent *dirent)
/*
  Directory filter for scandir().

   Looks for directories which name starts with BATTERY_DIR_PREFIX.
 */
{
	size_t prefix_length;
	char prefix[] = BATTERY_DIR_PREFIX;

	prefix_length = strlen(prefix);  // length without trailing '\0'
	if (memcmp(&dirent->d_name, &prefix, prefix_length) == 0)
		return 1;
	return 0;
}

int prepare_battery_output(battery_t *bat)
/* Fills out output field in battery structure. */
{
	char output_part[OUTPUT_BUFFER_LEN] = "";

	/* Construct output */
	if (bat->discharging) {
		if (rate_output && bat->rate >= rate_threshold) {
			snprintf(output_part, OUTPUT_BUFFER_LEN, "%s ", squash_int_to_str(bat->rate));
			strcat(bat->output, output_part);
		}
		if (bat->warning) {
			strcat(bat->output, warning_string);
		} else {
			strcat(bat->output, discharging_string);
		}
	}
	if (bat->charging) {
		strcat(bat->output, charging_string);
	}
	snprintf(output_part, OUTPUT_BUFFER_LEN, "%d%%", bat->percentage);
	strcat(bat->output, output_part);
}

battery_t * process_battery(char *battpath)
/* Deals with supplied battery directory by filing out corresponding struct. */
{
	char *fpath;
	battery_t *bat = (battery_t *) malloc(sizeof(battery_t));

	asprintf(&fpath, "%s/%s", battpath, BATTERY_REMAINING_CAPACITY);
	bat->remaining_capacity = get_int_from_file(fpath) / CAPACITY_DIVIDER;

	asprintf(&fpath, "%s/%s", battpath, BATTERY_LAST_CAPACITY);
	bat->last_capacity = get_int_from_file(fpath) / CAPACITY_DIVIDER;

	/* I've tried a couple of userspace notifiers and it seems the general
	 consensus is to round upwards. I guess pulling the cord and watching
	 indicator goes from 100 to 99 twice as fast is frustrating. */
	bat->percentage = ceil(bat->remaining_capacity / (bat->last_capacity / 100));

	asprintf(&fpath, "%s/%s", battpath, BATTERY_STATUS);
	bat->status = get_first_line_from_file(fpath);

	asprintf(&fpath, "%s/%s", battpath, BATTERY_DISCHARGE_RATE);
	bat->rate = ceil(get_int_from_file(fpath) / RATE_DIVIDER);

	bat->charging = strstr(bat->status, CHARGING_STATUS) != NULL;
	bat->discharging = strstr(bat->status, DISCHARGING_STATUS) != NULL;
	bat->warning = bat->percentage <= warn_threshold;

	return bat;
}

int main (int argc, char *argv[])
{
	// Figure out program name:
	if ((program_name = strrchr(argv[0], '/')) == NULL)
		program_name = argv[0];
	else
		++program_name;  // move pointer to char after '/'

	char option;
	opterr = 0;

	while (true) {
		static struct option long_options[] =
			{
				{"version",        no_argument,       0, 'v'},
				{"help",           no_argument,       0, 'h'},
				{"rate",           optional_argument, 0, 'r'},
				{"warn-threshold", required_argument, 0, 'w'},
				{"debug",          no_argument,       0,   0},
				{"charging",       required_argument, 0,   0},
				{"discharging",    required_argument, 0,   0},
				{"warning",        required_argument, 0,   0},
				{"rate-abbrevs",   required_argument, 0,   0},
				{0, 0, 0, 0}
			};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		option = getopt_long(argc, argv, "hvw:r::",
							 long_options, &option_index);
		if (option == -1)
			break;  // end of options

		switch (option)
		{
		case 0:  // handles exclusively long options
			if (long_options[option_index].name == "debug") {
				debug = true;
				break;
			}
			if (long_options[option_index].name == "rate-abbrevs") {
				rate_output = true;
				snprintf(rate_abbrevs, MAX_ABBREVS_LIST_LEN, "%s", optarg);
				break;
			}
			if (long_options[option_index].name == "charging") {
				snprintf(charging_string, MAX_STATUS_STRING_LEN, "%s", optarg);
				break;
			}
			if (long_options[option_index].name == "discharging") {
				snprintf(discharging_string, MAX_STATUS_STRING_LEN, "%s", optarg);
				break;
			}
			if (long_options[option_index].name == "warning") {
				snprintf(warning_string, MAX_STATUS_STRING_LEN, "%s", optarg);
				break;
			}
			break;
		case 'v':
			print_version();
			exit(EXIT_SUCCESS);
		case 'h':
			print_help();
			exit(EXIT_SUCCESS);
		case 'r':
			rate_output = true;
			if (optarg)
				rate_threshold = strtol(optarg, NULL, 10);
			break;
		case 'w':
			warn_threshold = strtol(optarg, NULL, 10);
			break;
		case '?':
			fprintf(stderr, "%s: invalid option: %c\n\n", program_name, optopt);
			print_help();
			exit(EXIT_FAILURE);
		}
	}


	int dirnum;
	struct dirent **directories;  // pointer to array of pointers to dirent

	// We will walk through all battery directories in sysfs filling out structs:
	dirnum = scandir(BATTERY_PATH, &directories, bat_dir_filter, alphasort);
	if (dirnum > 0)
    {
		for (int i = 0; i < dirnum; i++) {
			char *battpath;
			asprintf(&battpath, "%s/%s", BATTERY_PATH, directories[i]->d_name);
			batteries[i] = process_battery(battpath);
			prepare_battery_output(batteries[i]);
		}
    }
	else {
		/* I assume, the user is quite aware of total battery absence.
		   Instead of polluting interface by error mesages I prefer to exit
		   quietly at this point. */
		if (debug)
			fprintf(stderr, "%s: no batteries found!\n", program_name);
		exit(EXIT_FAILURE);
	}


	char output[255] = "";
	for (int i = 0; batteries[i] != NULL; i++) {
		strcat(output, batteries[i]->output);
		if (batteries[i+1])
			strcat(output, " ");
	}

	printf("%s\n", output);
	exit(EXIT_SUCCESS);
}
