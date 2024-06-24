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
