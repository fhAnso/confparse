#include "confparse.h"

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

static int count_lines(const char *filename) 
{
    int counter = 0;
    char buff[GENBUFF];
    FILE *fp = fopen(filename, "r"); // Open stream for config file

    if (fp == NULL) 
	{
        fprintf(stderr, "configinit: Unable to open stream for %s\n", filename);
        exit(-1);
    }

    while (fgets(buff, GENBUFF, fp) != NULL) 
	{
		// Increment after every new line char and
		// skip empty lines and lines starting with '#'
        if (buff[strlen(buff) - 1] == '\n') buff[strlen(buff) - 1] = '\0';
        if (strlen(buff) == 0 || buff[0] == '#') continue;

        counter++;
    }

    fclose(fp);

    return counter;
}

static void remove_space(char *str) 
{
    char *src = str, *dest = str;

    while (*src != '\0') 
	{
        if (*src != ' ') 
		{
            // If current character is not a space, copy to destination
            *dest = *src;
            dest++;
        }

        src++;
    }

    // Null-terminate the resulting string
    *dest = '\0';
}

static void ignore_comment(char *str) 
{
    // Replace '#' with Null-terminator
    char *pos = strchr(str, '#');
    if (pos != NULL) *pos = '\0';
}

static config_t tokenize(char *line) 
{
    size_t buff_size = 128;
    char category[buff_size], key[buff_size], value[buff_size];
    char *delim = "=";

    memset(category, 0, buff_size);
    memset(key, 0, buff_size);
    memset(value, 0, buff_size);

    // Check if the line is a category header
    if (line[0] == '[' && strchr(line, ']') != NULL) 
	{
        // Extract category name
        strncpy(category, line + 1, strchr(line, ']') - line - 1);
        category[strchr(line, ']') - line - 1] = '\0';

        config_t init;
        init.get_category = strdup(category);
        init.get_key = NULL;
        init.get_value = NULL;

        return init;
    }

    // Extract key part from the line
    char *key_ptr = strtok(line, delim);
    
	if (key_ptr != NULL) 
	{
        strncpy(key, key_ptr, buff_size - 1);
        key[buff_size - 1] = '\0';
    }

    // Extract value part from the line
    char *val_ptr = strtok(NULL, delim);
    
	if (val_ptr != NULL) 
	{
        strncpy(value, val_ptr, buff_size - 1);
        value[buff_size - 1] = '\0';
    }

    remove_space(key);
    remove_space(value);

    ignore_comment(key);
    ignore_comment(value);

    int x = 0, y = 0;
    
	while (value[x]) 
	{
        // Remove quotes from value
        if (value[x] != '"') value[y++] = value[x];
        x++;
    }

    value[y] = '\0'; // Null-terminate modified value

    config_t init;
    
	init.get_category = NULL;
    init.get_key = strdup(key);
    init.get_value = strdup(value);

    return init;
}

static int file_exist(const char *filename) 
{
    return access(filename, F_OK);
}

static unsigned int is_supported_file(const char *filename) 
{
    // Define accepted file types
    const char *exts[] = {".txt", ".conf", ".ini", ".cfg"};
    const char *ext = strrchr(filename, '.');
    unsigned int supported = 0;

    if (ext != NULL) 
	{
        // Test accepted file types
        for (int iter = 0; iter < sizeof(exts) / sizeof(exts[0]); iter++) 
		{
            if (strcmp(ext, exts[iter]) == 0) 
			{
                // File type of given file is valid
                supported = 1;
                break;
            }
        }
    }

    return supported;
}

