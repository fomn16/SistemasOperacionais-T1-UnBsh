#include "funcoesShell.h"
#include "comando.h"

char* findLineInFile(char* lineContent, FILE ** file){
	char cAtual = 0;
	long int contentCunter = 0;
	int found = 0;
	long int lineSize = 0;
	char * line = malloc(1);
	if (line == NULL){
		printf("Erro ao alocar memoria ler arquivo(1)\n");
		exit(1);
	}
	char * tmpLine;
	while (cAtual != EOF){
		cAtual = fgetc(*file);

		if(!found){
			if (cAtual == lineContent[contentCunter]){
			contentCunter++;
			}
			else{
				contentCunter = 0;
			}

			if(lineContent[contentCunter] == 0){
				found = 1;
			}

			if(cAtual == '\n' || cAtual == EOF){
				lineSize = 0;
				line[0] = 0;
			}
		}
		else{
			if(cAtual == '\n' || cAtual == EOF){
				return line;
			}
		}

		lineSize++;
		tmpLine = line;
		line = malloc(lineSize + 1);
		if (line == NULL){
			printf("Erro ao alocar memoria ler arquivo(2)\n");
			exit(1);
		}
		line[lineSize] = 0;
		if (line == NULL){
			printf("Erro ao alocar memoria para PATH interno\n");
			exit(1);
		}
		strcpy(line, tmpLine);
		free(tmpLine);
		line[lineSize - 1] = cAtual;
	}
	free(line);
	rewind(*file);
	return NULL;
}

FILE * openConfig(){
	char confPath[strlen("/home/") + strlen(getenv("USER")) + strlen("/.unbshrc_profile") + 1];
	strcpy(confPath, "/home/");
	strcat(confPath, getenv("USER"));
	strcat(confPath, "/.unbshrc_profile");
	FILE * config = fopen(confPath, "r");
	if(config == NULL){
		printf("ERRO: Arquivo de configuracao nao encontrado\n");
		exit(1);
	}
	return config;
}

char* getpathunb(){
	FILE * config = openConfig();
	char* ret = findLineInFile("PATH=", &config);
	if(ret == NULL){
		printf("ERRO: Arquivo de configuracao nao possui PATH definido\n");
		fclose(config);
		exit(1);
	}
	fclose(config);
	for(int i = 0;ret[i + 5] != 0; i++){
		ret[i] = ret[i+5];
		ret[i + 1] = 0;
	}
	return ret;
}

void verificaProgramaEmLote (char* path, Processo ** processosBackground, Processo ** processosCorrentes, Historico * historico){
	FILE* fp = fopen(path, "r");

	if(fp == NULL) return;
	char* condInicio1 = findLineInFile("#!/", &fp);
	char* condInicio2 = findLineInFile("#! /", &fp);
	char* condFim = findLineInFile("/unbsh", &fp);

	
	if(condFim != NULL && (!strcmp(condInicio1, condFim) || !strcmp(condInicio2, condFim))){
		free(condInicio1);
		free(condInicio2);
		free(condFim);

		int done = 0;
		while(!done){
			Comando comando;
			done = lerComando(&comando, fp);
			if(comando.len){
				executarComando(&comando, processosBackground, processosCorrentes, historico);
			}
			desalocarComando(&comando);
		}
		exit(0);
	}
	free(condInicio1);
	free(condInicio2);
	free(condFim);
	fclose(fp);
}