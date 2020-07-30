#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/ioctl.h> //para obtener dimension de la consola
#include "commons.h"
#include "juego.h"
#include "utiles.h"
#include "servicio_configuracion.h"

/*
	Guarda en valor lo que esta despues del caracter igual "=" en argumento.
	El argumento debe tener al caracter igual "=".
*/
void cargar_valor_de_argumento(char valor[MAX_ARGUMENTO], const char* argumento){
	valor[0] = '\0';
	strcat(valor, strstr(argumento, "=") + 1);
}

/*
	Trata de abrir el archivo a partir de la ruta y modo pasada por parametros.
	De no ser posible imprime el mensaje de error en pantalla y detiene la ejecución del programa.
*/
FILE* abrir_archivo(const char* ruta, char* modo, char* mensaje_error){
	FILE* archivo = fopen(ruta, modo);
	if(!archivo){
		printf("%s", mensaje_error);
		exit(FAILURE);
	}
	return archivo;
}

/*
	Transforma una ruta de config, de formato: <nombre>.<extension> a ranking_<nombre>.<extension de ranking>
	Guarda el resultado en ruta_ranking.
*/
void transformar_ruta_config_a_ranking(char ruta_config[MAX_RUTA], char ruta_ranking[MAX_RUTA]){
	ruta_ranking[0] = '\0';
	strcat(ruta_ranking, PRE_RUTA_RANKING);
	strcat(ruta_ranking, strtok(ruta_config, "."));
	strcat(ruta_ranking, EXTENSION_RANKING);
}

/*
	Pide el valor de UNA de las componentes de una coordeada. Fila o Columna.
	El valor guardado en pos estará en el rango del tamaño del nivel.
*/
void pedir_ubicacion_entrada(const char* ubicacion_pedida, int* pos, int tamanio_nivel) {
	c_print("eliga la %s en la que desea colocar la entrada (0 a %i): ", ubicacion_pedida, tamanio_nivel - 1);
	scanf("%i", pos);

	while(*pos < 0 || *pos >= tamanio_nivel){
		c_print("la %s debe estar en el rango solicitad (0 a %i): ", ubicacion_pedida, tamanio_nivel - 1);
		scanf("%i", pos);
	}
}

/*
	Llena cada valor del terreno con el caracter que representa tierra, sin camino.
*/
void limpiar_terreno(char* terreno[MAX_FILAS][MAX_COLUMNAS], int tope){
	for(int i = 0; i < tope; i++){
		for(int j = 0; j < tope; j++){
			terreno[i][j] = TIERRA;
		}
	}
}

/*
	Imprime en pantalla las teclas para utilziar el creador de caminos, a la vez de cuantos casilleros lleva llenados del total.
*/
void imprimir_controles(int tope_camino){
	char tabla[MAX_TEXTO];
	size_t offset = OFFSET; // dado el uso de barritas se rompe un poco el formato y me vi obligado a usar este magic number como offset para que este alineado
	struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

	sprintf(tabla, "║ Marcar camino: W-A-S-D ║ Deshacer: CTRL + Z ║ Cancelar: Esc ║ Casilleros llenados: %d/%d ║", tope_camino, MAX_CAMINO );
	
	//imprimo parte de arriba
	printf("%*s", (int)(offset + (w.ws_col - strlen(tabla)) / 2), " ");
	printf("╔");
    for(int j = 0; j < strlen(tabla) - offset; j++){
        printf("═");
    }
	printf("╗\n");

	//imprimo texto
	printf("%*s", (int)(offset + (w.ws_col - strlen(tabla)) / 2), " ");
    printf("%s\n", tabla);

    //imprimo parte de abajo
	printf("%*s", (int)(offset + (w.ws_col - strlen(tabla)) / 2), " ");
    printf("╚");
    for(int j = 0; j < strlen(tabla) - offset; j++){
        printf("═");
    }
    printf("╝");
	
}

