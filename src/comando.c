#include "comando.h"

int lerComando(Comando* out, FILE* fp){
	int done = 0;
	Comando comando = {0,0};
	do{
		PipeSection pipeSection;

		done = lerPipeSection(&pipeSection, fp);
		if(pipeSection.len){
			PipeSection * temp = malloc((comando.len + 1) * sizeof(PipeSection));

			if (temp == NULL){
				printf("Erro ao alocar memoria para o comando\n");
				exit(1);
			}

			for(int i = 0; i < comando.len; i++){
				temp[i] = comando.pipeSections[i];
			}

			temp[comando.len] = pipeSection;
			free(comando.pipeSections);
			comando.pipeSections = temp;
			comando.len ++;
		}
	} while (!done);
	out->len = comando.len;
	out->pipeSections = comando.pipeSections;
	if(done == 2){
		return 1;
	}
	return 0;
}

void executarComando(Comando * comando, Processo ** processosBackground, Processo ** processosCorrentes, Historico * historico){
	
	adicionarAoHistorico(comando, historico);

	int background = verificarComandoBackground(comando);

	int fd[2][2];
	for (int i = 0; i < comando->len; i++){
		verificarAliases(&comando->pipeSections[i]);
		//execucao normal caso o comando nao seja interno
		int tipoExecucao = verificarComandoInterno(comando, i, processosBackground, processosCorrentes, historico);

		if(tipoExecucao == 2){
			break;
		}
		if(tipoExecucao == 0){
			//pipe criado se existe pipeline e i < len - 1
			//nao sobrescrever ultimos fds criados -> 2 pipes intercalados
			//pipe atual = i%2; ultimo pipe = (i+1)%2
			if(i != comando->len - 1){
				pipe(fd[i%2]); // 0 = entrada; 1 = saida
			}

			int rc = fork();
			if (rc < 0) { // fork deu errrado
				printf("erro ao criar processo\n\n");
				exit(1);
			} else if (rc == 0) { // novo processo
				//se nao for primeiro pipe section -> entrada = ultimo pipe
				if(i != 0){
					close(0);
					close(fd[(i+1)%2][1]);
					dup(fd[(i+1)%2][0]);
					close(fd[(i+1)%2][0]);
				}
				//se nao for ultimo pipe section t-> saida = pipe atual
				if(i != comando->len - 1){
					close(1);
					close(fd[i%2][0]);
					dup(fd[i%2][1]);
					close(fd[i%2][1]);
				}
				verificarRedirecionamento(&comando->pipeSections[i]);
				executarPipeSection(&comando->pipeSections[i], processosBackground, processosCorrentes, historico);
				exit(0);
			}
			else{ // processo pai
				if(i != comando->len - 1){
					close(fd[i%2][1]);
				}
				if(i != 0){
					close(fd[(i+1)%2][0]);
				}
				
				if(i == comando->len - 1){
					if(background){
						adicionarAProcessos(processosBackground, rc, historico);
					}
					else{
						adicionarAProcessos(processosCorrentes, rc, historico);
					}
				}
			}
		}
	}

	if(background){
		printf("UnBsh-%s-/home/%s->Processo em background [%d] foi iniciado\n\n", getenv("USER"),getenv("USER"), ultimoProcesso(*processosBackground)->pid);
	}
	else{
		Processo* processo = ultimoProcesso(*processosCorrentes);

		if(processo != NULL){
			waitpid(processo->pid, NULL, 0);
			inicializarProcessos(processosCorrentes);
		}
	}
}

