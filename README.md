# confparse - Small configuration file parsing library

The confparse project facilitates the process of parsing a configuration file into keys and their corresponding values. These pairs can be handled individually, allowing for easy reading of configuration files. Currently, this project supports up to 500 entries.

### Usage

- To use confparse, copy the confparse directory to your project. 
```C
#include "confparse/confparse.h"
```

- Function overview
```C
// Validate config file
int config_validate(const char *filename, unsigned int verbose);

// Load the config file and define the number of entries
config_t *configparse_init(const char *filename, int *count);

// Extract the value of a specific key
char *config_get_value(config_t *session, const char *category, const char *entry, int count);

// Free memory allocated by the config_init function
void configparse_cleanup(config_t *storage, int count);
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

`config_init`

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

`config_cleanup`

Parameters:
- `storage`: entry pool
- `count`: number of entries to free allocated ressources

Description:

`config_init` will allocate memory for the current structure and returns a pointer to it. This `needs to be freed` and the config_cleanup function is used to free the previous allocated ressources when there is no need anymore.

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

int main(void) 
{
    const char *config_file = "test.ini";
    int check = config_validate(config_file, 0); // Validate the config file structure

	if (check != 1) puts("OK");

    int count;
    config_t *config = configparse_init(config_file, &count); // Initialise session

    if (config == NULL) 
    {
        fprintf(stderr, "Failed to initialize session\n");
        return 1;
    }

    const char *category = "Client";
    const char *entry = "ip";
    
    // Specify the category and key to parse the file and extract a specific value 
    char *value = config_get_value(config, category, entry, count);

    if (value != NULL) 
        printf("category: %s, entry: %s, value: %s\n", category, entry, value);
    else 
        printf("Entry %s not found in %s\n", entry, category);

    // Free allocated ressources
    configparse_cleanup(config, count);

    return 0;
}
```

`Testfile output`:
```
OK
category: Client, entry: ip, value: 127.0.0.1
```

### TODO
- [ ] Dynamically manipulate category, key or value
- [ ] Function for backing up the config file
- [ ] Automatically create and setup a new config file
- [ ] ...

### License

The confparse project has been released under the [MIT](https://github.com/fhAnso/confparse/blob/main/LICENSE) License.