int config_validate(const char *filename, unsigned int verbose) 
{
    // Setup verbose mode
    if (verbose != 1 && verbose != 0) 
	{
        fprintf(stderr, "config_validate: Unknown verbose value set\n");
        return -1;
    } 
	else if (verbose == 1) 
    {
        printf("Running validation checks for %s..", filename);
    }

    // Validate file
    if (file_exist(filename) != 0) 
	{
        if (verbose == 1)
        { 
            fprintf(stderr, "config_validate: File %s does not exist\n", filename);
        }

        return -1;
    }

    if (is_supported_file(filename) == 0) 
	{
        if (verbose == 1)
        {
            fprintf(stderr, "\nconfig_validate: File %s cannot be handled\n", filename);
        }

	    return -1;
    }

    // Open file stream after successful validation
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) 
	{
        if (verbose == 1)
        {
            fprintf(stderr, "\nconfig_validate: Unable to open stream for %s\n", filename);
        }

        return -1;
    }

    size_t test_buff = GENBUFF + 2;
    char *buff = NULL;
    size_t err = 0;

    while (getline(&buff, &test_buff, fp) != -1) 
	{
        size_t line_length = strlen(buff);
    
        // Remove the newline character if it exists
	    if (line_length > 0 && buff[line_length - 1] == '\n') line_length--;
        if (line_length > GENBUFF) // Check if the line length exceeds the maximum buffer
        {
            if (verbose == 1) 
                printf("\nconfig_validate: This line is too long: %.20s..\n", buff);
            
            err++;
            break;
        }

        char *temp = buff;
        
		while (*temp) 
		{
            // Check for invalid characters in the line
            if (*temp == '\'' 
				|| (*temp == '/' && *(temp + 1) == '/') 
				|| (*temp == '/' && *(temp + 1) == '*')) 
			{
                if (verbose == 1) 
                    printf("\nconfig_validate: Only '#' is allowed to mark a comment\n");
                
                err++;
                break;
            } 
			else 
                temp++;
        }
    }

    if (err == 1) return 1;

    fclose(fp);
    free(buff);

    // Currently 500 entries are allowed
    if (count_lines(filename) > GENBUFF) 
	{
        if (verbose == 1) 
            printf("\nconfig_validate: Too many entries!\n");

        return -1;
    }

    if (verbose == 1) printf("PASSED\n");

    return 0;
}

char *config_get_value(config_t *session, const char *category, const char *entry, int count) 
{
    char *value = NULL;
    int hit = 0;

    for (int iter = 0; iter < count; iter++) 
	{
        // Make sure the value will be extracted from the correct category
        if (category != NULL 
			&& session[iter].get_category != NULL 
			&& strcmp(session[iter].get_category, category) != 0)
            continue;

        // Get the value of the specific key
        if (session[iter].get_key != NULL && strcmp(session[iter].get_key, entry) == 0) 
		{
            value = session[iter].get_value;
            hit++;
            break;
        }
    }

    if (hit == 0) return NULL;

    return value;
}

config_t *config_init(const char *filename, int *count) 
{
    if (file_exist(filename) != 0) 
	{
        fprintf(stderr, "config_init: File %s does not exist\n", filename);
        exit(-1);
    }

    if (is_supported_file(filename) == 0) 
	{
        fprintf(stderr, "config_init: Filetype not supported: %s\n", filename);
        exit(-1);
    }

    if (count_lines(filename) > GENBUFF) 
	{
        fprintf(stderr, "config_init: File %s is too large\n", filename);
        exit(-1);
    }

    FILE *fp = fopen(filename, "r");
    
	if (fp == NULL) {
        fprintf(stderr, "configinit: Unable to open stream for %s\n", filename);
        exit(-1);
    }

    // Allocate memory for storing configuration entries
    config_t *storage = (config_t *)malloc(sizeof(config_t) * GENBUFF);
    
	if (storage == NULL) 
	{
        fprintf(stderr, "configinit: malloc: %s\n", strerror(errno));
        exit(-1);
    }

    int storage_count = 0; // Counter for the number of stored configurations
    char buff[GENBUFF]; // Buffer to read each line from the file

    while (fgets(buff, GENBUFF, fp) != NULL) 
	{
        // Skip lines that are empty or start with a comment character
        if (isspace((unsigned char)*buff) || buff[0] == '#') continue;

        // Ensure buff is properly null-terminated
        buff[strcspn(buff, "\n")] = '\0';
        // Tokenize the line and store the result in the storage array
        storage[storage_count] = tokenize(buff);
        
        storage_count++;
    }

    fclose(fp);
    
    // Set the count to the number of stored configurations
	*count = storage_count;

    return storage;
}

void config_cleanup(config_t *storage, int count) 
{
    for (int idx = 0; idx < count; idx++) 
	{
        free(storage[idx].get_category);
        free(storage[idx].get_key);
        free(storage[idx].get_value);
    }
 
    free(storage);
}