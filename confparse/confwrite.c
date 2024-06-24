/*
 * confparse - small config file parser library, v1.0.7
 *
 * The confparse project facilitates the process of parsing a configuration
 * file into keys and their corresponding values. These pairs can be handled
 * individually, allowing for easy reading and manipulating of configuration
 * files. Currently, this project supports up to 500 entries.
 *
 * Author: fhAnso
 * License: MIT
 * GitHub Repository: https://github.com/fhAnso/confparse
 */

#include "confwrite.h"

char *config_filename = NULL;

FILE *configwrite_init(char *filename)
{
	if (file_exist(filename) != 0)
	{
		fprintf(stderr, "configwrite_init: File %s does not exist\n", filename);
		exit(-1);
	}

	if (is_supported_file(filename) == 0)
	{
		fprintf(stderr, "configwrite_init: Filetype not supported: %s\n",
				filename);
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
		fprintf(stderr, "configwrite_init: Unable to open stream for %s\n",
				filename);
		return NULL;
	}

	config_filename = filename;

	return fp;
}

static void get_category(const char *value, char start, char end,
						 char *category_buffer)
{
	const char *s = strchr(value, start);
	const char *e = strchr(value, end);

	// Extract category from given line (value)
	if (s != NULL && e != NULL && s < e)
	{
		s++;
		size_t len = e - s;

		strncpy(category_buffer, s, len);
		category_buffer[len] = '\0';
	}
	else
		category_buffer[0] = '\0';
}

int config_set_value(FILE *fp, const char *category, const char *key,
					 const char *value)
{
	const char *temp_filename = "tmp.txt";
	size_t test_buff = GENBUFF + 2; // size of input buffer (getline)
	size_t buff_size = GENBUFF / 3; // category buffer size
	char *buff = NULL;
	char current_category[buff_size];
	int category_match = 0;
	int key_found = 0;
	FILE *temp_fp = fopen(temp_filename, "a+");

	if (temp_fp == NULL)
	{
		fprintf(stderr, "config_set_value: fopen: %s\n", strerror(errno));
		return -1;
	}

	memset(current_category, 0, buff_size);

	while (getline(&buff, &test_buff, fp) != -1)
	{
		size_t line_length = strlen(buff);

		// Remove the newline character of current line if it exists
		if (line_length > 0 && buff[line_length - 1] == '\n')
			buff[line_length - 1] = '\0';

		get_category(buff, '[', ']', current_category);

		// Check if the current line matches the specified category
		if (strlen(current_category) != 0)
		{
			if (strcmp(current_category, category) == 0)
				category_match = 1;
			else
				category_match = 0;
		}

		if (category_match == 1 && strstr(buff, key) != NULL &&
			strchr(buff, '=') != NULL)
		{
			char *pos = strchr(buff, '=');

			if (pos != NULL)
			{
				size_t key_len = strlen(key);

				// Update entry: write to temp file
				if (strncmp(buff, key, key_len) == 0 &&
					(buff[key_len] == ' ' || buff[key_len] == '='))
				{
					fprintf(temp_fp, "%s = %s\n", key, value);
					key_found = 1;
					continue;
				}
			}
		}

		// No match, write line unchanged to temp file
		fprintf(temp_fp, "%s\n", buff);
	}

	if (key_found == 0)
	{
		remove(temp_filename);
		return -1;
	}

	free(buff);
	fclose(temp_fp);

	// Temp file will be renamed to main config file
	rename(temp_filename, config_filename);

	return 0;
}

void configwrite_cleanup(FILE *fp) { fclose(fp); }