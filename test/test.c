#include "../confparse/confparse.h"

int main(void) 
{
    const char *config_file = "test.ini";
    int check = config_validate(config_file, 0); // Validate the config file structure

	if (check != 1) puts("OK");

    int count;
    config_t *config = config_init(config_file, &count); // Initialise session

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
    config_cleanup(config, count);

    return 0;
}
