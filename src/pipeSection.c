#include "pipeSection.h"
#include "funcoesShell.h"

int lerPipeSection (PipeSection * out, FILE* fp){
	fp = fp == NULL ? stdin: fp;
	PipeSection pipe = {0,0,0};
	char* string = malloc(1);
	string[0] = 0;
	char c;
	do
	{
		c = getc(fp);
		if(c == '#') {
			while(c != '\n' && c != EOF){
				c = getc(fp);
			}
		}
		if (c != '\n' && c != EOF && c != ' ' && c != '|'){
			char* tmp = malloc(strlen(string)+ 1);
			if (tmp == NULL){
				printf("Erro ao alocar memoria para o comando\n");
				exit(1);
			}

			strcpy(tmp, string);
			strncat(tmp, &c, 1);
			free(string);

			string = tmp;
		}
		else if(strlen(string)){
			char** tmp = malloc((pipe.len + 1)*sizeof(char*));
			if (tmp == NULL){
				printf("Erro ao alocar memoria para o comando\n");
				exit(1);
			}

			for(int i = 0; i < pipe.len; i++){
				tmp[i] = pipe.args[i];
			}

			tmp[pipe.len] = string;

			free(pipe.args);
			pipe.len ++;
			pipe.args = tmp;
			string = malloc(1);
			string[0] = 0;
		}
	} while (c != '\n' && c != EOF && c != '|');

	//adicionando argumento NULL ao final do pipe section
	char** tmp = malloc((pipe.len + 1)*sizeof(char*));
	if (tmp == NULL){
		printf("Erro ao alocar memoria para o comando\n");
		exit(1);
	}

	for(int i = 0; i < pipe.len; i++){
		tmp[i] = pipe.args[i];
	}

	tmp[pipe.len] = NULL;

	free(pipe.args);
	pipe.removidos ++;
	pipe.args = tmp;

	out->len = pipe.len;
	out->removidos = pipe.removidos;
	out->args = pipe.args;
	if( c == '\n'){
		return 1;
	}
	if( c == EOF){
		return 2;
	}
	return 0;
}

void executarPipeSection (PipeSection* pipeSection, Processo ** processosBackground, Processo ** processosCorrentes, Historico * historico){
	char* pathBackup = getpathunb();
	char* path = strtok(pathBackup, ";");
	
	DIR *dir;
	struct dirent *entrada;

	while(path != NULL)
	{
		if((dir = opendir(path)) != NULL){
			while ((entrada = readdir(dir)) != NULL){
				if(!strcmp(entrada->d_name, pipeSection->args[0])){
			    	char binpath[strlen(path) + strlen(entrada->d_name) + 1];
			    	strcpy(binpath, path);
			    	strcat(binpath, "/");
			    	strcat(binpath, entrada->d_name);
			    	
			    	if(access(binpath, F_OK|X_OK) == 0){
			    		closedir(dir);
			    		free(pathBackup);
			    		verificaProgramaEmLote(binpath, processosBackground, processosCorrentes, historico	);
				    	execv(binpath, pipeSection->args);
						printf("um erro ocorreu na execucao do comando %s\n\n", pipeSection->args[0]);
			    	}
				}
			}
			closedir(dir);
		}
		path = strtok(NULL, ";");
	}
	printf("UnBsh-%s-/home/%s>:Não achei o comando\n", getenv("USER"),getenv("USER"));
	free(pathBackup);
}

void verificarRedirecionamento(PipeSection* pipeSection){
	int redirecionar[2] = {-1,-1};
	int append = 0;
	int entrada;
	int saida;
	for (int i = 0; i < pipeSection->len; i++){
		if(!strcmp(pipeSection->args[i], "<")){
			redirecionar[0] = i;
		}
		else if(!strcmp(pipeSection->args[i], ">")){
			redirecionar[1] = i;
		}
		else if(!strcmp(pipeSection->args[i], ">>")){
			redirecionar[1] = i;
			append = 1;
		}
	}
	//redirecionamento de entrada
	if(redirecionar[0] != -1){
		entrada = open(pipeSection->args[redirecionar[0] + 1], O_RDONLY | S_IRUSR);
		if(entrada == -1){
			printf("redirecionamento falhou\n");
			exit(1);
		}

		close(0);
		dup(entrada);
		close(entrada);

		pipeSection->args[redirecionar[0] + 1] = pipeSection->args[redirecionar[0]] = NULL;
		pipeSection->removidos += 2;
		pipeSection->len -= 2;
	}
	//redirecionamento de saida
	if(redirecionar[1] != -1){
		int mode;
		if(append){
			mode = O_WRONLY | O_CREAT | O_APPEND;
		}
		else{
			mode = O_WRONLY | O_CREAT;
		}
		saida = open(pipeSection->args[redirecionar[1] + 1], mode, S_IWUSR | S_IRUSR);
		if(saida == -1){
			printf("redirecionamento falhou: %s\n", strerror(errno));
			exit(1);
		}

		close(1);
		dup(saida);
		close(saida);

		pipeSection->args[redirecionar[1] + 1] = pipeSection->args[redirecionar[1]] = NULL;
		pipeSection->removidos += 2;
		pipeSection->len -= 2;
	}
}
void verificarAliases (PipeSection* pipeSection){
	char searchString[strlen(pipeSection->args[0]) + 3];
	strcpy(searchString, "\"");
	strcat(searchString, pipeSection->args[0]);
	strcat(searchString, "\"");
	FILE * config = openConfig();
	char* alias = findLineInFile(searchString, &config);

	if(alias == NULL){
		fclose(config);
		return;
	}


	for(int i = 0; alias[i + 8] != '\"'; i++){
		alias[i] = alias[i + 8];
		alias[i + 1] = 0;
	}

	free(pipeSection->args[0]);
	pipeSection->args[0] = alias;
	fclose(config);
}
void append(char** out, char* in){
	char* temp;
	if (*out != NULL){
		temp = malloc(strlen(*out) + strlen(in) + 1);
		if (temp == NULL){
			printf("Erro ao alocar memoria para o comando\n");
			exit(1);
		}
		strcpy(temp, *out);
		strcat(temp, in);
		free(*out);
	}
	else{
		temp = malloc(strlen(in) + 1);
		if (temp == NULL){
			printf("Erro ao alocar memoria para o comando\n");
			exit(1);
		}
		strcpy(temp, in);
	}
	*out = temp;
}

void desalocarPipeSection(PipeSection* pipeSection){
	if(pipeSection->args != NULL){
		for (int i = 0; i < (pipeSection->len + pipeSection->removidos); i++){
			if(pipeSection->args[i] != NULL){
				free(pipeSection->args[i]);
			}
		}
		free(pipeSection->args);
	}
}