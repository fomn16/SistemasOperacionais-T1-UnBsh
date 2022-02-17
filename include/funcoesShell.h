#ifndef __FUNCOES_SHELL__
#define __FUNCOES_SHELL__

#include "common.h"

char* findLineInFile(char* lineContent, FILE ** file);
FILE * openConfig();
char* getpathunb();
void verificaProgramaEmLote (char* path, Processo ** processosBackground, Processo ** processosCorrentes, Historico *historico);

#endif