#ifndef __COMANDO__
#define __COMANDO__

#include "common.h"
#include "pipeSection.h"
#include "historico.h"
#include "processo.h"

int lerComando(Comando* out, FILE* fp);
void executarComando(Comando * comando, Processo ** processosBackground, Processo ** processosCorrentes, Historico * historico);
int verificarComandoInterno(Comando* comando, int pipeSection, Processo ** processosBackground, Processo ** processosCorrentes, Historico *historico);
int verificarComandoBackground(Comando * comando);
void duplicarComando(Comando* out, Comando* in);
void printComando(Comando comando);
char* comandoParaString(Comando comando);
void desalocarComando(Comando * comando);

#endif