/*
	Imprimer el terreno del nivel, con el camino llenandose.
	Nota: Hago uso de  <sys/ioctl.h> para obtener dimension de la consola, luego mete espacios hasta la mitad y ahi recien imprime todo.
*/
void imprimir_camino(coordenada_t camino[MAX_CAMINOS], int tope_camino, int tamanio_nivel){
	char* terreno[MAX_FILAS][MAX_COLUMNAS];

	limpiar_terreno(terreno, tamanio_nivel);

	for(int i = 0; i < tope_camino; i++){
		coordenada_t pos_actual = camino[i];
		terreno[pos_actual.fil][pos_actual.col] = " ";
	}

	struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
	for (int i = 0; i < (w.ws_row - tamanio_nivel) / 2; i++)
        printf("\n");

	imprimir_controles(tope_camino);
	printf("\n\n");

	for(int i = 0; i < tamanio_nivel; i++){
		printf("%*s", (int)((w.ws_col - (tamanio_nivel * 3)) / 2), " ");
		printf("%5i |", i);
		for(int j = 0; j < tamanio_nivel; j++)
				printf(" %s ", terreno[i][j]);
		printf("\n");
	}

	printf("%*s", (int)((w.ws_col - (tamanio_nivel * 3)) / 2), " ");
	printf("%7c", ' ');
	for(int i = 0; i < tamanio_nivel * 3; i++)
		printf("-");
	printf("\n");

	printf("%*s", (int)((w.ws_col - (tamanio_nivel * 3)) / 2), " ");
	printf("%7c", ' ');
	for(int i = 0; i < tamanio_nivel; i++){
		if(i < 10)
			printf("%2i ", i);
		else	
			printf("%3i", i);
	}

	for (int i = 0; i < (w.ws_row - tamanio_nivel) / 2; i++)
        printf("\n");
}

/*
	Determina si el usuario terminó de crear el camino.
	El llegado depende de donde se encuentra la salida.
*/
bool camino_completado(coordenada_t pos_actual, int tamanio_nivel, int salida){
	switch(salida){
		case ESTE:  return pos_actual.col == tamanio_nivel - 1;
		case OESTE: return pos_actual.col == 0;
		case SUR:   return pos_actual.fil == tamanio_nivel - 1;
		case NORTE: return pos_actual.fil == 0;
		default: return false; 
	}
}

/*
	Obtiene y devuelve la coordenada de entrada del camino.
	El usuario solo determina una de las dos componentes, dependiendo del nivel.
*/
coordenada_t obtener_coordenada_entrada(int entrada, int tamanio_nivel){
	coordenada_t coordenada_entrada;

	switch(entrada){
		case ESTE: {
			coordenada_entrada.col = tamanio_nivel - 1;
			pedir_ubicacion_entrada("fila", &(coordenada_entrada.fil), tamanio_nivel);
			break;
		}
		case OESTE: {
			coordenada_entrada.col = 0;
			pedir_ubicacion_entrada("fila", &(coordenada_entrada.fil), tamanio_nivel);
			break;
		}
		case NORTE: {
			coordenada_entrada.fil = 0;
			pedir_ubicacion_entrada("columna", &(coordenada_entrada.col), tamanio_nivel);
			break;
		}
		case SUR: {
			coordenada_entrada.fil = tamanio_nivel - 1;
			pedir_ubicacion_entrada("columna", &(coordenada_entrada.col), tamanio_nivel);
			break;
		}
		default: break;
	}
	return coordenada_entrada;
}

/*
	Devuelve si el usuario puede moverse para la dirección ingresada por teclado.
	Devolverá verdadero si:
		- No llegó al maximo numero de casilleros
		- No se pasa del mapa
		- No camina para atras
*/
bool puede_hacer_movimiento(coordenada_t nueva_pos, coordenada_t pos_anterior, int tope_camino, int tamanio_nivel){
	
	if(nueva_pos.col == pos_anterior.col && nueva_pos.fil == pos_anterior.fil){
		return false;
	}

	if(tope_camino < MAX_CAMINO){
		if(nueva_pos.col >= 0 && nueva_pos.col < tamanio_nivel){
			if(nueva_pos.fil >= 0 && nueva_pos.fil < tamanio_nivel){
				return true;
			}
		}
	}

	return false;
}

