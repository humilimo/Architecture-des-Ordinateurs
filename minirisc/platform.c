#include <stdlib.h>
#include <stdio.h>
#include "platform.h"

/*
 * Carte mémoire de la plateforme :
 *  - RAM de 32 Mo à partir de l'adresse 0x80000000 (cf. minirisc.ld) ;
 *  - un périphérique "console" à l'adresse 0x10000000 : écrire un octet
 *    à cette adresse affiche le caractère correspondant sur stdout.
 */
#define RAM_BASE      0x80000000u
#define RAM_SIZE      (32u * 1024u * 1024u)
#define CHAROUT_ADDR  0x10000000u


platform_t* platform_new()
{
	// Allocation de la plateforme et de sa RAM (initialisée à zéro par calloc)
	platform_t *plt = malloc(sizeof(platform_t));
	if (plt == NULL)
		return NULL;

	plt->memory = calloc(RAM_SIZE / 4, sizeof(uint32_t));
	if (plt->memory == NULL) {
		free(plt);
		return NULL;
	}
	return plt;
}


void platform_free(platform_t *platform)
{
	if (platform == NULL)
		return;
	free(platform->memory);
	free(platform);
}


int platform_load_program(platform_t *plt, const char *file_name)
{
	// Le fichier binaire brut est copié tel quel au début de la RAM
	FILE *f = fopen(file_name, "rb");
	if (f == NULL) {
		perror(file_name);
		return -1;
	}

	size_t n = fread(plt->memory, 1, RAM_SIZE, f);
	if (ferror(f)) {
		fprintf(stderr, "Erreur de lecture de %s\n", file_name);
		fclose(f);
		return -1;
	}
	fclose(f);

	printf("Programme chargé : %zu octets\n", n);
	return 0;
}


int platform_read(platform_t *plt, platform_access_type_t access_type, uint32_t addr, uint32_t *data)
{
	// Les valeurs de l'enum (0, 1, 3) servent directement de masque d'alignement
	if (addr & access_type)
		return -1; // accès non aligné

	if (addr < RAM_BASE || addr - RAM_BASE >= RAM_SIZE)
		return -1; // hors de la RAM

	// On lit le mot de 32 bits contenant l'adresse, puis on extrait
	// l'octet ou le demi-mot voulu (little-endian)
	uint32_t offset = addr - RAM_BASE;
	uint32_t word   = plt->memory[offset >> 2];
	uint32_t shift  = (offset & 3) * 8;

	switch (access_type) {
	case PLATFORM_ACCESS_BYTE: *data = (word >> shift) & 0xff;   break;
	case PLATFORM_ACCESS_HALF: *data = (word >> shift) & 0xffff; break;
	case PLATFORM_ACCESS_WORD: *data = word;                     break;
	default: return -1;
	}
	return 0;
}


int platform_write(platform_t *plt, platform_access_type_t access_type, uint32_t addr, uint32_t data)
{
	if (addr & access_type)
		return -1; // accès non aligné

	// Périphérique console : on affiche le caractère reçu
	if (addr == CHAROUT_ADDR) {
		putchar(data & 0xff);
		fflush(stdout);
		return 0;
	}

	if (addr < RAM_BASE || addr - RAM_BASE >= RAM_SIZE)
		return -1; // hors de la RAM

	// Écriture partielle : on ne modifie que les bits concernés du mot
	uint32_t offset = addr - RAM_BASE;
	uint32_t *word  = &plt->memory[offset >> 2];
	uint32_t shift  = (offset & 3) * 8;
	uint32_t mask;

	switch (access_type) {
	case PLATFORM_ACCESS_BYTE: mask = 0xffu   << shift; break;
	case PLATFORM_ACCESS_HALF: mask = 0xffffu << shift; break;
	case PLATFORM_ACCESS_WORD: mask = 0xffffffffu;      break;
	default: return -1;
	}
	*word = (*word & ~mask) | ((data << shift) & mask);
	return 0;
}
