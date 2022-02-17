#ifndef __PROCESSO__
#define __PROCESSO__

#include "common.h"
#include "historico.h"

void inicializarProcessos(Processo** processos);
void adicionarAProcessos(Processo** processos, int pid, Historico *historico);
void removerDePocessos(Processo** processos, int pid);
int nProcessos(Processo* processos);
Processo* ultimoProcesso(Processo* processos);
void desalocarProcessos(Processo* processos, int desalocarComando);

#endif