/*
	Crea y guarda UN camino, que tendrá sus entradas y salidas pasadas por parametro.
	Le deja al usuario manejarse mediante el teclado, para moverse mientras va creando el camino.
	Terminará de crearlo cuando llegue a una salida valida.
	No se creará hasta que el camino esté completamente válido.
	El camino PUEDE cruzarse a si mismo.

	Nota: Modifico el modo de input temporalmente al obtener el caracter. Al ponerlo en modo raw para obtener el caracter,
	el usuario no necesita presionar enter para que el programa pueda leer del buffer. Hay que tener mucho cuidado con este uso
	porque es bastante frágil. Sin embargo, dado que se compilará en un entorno muy similar al mío, espero que no haya problemas,
	pues mejora bastante la usabilidad de la función.
*/
void crear_camino_nivel(coordenada_t camino[MAX_CAMINOS], int* tope_camino, int entrada, int salida, int tamanio_nivel, int nivel_actual, int numero_camino){
	int c = -1; // input del usuario
	bool camino_terminado = false;
	*tope_camino = 1;

	imprimir_camino(camino, 0, tamanio_nivel);
	c_print("CAMINO %i DE NIVEL %i \n", numero_camino, nivel_actual);
	coordenada_t pos_actual = obtener_coordenada_entrada(entrada, tamanio_nivel);

	while(c != CANCELAR && !camino_terminado){
		camino[(*tope_camino) - 1] = pos_actual;
		imprimir_camino(camino, *tope_camino, tamanio_nivel);
		system ("/bin/stty raw");
		c = getchar();
		system ("/bin/stty cooked");
		switch(c){
			case ARRIBA: {
				coordenada_t nueva_pos = (coordenada_t){pos_actual.fil - 1, pos_actual.col};
				if(puede_hacer_movimiento(nueva_pos, camino[(*tope_camino)-2], *tope_camino, tamanio_nivel)){
					pos_actual = nueva_pos;
					(*tope_camino)++;
				}
				break;
			}
			case ABAJO: {
				coordenada_t nueva_pos = (coordenada_t){pos_actual.fil + 1, pos_actual.col};
				if(puede_hacer_movimiento(nueva_pos, camino[(*tope_camino)-2], *tope_camino, tamanio_nivel)){
					pos_actual = nueva_pos;
					(*tope_camino)++;
				}
				break;
			}
			case IZQUIERDA: {
				coordenada_t nueva_pos = (coordenada_t){pos_actual.fil, pos_actual.col - 1};
				if(puede_hacer_movimiento(nueva_pos, camino[(*tope_camino)-2], *tope_camino, tamanio_nivel)){
					pos_actual = nueva_pos;
					(*tope_camino)++;
				}
				break;
			}
			case DERECHA: {
				coordenada_t nueva_pos = (coordenada_t){pos_actual.fil, pos_actual.col + 1};
				if(puede_hacer_movimiento(nueva_pos, camino[(*tope_camino)-2], *tope_camino, tamanio_nivel)){
					pos_actual = nueva_pos;
					(*tope_camino)++;
				}
				break;
			}
			case DESHACER: {
				if((*tope_camino) > 1){
					(*tope_camino)--;
					pos_actual = camino[(*tope_camino) - 1];
				}
				break;
			}
			default: break;
		}
		camino_terminado = camino_completado(pos_actual, tamanio_nivel, salida);
	}

	if(c == CANCELAR){
		printf("Creación de caminos cancelada exitosamente.\n");
		exit(SUCCESS);
	}

	camino[(*tope_camino) - 1] = pos_actual;
	imprimir_camino(camino, *tope_camino, tamanio_nivel);
}

/*
	Crea los caminos (1 o 2) del nivel pasado por parametros y los guarda en caminos_nivel.
*/
void crear_caminos_nivel(caminos_t* caminos_nivel, int nivel_actual){
	caminos_nivel -> tope_camino_1 = 0;
	caminos_nivel -> tope_camino_2 = 0;

	switch(nivel_actual){
		case NIVEL_1: {
				crear_camino_nivel(caminos_nivel -> camino_1, &(caminos_nivel -> tope_camino_1), ESTE, OESTE, TAMANO_NIVEL_1, nivel_actual, 1);
				break;
			}
		case NIVEL_2: {
				crear_camino_nivel(caminos_nivel -> camino_1,  &(caminos_nivel -> tope_camino_1), OESTE, ESTE, TAMANO_NIVEL_2, nivel_actual, 1);
				break;
			}
		case NIVEL_3: {
				crear_camino_nivel(caminos_nivel -> camino_1,  &(caminos_nivel -> tope_camino_1), NORTE, SUR, TAMANO_NIVEL_3, nivel_actual, 1);
				crear_camino_nivel(caminos_nivel -> camino_2,  &(caminos_nivel -> tope_camino_2), NORTE, SUR, TAMANO_NIVEL_3, nivel_actual, 2);
				break;
			}
		case NIVEL_4: {
				crear_camino_nivel(caminos_nivel -> camino_1,  &(caminos_nivel -> tope_camino_1), SUR, NORTE, TAMANO_NIVEL_4, nivel_actual, 1);
				crear_camino_nivel(caminos_nivel -> camino_2,  &(caminos_nivel -> tope_camino_2), SUR, NORTE, TAMANO_NIVEL_4, nivel_actual, 2);
				break;
			}
		default: break;
	}
}

