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

#ifndef CONFWRITE_H
#define CONFWRITE_H

#include "confparse.h"

#include <stdio.h>
#include <string.h>
#include <string.h>

FILE *configwrite_init(char *filename);
int config_set_value(FILE *fp, const char *category, 
						const char *key, const char *value);

void config_build_config(const char *filename);
void configwrite_cleanup(FILE *fp);

#endif
