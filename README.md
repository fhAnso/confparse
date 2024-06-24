# confparse - Small configuration file parsing library

The confparse project facilitates the process of parsing a configuration file into keys and their corresponding values. These pairs can be handled individually, allowing for easy reading of configuration files. Currently, this project supports up to 500 entries.

### Usage

- To use confparse, copy the confparse directory to your project. 
```C
#include "confparse/confparse.h"
```

- Function overview
```C
/* --- confparse */
// Validate config file
int config_validate(const char *filename, unsigned int verbose);

// Load the config file and initialize entry pool
config_t *configparse_init(const char *filename, int *count);

// Extract the value of a specific key
char *config_get_value(config_t *session, const char *category, const char *entry, int count);

// Free memory allocated by the config_init function
void configparse_cleanup(config_t *storage, int count);

/* --- confwrite */
// Load the config file and open main stream
FILE *configwrite_init(char *filename);

// Set a new value of a specific key or replace the current
int config_set_value(FILE *fp, const char *category, const char *key, const char *value);

// Cleanup
void configwrite_cleanup(FILE *fp);
```

#### Details:
`config_validate`

Parameters:
- `filename`: config file 
- `verbose`: verbose mode, 1 to enable messages and 0 to hide

Description:

The config_validate function checks the file for required formats like commenting, size and type.

Return value:

configvalidate will return `-1` when any check fails and `0` when no failures where detected.

`configparse_init`

Parameters:
- `filename`: config file
- `count`: number of entries (lines) set in `filename`

Description:

config_init is used to load the config file and also checks for some issues when `config_validate` isn't 
called. This function will tokenize the entries and perform various handles.

Return value:

On success, config_init returns a pointer to an array containing the `key` and the `value` of the entry in 
specific category. If it fails, it will print a message to stderr with a hint to the failure.

`config_get_value`

Parameters:
- `session`: pointer to session
- `entry`: key to get the value from
- `category`: category where to find the key 
- `count`: number of entries (lines)

Description:

config_get_value will query the value in the main pool to find the corresponding value. 

Return value:

If the call fails, config_get_value returns NULL. Otherwise, the value of the specified entry is returned.

`configparse_cleanup`

Parameters:
- `storage`: entry pool
- `count`: number of entries to free allocated ressources

Description:

`configparse_init` will allocate memory for the current structure and returns a pointer to it. This `needs to be freed` and the config_cleanup function is used to free the previous allocated ressources when there is no need anymore.

`configwrite_init`

Parameters:
- `filename`: config file

Description:

Like configparse_init, the configwrite_init function also performs some basic checks to `validate` the `structure` of the given config file and `opens a stream` if the tests are passed. 

Return value:

configparse_init returns a `pointer` to the main file object. If the function fails to 
open a stream, NULL is returned.

`config_set_value`

Parameters:
- `fp`: pointer to file object (stream opened by configwrite_init)
- `category`: config category
- `key`: config key
- `value`: new value for key

Description:

This function opens a secondary stream for a temporary file to which the updated entries are 
written. The temporary file is then renamed to the actual file name of the configuration.

Return value:

If the value of a specific key was set successfully, config_set_value returns 0. Otherwise 
it will return -1.

`configwrite_cleanup` 

Parameters:
- `fp`: pointer to file object

Description:

This is the cleanup function for confwrite. Currently it is basically only closing the file stream. This can also be done by simply use `fclose` directly.

### Example
`test.ini`:
```ini
# -- confparse test file
# Comments can be defined using a hashtag

[Client]
ip = "127.0.0.1" 
port=5003 

[Host]
OS = "Windows" 
``` 

`test.c`:
```C
#include "../confparse/confparse.h"
#include "../confparse/confwrite.h"

int main(void)
{
	char *config_file = "test.ini";
    // Validate the config file structure
	int check = config_validate(config_file, 0); 

	if (check != 1)
		puts("OK");

	FILE *writer = configwrite_init(config_file);

	if (writer == NULL)
	{
		fprintf(stderr, "Failed to initialize configwrite session\n");
		return 1;
	}

	int set_value = config_set_value(writer, "Client", "port", "1112");

	if (set_value == 0)
		puts("Value updated");
	else
		printf("config_set_value returns: %d\n", set_value);

	int count;
    // Initialise session
	config_t *config = configparse_init(config_file, &count); 

	if (config == NULL)
	{
		fprintf(stderr, "Failed to initialize configparse session\n");
		return 1;
	}

	const char *category = "Client";
	const char *entry = "ip";

	// Specify the category and key to parse the file and extract a specific
	// value
	char *value = config_get_value(config, category, entry, count);

	if (value != NULL)
		printf("category: %s, entry: %s, value: %s\n", category, entry, value);
	else
		printf("Entry %s not found in %s\n", entry, category);

	// Free allocated ressources
	configparse_cleanup(config, count);
	configwrite_cleanup(writer);

	return 0;
}
```

`Testfile output`:
```
OK
Value updated
category: Client, entry: ip, value: 127.0.0.1
```

### TODO
- [ ] Dynamically manipulate category, key or value
- [ ] Function for backing up the config file
- [ ] Automatically create and setup a new config file
- [ ] ...

### License

The confparse project has been released under the [MIT](https://github.com/fhAnso/confparse/blob/main/LICENSE) License.