/*
	Guarda los valores de la configuracion en el registro pasado por parámetros.
	No importa el orden de las lineas, pero si el de los valores en las líneas.
*/
void guardar_config_en_registro(FILE* archivo, configuracion_t* config){
	int hay_config;
	char argumento[MAX_ARGUMENTO];
	while((hay_config = fscanf(archivo, "%[^=]=", argumento)) > 0){
		if(strcmp(argumento, CONFIG_PARAM_RESISTENCIA_TORRES) == 0){
			fscanf(archivo, "%i,%i\n", &(config -> resistencia_torre_1), &(config -> resistencia_torre_2));
		} else if(strcmp(argumento, CONFIG_PARAM_ENANOS_INICIO) == 0) {
			for(int i = NIVEL_1; i <= MAX_NIVEL; i++){
				fscanf(archivo, "%i", &(config -> enanos_inicio[i-1]));
				if(i < MAX_NIVEL)
					fscanf(archivo, ",");
			}
			fscanf(archivo, "\n");
		} else if(strcmp(argumento, CONFIG_PARAM_ELFOS_INICIO) == 0) {
			for(int i = NIVEL_1; i <= MAX_NIVEL; i++){
				fscanf(archivo, "%i", &(config -> elfos_inicio[i-1]));
				if(i < MAX_NIVEL)
					fscanf(archivo, ",");
			}
			fscanf(archivo, "\n");
		} else if(strcmp(argumento, CONFIG_PARAM_ENANOS_EXTRA) == 0){
			fscanf(archivo, "%i,%i,%i\n", &(config -> enanos_extra), &(config -> costo_torre_1_enano), &(config -> costo_torre_2_enano));
		} else if(strcmp(argumento, CONFIG_PARAM_ELFOS_EXTRA) == 0){
			fscanf(archivo, "%i,%i,%i\n", &(config -> elfos_extra), &(config -> costo_torre_1_elfo), &(config -> costo_torre_2_elfo));
		} else if(strcmp(argumento, CONFIG_PARAM_ENANOS_ANIMO) == 0){
			fscanf(archivo, "%i,%i\n", &(config -> fallo_enanos), &(config -> critico_enanos));
		} else if(strcmp(argumento, CONFIG_PARAM_ELFOS_ANIMO) == 0){
			fscanf(archivo, "%i,%i\n", &(config -> fallo_elfos), &(config -> critico_elfos));
		} else if(strcmp(argumento, CONFIG_PARAM_VELOCIDAD) == 0){
			fscanf(archivo, "%f\n", &(config -> velocidad));
		} else if(strcmp(argumento, CONFIG_PARAM_CAMINOS) == 0){
			fscanf(archivo, "%s\n", config -> ruta_caminos);
		} 
	}
}


/*
	Valida que la ruta este bien formateada: Sin espacios, con la extension correcta de configuracion y que no se pase del limite de caracteres.
*/
bool validar_ruta(const char* input, char* extension){
	return strlen(input) >= 1 && strlen(input) <= MAX_RUTA && !strchr(input, ' ') && strstr(input, extension);
}


