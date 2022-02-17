#ifndef __COMMON__
#define __COMMON__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

typedef struct PipeSection
{
	int len;
	int removidos;
	char** args;
} PipeSection;

typedef struct Comando
{
	int len;
	PipeSection * pipeSections;
} Comando;

typedef struct Historico
{
	int cur;
	Comando comandos[10];
} Historico;

typedef struct Processo{
	int pid;
	char* comando;
} Processo;

#endif