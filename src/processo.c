#include "processo.h"
#include "comando.h"

void inicializarProcessos(Processo** processos){
	desalocarProcessos(*processos, 0);
	(*processos) = malloc(sizeof(Processo));
	if((*processos) == NULL){
		printf("Erro ao alocar memoria para o registro de Processo\n");
		exit(1);
	}
	(*processos)[0].pid = -1;
	(*processos)[0].comando = NULL;
}

void adicionarAProcessos(Processo** processos, int pid, Historico * historico){
	int n = nProcessos(*processos);
	Processo * temp = malloc(sizeof(Processo)*(n + 2));
	if(temp == NULL){
		printf("Erro ao alocar memoria para o registro de Processo\n");
		exit(1);
	}
	for(int i = 0; i < n; i++){
		temp[i].pid = (*processos)[i].pid;
		temp[i].comando = (*processos)[i].comando;
	}
	temp[n].pid = pid;
	temp[n].comando = comandoParaString(historico->comandos[historico->cur%10]);
	temp[n+1].pid = -1;
	temp[n+1].comando = NULL;
	desalocarProcessos(*processos, -1);
	*processos = temp;
}

void removerDePocessos(Processo** processos, int pid){
	int n = nProcessos(*processos);
	int offset = 0;
	Processo* temp = malloc(sizeof(Processo)*n);
	if (temp == NULL){
		printf("Erro ao alocar memoria para o registro de Processo\n");
		exit(1);
	}
	for(int i = 0; i < n; i++){
		if((*processos)[i].pid == pid) offset++;
		temp[i].pid = (*processos)[i + offset].pid;
		temp[i].comando = (*processos)[i + offset].comando;
	}
	desalocarProcessos(*processos, pid);
	*processos = temp;
}

int nProcessos(Processo* processos){
	int i = 0;
	while(1){
		if (processos[i].pid == -1) return i;
		i++;
	}
}

Processo* ultimoProcesso(Processo* processos){
	if(processos[0].pid == -1) return NULL;
	int i = 0;
	while(1){
		if(processos[i+1].pid == -1) return &processos[i];
		i++;
	}
}

void desalocarProcessos(Processo* processos, int desalocarComando){
	if(processos != NULL){
		int i = 0;
		while(1){
			if(processos[i].pid == -1) break;
			//desalocar comandos -> 0 desaloca todos, -1 nao desaloca, pid desaloca apenas com o pid
			if(!desalocarComando || (desalocarComando == processos[i].pid)) 
				free(processos[i].comando);
			i++;
		}
		free(processos);
	}
}