/*	
	Pide un valor para la configuracion de tipo entero valido.
	Debe cumplir: que sea numero entero y que este en el rango.
*/
void obtener_valor_config_opcional_entero(int* valor, int min, int max){
    printf(" (%i a %i): ", min, max);
	char input[MAX_ARGUMENTO];
    fgets(input, MAX_ARGUMENTO, stdin);
    if(strlen(input) == 1 && input[0] == '\n'){
        *valor = -1;
    } else {
    	while(!(atoi(input) >= min && atoi(input) <= max)){
    		printf("Asegurate que el valor sea numero entero y este entre %i y %i: ", min, max);
    		fgets(input, MAX_ARGUMENTO, stdin);
    	}
        	*valor = atoi(input);
    }
    printf("\n");
}

/*	
	Pide un valor para la configuracion de tipo float valido.
	Debe cumplir: que sea numero float y que este en el rango.
*/
void obtener_valor_config_opcional_float(float* valor, float min, float max){
	printf(" (%.1f a %.1f): ", min, max);
	char input[MAX_ARGUMENTO];
    fgets(input, MAX_ARGUMENTO, stdin);
    if(strlen(input) == 1 && input[0] == '\n'){
        *valor = -1;
    } else {
    	while((float)atof(input) < min || (float)atof(input) > max){
    		printf("Asegurate que el valor sea numero entre %.1f y %.1f: ", min, max);
    		fgets(input, MAX_ARGUMENTO, stdin);
    	}
    	printf("%s-", input);
    	printf("%f", (float)atof(input));
        *valor = (float)atof(input);
    }
    printf("\n");
}

/*
	Pide un valor de tipo string opcional para la config.
	DDebe cumplir la funcion validar_ruta.
*/
void obtener_valor_config_opcional_string(char valor[MAX_ARGUMENTO]){
	char input[MAX_ARGUMENTO];
    fgets(input, MAX_ARGUMENTO, stdin);
    if(strlen(input) == 1 && input[0] == '\n'){
        strcpy(valor, "-1");
    } else {
    	while(!validar_ruta(input, EXTENSION_CAMINOS)){
    		printf("Asegurate de ingresar una ruta válida: ");
    		fgets(input, MAX_ARGUMENTO, stdin);
    	}
        strcpy(valor, input);
    }
    printf("\n");
}

/*
	--------------------------------------------------------------------------------------------------------------------------------------
	Las siguientes funciones estan documentadas en su respectivo headerfile(servicio_configuracion.h), dado que son expuestas publicamente.
	--------------------------------------------------------------------------------------------------------------------------------------
*/


void guardar_puntaje(juego_t juego, char ruta_ranking[MAX_RUTA], int puntaje_nuevo){
	char nombre_nuevo[MAX_NOMBRE];
	c_print("Escribí tu nombre asi te identificas en el ranking: ");
	scanf("%s", nombre_nuevo);


	FILE* archivo_ranking = fopen(ruta_ranking, "r");
	FILE* nuevo_archivo_ranking = fopen(RANKING_AUX, "w");

	if(!nuevo_archivo_ranking){
		c_print(ACCESO_DENEGADO(ACCION_FRACASO));
	}
	if(!archivo_ranking){
		fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_nuevo, puntaje_nuevo);
	} else {
		bool guardo_nuevo_usuario = false;
		int puntaje_usuario_actual = 0;
		char nombre_usuario_actual[MAX_NOMBRE];
		int cantidad_elementos_leidos = fscanf(archivo_ranking, "%[^;];%i\n", nombre_usuario_actual, &puntaje_usuario_actual);

		while(cantidad_elementos_leidos == ELEMENTOS_LINEA_RANKING){
			if(puntaje_nuevo > puntaje_usuario_actual && !guardo_nuevo_usuario){
				fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_nuevo, puntaje_nuevo);
				guardo_nuevo_usuario = true;
				fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_usuario_actual, puntaje_usuario_actual);
			} else if(puntaje_nuevo == puntaje_usuario_actual && !guardo_nuevo_usuario){
				if(strcmp(nombre_nuevo, nombre_usuario_actual) < 0){
					fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_nuevo, puntaje_nuevo);
					fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_usuario_actual, puntaje_usuario_actual);
				} else {
					fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_usuario_actual, puntaje_usuario_actual);
					fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_nuevo, puntaje_nuevo);
				}
				guardo_nuevo_usuario = true;
			} else {
				fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_usuario_actual, puntaje_usuario_actual);			
			}
			cantidad_elementos_leidos = fscanf(archivo_ranking, "%[^;];%i\n", nombre_usuario_actual, &puntaje_usuario_actual);
		}

		if(!guardo_nuevo_usuario){
			fprintf(nuevo_archivo_ranking, "%s;%i\n", nombre_nuevo, puntaje_nuevo);
		}
	}
	if(archivo_ranking){
		fclose(archivo_ranking);
	}
	c_print("Genial, ya podes hecharle la vista a ver que tan bien te fue ;)\n");
	fclose(nuevo_archivo_ranking);
	rename(RANKING_AUX, ruta_ranking);
}

