#include "historico.h"
#include "comando.h"

void adicionarAoHistorico(Comando* comando, Historico *historico){
	historico->cur ++;
	int id = historico->cur%10;
	if(historico->cur > 9){
		desalocarComando(&historico->comandos[id]);
	}
	duplicarComando(&historico->comandos[id], comando);
}

void printHistorico(Historico* historico){
	int n = historico->cur;
	if (n > 9) n = 9;
	for (int i = n; i >= 0; i--){
		int id = (historico->cur - i)%10;
		if(id < 0) id += 10;
		printf("%d", n - i + 1);
		printComando(historico->comandos[id]);
		printf("\n");
	}
}