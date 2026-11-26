#include <stdint.h>
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
	platform->memory = calloc(32, MEGABYTE);
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
	FILE *f = fopen(file_name, "rb");
	if (f == NULL){
		perror("fopen f");
		return -1;
	}
	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (file_size < 0 || file_size > 32 * MEGABYTE){
		fclose(f);
		return -1;
	}
	unsigned long read = fread(plt->memory, 1, file_size, f);
	if (read < file_size){
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}


int platform_read(platform_t *plt, platform_access_type_t access_type, uint32_t addr, uint32_t *data)
{
	// TODO
}


int platform_write(platform_t *plt, platform_access_type_t access_type, uint32_t addr, uint32_t data)
{
	// TODO
}