void ranking(int argc, char const *argv[]){
	int i = PRIMER_ARGUMENTO, cant_listados = DEFAULT_NUM;
	FILE* archivo_ranking = NULL;

	while(i < argc){
		if(strstr(argv[i], LISTAR)){
			char cantidad[MAX_ARGUMENTO];
			cargar_valor_de_argumento(cantidad, argv[i]);
			cant_listados = atoi(cantidad);

		} else if(strstr(argv[i], CONFIG)){
			char ruta_config[MAX_ARGUMENTO];
			cargar_valor_de_argumento(ruta_config, argv[i]);
			char ruta_ranking[MAX_RUTA];
			transformar_ruta_config_a_ranking(ruta_config,ruta_ranking);
			archivo_ranking = abrir_archivo(ruta_ranking, "r", SIN_RANKING_CONFIG_ERROR);
		}
		i++;
	}

	if(!archivo_ranking){
		archivo_ranking = abrir_archivo(RANKING_DEFAULT, "r", SIN_RANKING_ERROR);
	}

	char nombre[MAX_ARGUMENTO], puntaje[MAX_ARGUMENTO];
	int cantidad_lecturas = 0;

	int cantidad_elementos_leidos = fscanf(archivo_ranking, FORMATO_RANKING, nombre, puntaje);

	while(cantidad_elementos_leidos == ELEMENTOS_LINEA_RANKING && (cant_listados == DEFAULT_NUM || cantidad_lecturas < cant_listados)){
		printf("%i: %s con %s puntos.\n", ++cantidad_lecturas, nombre, puntaje);
		cantidad_elementos_leidos = fscanf(archivo_ranking, FORMATO_RANKING, nombre, puntaje);
	}

	fclose(archivo_ranking);
}

void crear_camino(int argc, char const *argv[]){
	FILE* archivo_caminos;
	caminos_t caminos[MAX_NIVEL];
	int nivel_actual = NIVEL_1;
	int i = PRIMER_ARGUMENTO;
	char ruta_caminos[MAX_RUTA];

	if(i < argc){
		strcpy(ruta_caminos, argv[i]);
		if(!validar_ruta(ruta_caminos, EXTENSION_CAMINOS)){
			printf(VERIFICAR_RUTA);
			exit(FAILURE);
		}
		while(nivel_actual <= MAX_NIVEL){
			crear_caminos_nivel(&(caminos[nivel_actual - 1]), nivel_actual);
			nivel_actual++;
		}
	} else {
		printf(FALTA_ARG_OBLIGATORIO(nombre del archivo de caminos));
		exit(FAILURE);
	}
	
	archivo_caminos = abrir_archivo(ruta_caminos, "w", ACCESO_DENEGADO(ruta_caminos));

	for(int i = 0; i < MAX_NIVEL; i++){
		fprintf(archivo_caminos, FORMATO_NIVEL "%i\n", i + 1);
		fprintf(archivo_caminos, FORMATO_CAMINO "%i\n", CAMINO_1);

		for(int j = 0; j < caminos[i].tope_camino_1; j++){
			fprintf(archivo_caminos, "%i;%i\n", caminos[i].camino_1[j].fil, caminos[i].camino_1[j].col);
		}

		if(caminos[i].tope_camino_2 > 0) {
			fprintf(archivo_caminos, FORMATO_CAMINO "%i\n", CAMINO_2);
			for(int j = 0; j < caminos[i].tope_camino_2; j++){
				fprintf(archivo_caminos, "%i;%i\n", caminos[i].camino_2[j].fil, caminos[i].camino_2[j].col);
			}
		}
	}
	c_print("Caminos creados exitosamente!\n");
	fclose(archivo_caminos);
}


