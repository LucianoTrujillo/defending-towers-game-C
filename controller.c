#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "servicio_configuracion.h"

void manual(){
	
}

int main(int argc, const char *argv[]){
	if(argc == 1){
		printf("Se requiere un comando a ejecutar.\n");
		exit(FAILURE);
	} else {
		if(strcmp(argv[1], JUGAR) == 0) {
			jugar(argc ,argv);
		} else if(strcmp(argv[1], RANKING) == 0){
			ranking(argc, argv);
		} else if(strcmp(argv[1], CREAR_CAMINO) == 0){
			crear_camino(argc, argv);
		} else if(strcmp(argv[1], REPE) == 0){
			poneme_la_repe(argc, argv);
		} else if(strcmp(argv[1], CREAR_CONFIG) == 0){
			crear_configuracion(argc, argv);
		}
	}
	exit(SUCCESS);
}