int verificarComandoInterno(Comando* comando, int pipeSection, Processo ** processosBackground, Processo ** processosCorrentes, Historico * historico){
	if(!strcmp(comando->pipeSections[pipeSection].args[0], "cd")){
		if(comando->pipeSections[pipeSection].len != 2){
			printf("numero incorreto de argumentos para comando cd\n");
		}
		else{
			chdir(comando->pipeSections[pipeSection].args[1]);
		}
		return 1;
	}
	if(!strcmp(comando->pipeSections[pipeSection].args[0], "ver")){
		printf("UnBsh 18.16 atualizado em 11/02/2020 por Felipe Oliveira\n");
		return 1;
	}
	if(!strcmp(comando->pipeSections[pipeSection].args[0], "history") || !strcmp(comando->pipeSections[pipeSection].args[0], "historico")){
		if(comando->pipeSections[pipeSection].len < 2){
			printHistorico(historico);
			return 1;
		}
		else if(comando->pipeSections[pipeSection].len == 2){	
			int n = atoi(comando->pipeSections[pipeSection].args[1]);
			if(n > 10 || n > historico->cur){
				printf("Historico nao achado\n");
				return 2;
			}
			Comando cmdo;
			int id = (n + historico->cur - 1)%10;
			if(historico->cur < 10){
				id = n - 1;
			}
			duplicarComando(&cmdo, &(historico->comandos[id]));
			executarComando(&cmdo, processosBackground, processosCorrentes, historico);
			desalocarComando(&cmdo);
			return 1;
		}
	}
	if(!strcmp(comando->pipeSections[pipeSection].args[0], "exit")){
		printf("UnBsh-%s-/home/%s-> Encerrando o shell\n", getenv("USER"),getenv("USER"));
		exit(0);
	}
	return 0;
}

int verificarComandoBackground(Comando * comando){
	int background = 0;
	int nSections = comando->len;
	int nArgs = comando->pipeSections[nSections-1].len;
	if(!strcmp(comando->pipeSections[nSections-1].args[nArgs - 1], "&")){
		background = 1;
		comando->pipeSections[nSections-1].args[nArgs - 1] = NULL;
		comando->pipeSections[nSections-1].len --;
		comando->pipeSections[nSections-1].removidos ++;
	}
	return background;
}

void duplicarComando(Comando* out, Comando* in){
	out->len = in->len;
	out->pipeSections = malloc(sizeof(PipeSection)*out->len);
	if (out->pipeSections == NULL){
		printf("Erro ao alocar memoria para duplicar registro de comando(1)\n");
		exit(1);
	}
	for (int i = 0; i < out->len; i++){
		out->pipeSections[i].len = in->pipeSections[i].len;
		out->pipeSections[i].removidos = in->pipeSections[i].removidos;
		out->pipeSections[i].args = malloc(sizeof(char*)*(out->pipeSections[i].len + out->pipeSections[i].removidos));
		if (out->pipeSections[i].args == NULL){
			printf("Erro ao alocar memoria para duplicar registro de comando(2)\n");
			exit(1);
		}
		for (int j = 0; j < (out->pipeSections[i].len + out->pipeSections[i].removidos); j++){
			if(in->pipeSections[i].args[j] != NULL){
				out->pipeSections[i].args[j] = malloc(strlen(in->pipeSections[i].args[j]) + 1);
				if (out->pipeSections[i].args[j] == NULL){
					printf("Erro ao alocar memoria para duplicar registro de comando(3)\n");
					exit(1);
				}
				strcpy(out->pipeSections[i].args[j], in->pipeSections[i].args[j]);
			}
			else{
				out->pipeSections[i].args[j] = NULL;
			}
		}
	}
}

void printComando(Comando comando){
	for (int j = 0; j < comando.len; j++){
		if(j) printf(" |");
		for (int k = 0; k < comando.pipeSections[j].len; k++){
			if(comando.pipeSections[j].args != NULL)
				printf(" %s", comando.pipeSections[j].args[k]);
		}
	}
}

char* comandoParaString(Comando comando){
	char* out = NULL;
	for (int j = 0; j < comando.len; j++){
		if(j) append(&out, "| ");
		for (int k = 0; k < comando.pipeSections[j].len; k++){
			if(comando.pipeSections[j].args[k] != NULL){
				append(&out, comando.pipeSections[j].args[k]); 
				if(!(k == comando.pipeSections[j].len-1 && j == comando.len-1))
					append(&out, " ");
			}
		}
	}
	return out;
}

void desalocarComando(Comando * comando){
	for (int i = 0; i < comando->len; i++){
		desalocarPipeSection(&comando->pipeSections[i]);
	}
	free(comando->pipeSections);
}