void crear_configuracion(int argc, char const *argv[]){
	FILE* archivo_config;
	configuracion_t config;
	int i = PRIMER_ARGUMENTO;

	if(i < argc){
		if(!validar_ruta(argv[i], EXTENSION_CONFIG)){
			printf(VERIFICAR_RUTA);
			exit(FAILURE);
		}
		printf("Creación de configuración (si queres dejar el valor por default, solo presioná enter):\n");
		printf("%s", "Ingrese los puntos de vida de la torre 1");
		obtener_valor_config_opcional_entero(&(config.resistencia_torre_1), 1, MAX_VIDA_TORRE);
		printf("%s", "Ingrese los puntos de vida de la torre 2");
		obtener_valor_config_opcional_entero(&(config.resistencia_torre_2), 1, MAX_VIDA_TORRE);
		for(int i = NIVEL_1; i <= MAX_NIVEL; i++){
			printf("%s %i", "Ingrese la cantidad de enanos en el nivel ", i);
			obtener_valor_config_opcional_entero(&(config.enanos_inicio[i-1]), 0, MAX_DEFENSORES_INICIALES);
			printf("%s %i", "Ingrese la cantidad de elfos en el nivel ", i);
			obtener_valor_config_opcional_entero(&(config.elfos_inicio[i-1]), 0, MAX_DEFENSORES_INICIALES);
		}
		printf("%s", "Ingrese la cantidad de enanos extra");
		obtener_valor_config_opcional_entero(&(config.enanos_extra), 0, MAX_DEFENSORES_EXTRAS);
		printf("%s", "Ingrese cuanta resistencia le costara agregar un enano a la torre 1");
		obtener_valor_config_opcional_entero(&(config.costo_torre_1_enano), 0, MAX_COSTO_TORRE_POR_DEFENSOR);
		printf("%s", "Ingrese cuanta resistencia le costara agregar un enano a la torre 2");
		obtener_valor_config_opcional_entero(&(config.costo_torre_2_enano), 0, MAX_COSTO_TORRE_POR_DEFENSOR);
		printf("%s", "Ingrese la cantidad de elfos extra");
		obtener_valor_config_opcional_entero(&(config.elfos_extra), 0, MAX_DEFENSORES_EXTRAS);
		printf("%s", "Ingrese cuanta resistencia le costara agregar un elfo a la torre 1");
		obtener_valor_config_opcional_entero(&(config.costo_torre_1_elfo), 0, MAX_COSTO_TORRE_POR_DEFENSOR);
		printf("%s", "Ingrese cuanta resistencia le costara agregar un elfo a la torre 2");
		obtener_valor_config_opcional_entero(&(config.costo_torre_2_elfo), 0, MAX_COSTO_TORRE_POR_DEFENSOR);
		printf("%s", "Ingrese el daño crítico de los enanos");
		obtener_valor_config_opcional_entero(&(config.critico_enanos), 0, MAX_CRITICO);
		printf("%s", "Ingrese el daño crítico de los elfos");
		obtener_valor_config_opcional_entero(&(config.critico_elfos), 0, MAX_CRITICO);
		printf("%s", "Ingrese el fallo de los enanos");
		obtener_valor_config_opcional_entero(&(config.fallo_enanos), 0, MAX_FALLO);
		printf("%s", "Ingrese el fallo de los elfos");
		obtener_valor_config_opcional_entero(&(config.fallo_elfos), 0, MAX_FALLO);
		printf("%s", "Ingrese la velocidad del juego");
		obtener_valor_config_opcional_float(&(config.velocidad), 0, MAX_VELOCIDAD);
		printf("%s", "Ingrese la ruta del archivo de caminos: ");
		obtener_valor_config_opcional_string(config.ruta_caminos);
	} else {
		printf(FALTA_ARG_OBLIGATORIO(nombre de configuracion));
		exit(FAILURE);
	}

	archivo_config = abrir_archivo(argv[i], "w", ACCESO_DENEGADO(argv[i]));
	fprintf(archivo_config, CONFIG_PARAM_RESISTENCIA_TORRES "=%i,%i\n", config.resistencia_torre_1, config.resistencia_torre_2);
	fprintf(archivo_config, CONFIG_PARAM_ENANOS_INICIO "=");
	for(int i = NIVEL_1; i <= MAX_NIVEL; i++){
		fprintf(archivo_config, "%i", config.enanos_inicio[i-1]);
		if(i < MAX_NIVEL)
			fprintf(archivo_config, ",");
	}
	fprintf(archivo_config, "\n");
	fprintf(archivo_config, CONFIG_PARAM_ELFOS_INICIO "=");
	for(int i = NIVEL_1; i <= MAX_NIVEL; i++){
		fprintf(archivo_config, "%i", config.elfos_inicio[i-1]);
		if(i < MAX_NIVEL)
			fprintf(archivo_config, ",");
	}
	fprintf(archivo_config, "\n");
	fprintf(archivo_config, CONFIG_PARAM_ENANOS_EXTRA "=%i,%i,%i\n", config.enanos_extra, config.costo_torre_1_enano, config.costo_torre_2_enano);
	fprintf(archivo_config, CONFIG_PARAM_ELFOS_EXTRA "=%i,%i,%i\n", config.elfos_extra, config.costo_torre_1_elfo, config.costo_torre_2_elfo);
	fprintf(archivo_config, CONFIG_PARAM_ENANOS_ANIMO "=%i,%i\n", config.fallo_enanos, config.critico_enanos);
	fprintf(archivo_config, CONFIG_PARAM_ELFOS_ANIMO"=%i,%i\n", config.fallo_elfos, config.critico_elfos);
	fprintf(archivo_config, CONFIG_PARAM_VELOCIDAD "=%.1f\n", config.velocidad);
	fprintf(archivo_config, CONFIG_PARAM_CAMINOS "=%s\n", config.ruta_caminos);
	fclose(archivo_config);
}

