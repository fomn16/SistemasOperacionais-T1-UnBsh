#include "common.h"
#include "comando.h"
#include "processo.h"
#include "historico.h"

Historico historico;
Processo * processosCorrentes = NULL;
Processo * processosBackground = NULL;

int main(){
	//inicializando historico
	historico.cur = -1;

	//inicializando processos atuais e em background
	inicializarProcessos(&processosCorrentes);
	inicializarProcessos(&processosBackground);

	while (1){
		//infelizmente nao encontrei uma forma de recuperar diretorio corrente com tamanho variavel
		char cwd[400];
		getcwd(cwd, 400);

		//se existem processos em background, verifica se eles foram encerrados
		if(nProcessos(processosBackground)){
			int i = 0;
			while(1){
				if(processosBackground[i].pid == -1) break;
				//se processo foi encerrado, avisa usuario antes do procimo prompt
				if(waitpid(processosBackground[i].pid, NULL, WNOHANG) != 0){
					printf("\nUnB - %s – %s -> Processo em background [%d] executado \"%s\"\n",getenv("USER"), cwd, processosBackground[i].pid, processosBackground[i].comando);
					removerDePocessos(&processosBackground, processosBackground[i].pid);
				}
				else i++;
			}
		}

		//prompt do shell
		printf("\nUnB - %s – %s -> ",getenv("USER"), cwd);

		//declarando e lendo comando
		Comando comando;
		lerComando(&comando, NULL);

		//executando comando
		executarComando(&comando, &processosBackground, &processosCorrentes, &historico);

		//desalocando comando
		desalocarComando(&comando);
	}
	return 0;
}