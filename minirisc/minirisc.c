#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "minirisc.h"


minirisc_t* minirisc_new(uint32_t initial_PC, platform_t *platform)
{
	// Création du processeur : registres à zéro, PC sur le point d'entrée
	minirisc_t *mr = malloc(sizeof(minirisc_t));
	if (mr == NULL)
		return NULL;

	memset(mr, 0, sizeof(minirisc_t));
	mr->PC       = initial_PC;
	mr->next_PC  = initial_PC;
	mr->platform = platform;
	return mr;
}


void minirisc_free(minirisc_t *mr)
{
	free(mr);
}


void minirisc_fetch(minirisc_t *mr)
{
	// Lecture de l'instruction (32 bits) pointée par PC dans IR
	if (platform_read(mr->platform, PLATFORM_ACCESS_WORD, mr->PC, &mr->IR)) {
		fprintf(stderr, "Erreur de fetch à l'adresse 0x%08x\n", mr->PC);
		mr->error = 1;
		mr->halt  = 1;
	}
}


void minirisc_decode_and_execute(minirisc_t *mr)
{
	// Première étape du décodage : l'opcode est dans les 7 bits de poids faible.
	// Par défaut, l'instruction suivante est à PC + 4.
	uint32_t opcode = mr->IR & 0x7f;
	mr->next_PC = mr->PC + 4;

	switch (opcode) {
	case 0x73: // SYSTEM : pour l'instant seul ebreak (0x00100073) est géré
		if (mr->IR == 0x00100073) {
			mr->halt = 1;
			break;
		}
		/* fallthrough */
	default:
		// Instructions pas encore implémentées : on arrête l'émulation
		fprintf(stderr, "Instruction non implémentée 0x%08x (opcode 0x%02x) à PC=0x%08x\n",
		        mr->IR, opcode, mr->PC);
		mr->error = 1;
		mr->halt  = 1;
		break;
	}

	mr->regs[0] = 0; // r0 est toujours câblé à zéro
}


int minirisc_run(minirisc_t *mr)
{
	// Boucle principale : fetch -> decode/execute -> mise à jour du PC
	while (!mr->halt) {
		minirisc_fetch(mr);
		if (mr->halt)
			break;
		minirisc_decode_and_execute(mr);
		mr->PC = mr->next_PC;
	}
	return mr->error ? -1 : 0;
}