void poneme_la_repe(int argc, char const *argv[]){
	juego_t juego;
	FILE* archivo_grabacion;
	float velocidad = VELOCIDAD_DEFAULT;

	int i = PRIMER_ARGUMENTO;
	while(i < argc){
		if(strstr(argv[i], VELOCIDAD)){
			char arg[MAX_ARGUMENTO];
			cargar_valor_de_argumento(arg, argv[i]);
			velocidad = (float)atof(arg);
		} else if(strstr(argv[i], GRABACION)){
			char ruta[MAX_ARGUMENTO];
			cargar_valor_de_argumento(ruta, argv[i]);
			archivo_grabacion = abrir_archivo(ruta, "r", SIN_GRABACION_ERROR);
		} 
		i++;
	}

	if(!archivo_grabacion) {
		printf(FALTA_ARG_OBLIGATORIO(grabacion));
		exit(FAILURE);
	}

	fread(&juego, sizeof(juego_t), CANTIDAD_LECTURAS_JUEGO, archivo_grabacion);
	while(!feof(archivo_grabacion)){
		mostrar_juego(juego);
		detener_el_tiempo(velocidad);
		fread(&juego, sizeof(juego_t), CANTIDAD_LECTURAS_JUEGO, archivo_grabacion);
	}

	fclose(archivo_grabacion);
}

void jugar(int argc, char const *argv[]){
	char ruta_grabacion[MAX_RUTA] = REPE_DEFAULT;
	char ruta_config[MAX_ARGUMENTO] = CONFIG_DEFAULT;
	configuracion_t config;
	FILE *archivo_config = NULL;

	int i = PRIMER_ARGUMENTO;
	while(i < argc){
	 	if(strstr(argv[i], CONFIG)){
			cargar_valor_de_argumento(ruta_config, argv[i]);
			archivo_config = abrir_archivo(ruta_config, "r", SIN_CONFIG_ERROR);
			guardar_config_en_registro(archivo_config, &config);
			fclose(archivo_config);
		} else if(strstr(argv[i], GRABACION)){
			cargar_valor_de_argumento(ruta_grabacion, argv[i]);
		}
		i++;
	}

	if(!archivo_config){
		archivo_config = abrir_archivo(ruta_config, "r", SIN_CONFIG_ERROR);
		guardar_config_en_registro(archivo_config, &config);
		fclose(archivo_config);
	}
	char ruta_ranking[MAX_RUTA];
	transformar_ruta_config_a_ranking(ruta_config,ruta_ranking);
	juego(config, ruta_ranking, ruta_grabacion);
}