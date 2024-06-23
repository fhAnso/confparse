/*
* confparse - small config file parser library, v1.0.6
*
* The confparse project facilitates the process of parsing a configuration 
* file into keys and their corresponding values. These pairs can be handled 
* individually, allowing for easy reading of configuration files. Currently, 
* this project supports up to 500 entries.
*
* Author: fhAnso
* License: MIT
* GitHub Repository: https://github.com/fhAnso/confparse
*/

#ifndef CONFPARSE_H
#define CONFPARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#define GENBUFF 512

struct settings 
{
	char *get_category;
	char *get_key;
	char *get_value;
};

typedef struct settings config_t;

config_t *config_init(const char *filename, int *count);
int config_validate(const char *filename, unsigned int verbose);
char *config_get_value(config_t *session, const char *category, const char *entry, int count);
void config_cleanup(config_t *storage, int count);

#endif
