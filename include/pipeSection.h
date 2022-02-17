#ifndef __PIPE_SECTION__
#define __PIPE_SECTION__

#include "common.h"

int lerPipeSection (PipeSection * out, FILE* fp);
void executarPipeSection (PipeSection* pipeSection, Processo ** processosBackground, Processo ** processosCorrentes, Historico* historico);
void verificarRedirecionamento(PipeSection* pipeSection);
void verificarAliases (PipeSection* pipeSection);
void append(char** out, char* in);
void desalocarPipeSection(PipeSection* pipeSection);

#endif