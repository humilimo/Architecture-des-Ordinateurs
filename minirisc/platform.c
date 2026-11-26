#include <stdlib.h>
#include <stdio.h>
#include "platform.h"


platform_t* platform_new()
{
	platform_t *platform = calloc(1, sizeof(platform_t));
	if (platform == NULL){
		perror("calloc platform");
		exit(EXIT_FAILURE);
	}
	platform->memory = calloc(32, 1024 * 256 * sizeof(uint32_t));
	if (platform->memory == NULL){
		perror("calloc platform->memory");
		exit(EXIT_FAILURE);
	}
	return platform;
}


void platform_free(platform_t *platform)
{
	if (platform == NULL) {
		return;
	}
	free(platform->memory);
	free(platform);
}


int platform_load_program(platform_t *plt, const char *file_name)
{
	// TODO
}


int platform_read(platform_t *plt, platform_access_type_t access_type, uint32_t addr, uint32_t *data)
{
	// TODO
}


int platform_write(platform_t *plt, platform_access_type_t access_type, uint32_t addr, uint32_t data)
{
	// TODO
}


