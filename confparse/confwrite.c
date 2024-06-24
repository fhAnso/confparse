/*
 * confparse - small config file parser library, v1.0.7
 *
 * The confparse project facilitates the process of parsing a configuration
 * file into keys and their corresponding values. These pairs can be handled
 * individually, allowing for easy reading and manipulating of configuration files. Currently,
 * this project supports up to 500 entries.
 *
 * Author: fhAnso
 * License: MIT
 * GitHub Repository: https://github.com/fhAnso/confparse
 */

#include "confwrite.h"

FILE *configwrite_init(const char *filename)
{
	if (file_exist(filename) != 0)
	{
		fprintf(stderr, "configwrite_init: File %s does not exist\n", filename);
		exit(-1);
	}

	if (is_supported_file(filename) == 0)
	{
		fprintf(stderr, "configwrite_init: Filetype not supported: %s\n", filename);
		exit(-1);
	}

	if (count_lines(filename) > GENBUFF)
	{
		fprintf(stderr, "configwrite_init: File %s is too large\n", filename);
		exit(-1);
	}

	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		fprintf(stderr, "configwrite_init: Unable to open stream for %s\n", filename);
		exit(-1);
	}

    return fp;
}