#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h> //para obtener dimension de la consola
#include <stdarg.h>
#include "defendiendo_torres.h"
#include "servicio_configuracion.h"
#include "utiles.h"
#include "animos.h"
#include "juego.h"

#define ELFO 'L'
#define ENANO 'G'

#define ENANOS_INICIALES_NIVEL_1 5
#define ELFOS_INICIALES_NIVEL_1 0


#define ENANOS_INICIALES_NIVEL_2 0
#define ELFOS_INICIALES_NIVEL_2 5


#define ENANOS_INICIALES_NIVEL_3 3
#define ELFOS_INICIALES_NIVEL_3 3


#define ENANOS_INICIALES_NIVEL_4 0
#define ELFOS_INICIALES_NIVEL_4 0

#define DANIO_A_TORRE_POR_ENANO 50
#define DANIO_A_TORRE_POR_ELFO 50

#define CANTIDAD_ENEMIGOS_NIVEL_1 100
#define CANTIDAD_ENEMIGOS_NIVEL_2 200
#define CANTIDAD_ENEMIGOS_NIVEL_3 300
#define CANTIDAD_ENEMIGOS_NIVEL_4 500

#define VICTORIA 1
#define DERROTA -1
#define JUGANDO 0
#define INVALIDO -1
#define ACEPTA 's'

#define CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_1 25
#define CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_2 50
#define CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_3 50
#define CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_4 50

#define FACTOR_ENEMIGOS_MATADOS 1000

/*
	Un custom print para escribir en la mitad de la pantalla, con delay entre las letras similar a los juegos arcade.
	Recibe los parametros de la misma manera que printf, el primer argumento es el string formato y el resto son las variables del tipo correspondiente, en orden.
*/
void c_print(char *fmt, ...){
	va_list ap;
	char *p;
	int ival;
	char* txt;
	va_start(ap, fmt);
	struct winsize w; 
    ioctl(0, TIOCGWINSZ, &w);  
	printf("%*s> ", ((w.ws_col - strlen(fmt)) / 2), " ");
	for (p = fmt; *p; p++) {
		if (*p != '%') {
			putchar(*p);
			fflush(stdout);
			if(*p == ',' || *p == '.' || *p == '!' || *p == '?'){
				detener_el_tiempo(0.03f);
			} else {
				detener_el_tiempo(0.03f);
			}
			continue;
		}
		switch (*++p) {
			case 'd':
				ival = va_arg(ap, int);
				printf("%d", ival);
				break;
			case 'i':
				ival = va_arg(ap, int);
				printf("%i", ival);
				break;
			case 'c':
				ival = va_arg(ap, int);
				printf("%c", ival);
				break;
			case 's':
				txt = va_arg(ap, char*);
				printf("%s", txt);
				break;
			default: 
				putchar(*p);
				break;
		}
	}
	detener_el_tiempo(0.03f);
}

/*
	Interactua con el usuario para que este determine algunas decisiones.
	Devolvera verdadero si el jugador aceptó la decision y falso si la rechazo.
*/
bool jugador_toma_decision(char* decision){
	char input;

	c_print(decision);
	printf("\n");
	c_print("si(s)/no(n)");
	scanf(" %c", &input);
	return input == ACEPTA;
}


/*
	Guarda la configuracion de un nivel: Entradas, torres, cantidad de enemigos total, etc.
*/
typedef struct config_nivel {
	coordenada_t posicion_torre_1;
	coordenada_t posicion_torre_2;
	coordenada_t entrada_camino_1;
	coordenada_t entrada_camino_2;
	int danio_a_torre_enano;
	int danio_a_torre_elfo;
	int max_enemigos_nivel;
	int cantidad_enemigos_por_defensor;
	int enanos_iniciales;
	int elfos_iniciales;
	int tamanio_tablero;
	int cantidad_caminos;
}  config_nivel_t;


static config_nivel_t config_nivel_1 = {
	.tamanio_tablero = TAMANO_NIVEL_1,
	.enanos_iniciales = ENANOS_INICIALES_NIVEL_1,
	.elfos_iniciales = ELFOS_INICIALES_NIVEL_1,
	.max_enemigos_nivel = CANTIDAD_ENEMIGOS_NIVEL_1,
	.danio_a_torre_enano = DANIO_A_TORRE_POR_ENANO,
	.danio_a_torre_elfo = DANIO_A_TORRE_POR_ELFO,
	.cantidad_caminos = 1,
	.cantidad_enemigos_por_defensor = CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_1,
	.entrada_camino_1 =  (coordenada_t){TAMANO_NIVEL_1 / 2, TAMANO_NIVEL_1 - 1},
	.posicion_torre_1 = (coordenada_t){TAMANO_NIVEL_1 / 2, 0},
};

static config_nivel_t config_nivel_2 = {
	.tamanio_tablero = TAMANO_NIVEL_2,
	.enanos_iniciales = ENANOS_INICIALES_NIVEL_2,
	.elfos_iniciales = ELFOS_INICIALES_NIVEL_2,
	.max_enemigos_nivel = CANTIDAD_ENEMIGOS_NIVEL_2,
	.danio_a_torre_enano = DANIO_A_TORRE_POR_ENANO,
	.danio_a_torre_elfo = DANIO_A_TORRE_POR_ELFO,
	.cantidad_caminos = 1,
	.cantidad_enemigos_por_defensor = CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_2,
	.entrada_camino_1 =  (coordenada_t){TAMANO_NIVEL_2 / 2, 0},
	.posicion_torre_1 = (coordenada_t){TAMANO_NIVEL_2 / 2, TAMANO_NIVEL_2 - 1},
};

static config_nivel_t config_nivel_3 = {
	.tamanio_tablero = TAMANO_NIVEL_3,
	.enanos_iniciales = ENANOS_INICIALES_NIVEL_3,
	.elfos_iniciales = ELFOS_INICIALES_NIVEL_3,
	.max_enemigos_nivel = CANTIDAD_ENEMIGOS_NIVEL_3,
	.cantidad_enemigos_por_defensor = CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_3,
	.danio_a_torre_enano = DANIO_A_TORRE_POR_ENANO,
	.danio_a_torre_elfo = DANIO_A_TORRE_POR_ELFO,
	.cantidad_caminos = 2,
	.entrada_camino_1 =  (coordenada_t){0, 5},
	.posicion_torre_1 = (coordenada_t){TAMANO_NIVEL_3 - 1, TAMANO_NIVEL_3 * 2 / 3},
	.entrada_camino_2 =  (coordenada_t){0, 15},
	.posicion_torre_2 = (coordenada_t){TAMANO_NIVEL_3 - 1, TAMANO_NIVEL_3 / 3},
};
static config_nivel_t config_nivel_4 = {
	.tamanio_tablero = TAMANO_NIVEL_4,
	.enanos_iniciales = ENANOS_INICIALES_NIVEL_4,
	.elfos_iniciales = ELFOS_INICIALES_NIVEL_4,
	.max_enemigos_nivel = CANTIDAD_ENEMIGOS_NIVEL_4,
	.danio_a_torre_enano = DANIO_A_TORRE_POR_ENANO,
	.danio_a_torre_elfo = DANIO_A_TORRE_POR_ELFO,
	.cantidad_caminos = 2,
	.cantidad_enemigos_por_defensor = CANTIDAD_ENEMIGOS_POR_DEFENSOR_NIVEL_4,
	.entrada_camino_1 =  (coordenada_t){TAMANO_NIVEL_4 - 1,  TAMANO_NIVEL_4 * 2 / 3},
	.posicion_torre_1 = (coordenada_t){0,  TAMANO_NIVEL_4 * 2 / 3},
	.entrada_camino_2 =  (coordenada_t){TAMANO_NIVEL_4 - 1, TAMANO_NIVEL_4 / 3},
	.posicion_torre_2 = (coordenada_t){0, TAMANO_NIVEL_4 / 3},
};

/*
	Devuelve la config del nivel actual, para luego ser accedida en otras funciones.
	El nivel tiene que estar en el rango valido.
*/
config_nivel_t* config(int nivel){
	switch(nivel){
		case NIVEL_1: return &config_nivel_1;
		case NIVEL_2: return &config_nivel_2;
		case NIVEL_3: return &config_nivel_3;
		case NIVEL_4: return &config_nivel_4;
		default: return &config_nivel_1;
	}
}

int leer_nivel(FILE* archivo, int* nivel){
	return fscanf(archivo, FORMATO_NIVEL "%i\n", nivel);
}

int leer_camino(FILE* archivo, int* camino){
	return fscanf(archivo, FORMATO_CAMINO "%i\n", camino);
}

/*
	Guarda los valores del archivo csv de caminos al registro "caminos".
	El archivo debe estar correctamente formateado y completo con todos los caminos de los niveles.
	En este de estar en default la ruta de caminos, se inicializaran los caminos de manera pseudo aleatoria, respetando entradas y salidas. 
*/

void inicializar_caminos(caminos_t caminos[MAX_NIVEL], char* ruta_caminos){
	if(strcmp(ruta_caminos, DEFAULT_STRING) == 0){	
		obtener_camino((caminos[0].camino_1), &(caminos[0].tope_camino_1), config(NIVEL_1) -> entrada_camino_1, config(NIVEL_1) -> posicion_torre_1);
		caminos[0].tope_camino_2 = 0;
		obtener_camino((caminos[1].camino_1), &(caminos[1].tope_camino_1), config(NIVEL_2) -> entrada_camino_1, config(NIVEL_2) -> posicion_torre_1);
		caminos[1].tope_camino_2 = 0;
		obtener_camino((caminos[2].camino_1), &(caminos[2].tope_camino_1), config(NIVEL_3) -> entrada_camino_1, config(NIVEL_3) -> posicion_torre_1);
		obtener_camino((caminos[2].camino_2), &(caminos[2].tope_camino_2), config(NIVEL_3) -> entrada_camino_2, config(NIVEL_3) -> posicion_torre_2);
		obtener_camino((caminos[3].camino_1), &(caminos[3].tope_camino_1), config(NIVEL_4) -> entrada_camino_1, config(NIVEL_4) -> posicion_torre_1);
		obtener_camino((caminos[3].camino_2), &(caminos[3].tope_camino_2), config(NIVEL_4) -> entrada_camino_2, config(NIVEL_4) -> posicion_torre_2);
	
	} else {
		FILE* archivo_caminos = fopen(ruta_caminos, "r");
		int nivel_actual = NIVEL_1;
		int cantidad_leidos_nivel = fscanf(archivo_caminos, FORMATO_NIVEL "%i\n", &nivel_actual);
		while(cantidad_leidos_nivel == ELEMENTOS_LINEA_NIVEL){
			int camino;
			int canidad_leidos_camino = fscanf(archivo_caminos, FORMATO_CAMINO "%i\n", &camino);
			while(canidad_leidos_camino == ELEMENTOS_LINEA_CAMINO){
				coordenada_t coordenada_camino;
				int tope_camino_1 = 0, tope_camino_2 = 0;
				int cantidad_leidos_coordenada = fscanf(archivo_caminos, FORMATO_COORDENADA, &(coordenada_camino.fil), &(coordenada_camino.col));
				while(cantidad_leidos_coordenada == ELEMENTOS_LINEA_COORDENADA){
					if(camino == 1){
						caminos[nivel_actual - 1].camino_1[tope_camino_1++] = coordenada_camino;
						caminos[nivel_actual - 1].tope_camino_1 = tope_camino_1;

					} else if(camino == 2){
						caminos[nivel_actual - 1].camino_2[tope_camino_2++] = coordenada_camino;
						caminos[nivel_actual - 1].tope_camino_2 = tope_camino_2;
					}

					cantidad_leidos_coordenada = fscanf(archivo_caminos, FORMATO_COORDENADA, &(coordenada_camino.fil), &(coordenada_camino.col));
				}
				canidad_leidos_camino = fscanf(archivo_caminos, FORMATO_CAMINO "%i\n", &camino);
			}
			cantidad_leidos_nivel = fscanf(archivo_caminos, FORMATO_NIVEL "%i\n", &nivel_actual);
		}
	}
}

/*
	Resetea los enemigos, defensores y los caminos.
	El nivel tiene que estar en el rango valido.
*/
void inicializar_nivel(nivel_t* nivel, int nivel_actual, caminos_t caminos[MAX_NIVEL]){
	nivel -> tope_enemigos = 0;
	nivel -> tope_defensores = 0;
	nivel -> max_enemigos_nivel = config(nivel_actual) -> max_enemigos_nivel;
	for(int i = 0; i < caminos[nivel_actual - 1].tope_camino_1; i++){
		nivel -> camino_1[i] = caminos[nivel_actual - 1].camino_1[i];
		nivel -> tope_camino_1 = caminos[nivel_actual - 1].tope_camino_1;
	}
	for(int i = 0; i < caminos[nivel_actual - 1].tope_camino_2; i++){
		nivel -> camino_2[i] = caminos[nivel_actual - 1].camino_2[i];
		nivel -> tope_camino_2 = caminos[nivel_actual - 1].tope_camino_2;
	}
}

/*
	Verifica si se tiene enanos en las torres como para agregar un extra ademas de si la torre 1 cuenta con suficiente vida como para no morir al agregarlo.
*/
bool puede_agregar_enano(torres_t torres){
	return (torres.enanos_extra > 0) && (torres.resistencia_torre_1 > DANIO_A_TORRE_POR_ENANO);
}

/*
	Verifica si se tiene elfos en las torres como para agregar un extra ademas de si la torre 2 cuenta con suficiente vida como para no morir al agregarlo.
*/
bool puede_agregar_elfo(torres_t torres){
	return (torres.elfos_extra > 0) && (torres.resistencia_torre_2 > DANIO_A_TORRE_POR_ELFO);
}

/*
	Verifica si puede agregar un defensor extra del tipo pasado por parametro.
	Devolvera verdadero en caso de tener suficientes defensores y vida en torre disponible.
*/
bool puede_agregar_defensor_extra_de_tipo(torres_t torres, char tipo){
	switch(tipo){
		case ENANO:
			return puede_agregar_enano(torres);
		case ELFO:
			return puede_agregar_elfo(torres);
		default:
			return false;
	}
}

/*
	Vertifica si puede agregar un defensor de al menos algún tipo.
*/
bool puede_agregar_algun_defensor(torres_t torres){
	return puede_agregar_defensor_extra_de_tipo(torres, ENANO) || puede_agregar_defensor_extra_de_tipo(torres, ELFO);
}

/*
	Verifica que el defensor sea del tipo posible.
*/
bool es_tipo_valido(char tipo){
	return tipo == ENANO || tipo == ELFO;
}

/*
	Verifica que el componente x o y de la coordenada este en el rango del mapa del nivel pasado.
*/
bool componente_coordenada_en_rango(int componente_coordenada, int nivel_actual){
	return (componente_coordenada >= 0 && componente_coordenada <= config(nivel_actual) -> tamanio_tablero - 1);
}

/*
	Verifica si puede agregar un defensor extra en el caso de que hayan salido los suficientes enemigos y las torres tengan la vida suficiente.
*/
bool puede_agregar_defensor_extra(int nivel_actual, int cantidad_enemigos_actual, torres_t torres, nivel_t nivel){
	static bool ultimo_defensor_agregado = false;
	static int nuevo_nivel = NIVEL_1;
	if(nuevo_nivel != nivel_actual){
		ultimo_defensor_agregado = false;
		nuevo_nivel = nivel_actual;
	}

	bool cantidad_enemigos_suficiente = false;
	if(!ultimo_defensor_agregado && cantidad_enemigos_actual > 0){
		cantidad_enemigos_suficiente = cantidad_enemigos_actual % config(nivel_actual) -> cantidad_enemigos_por_defensor == 0;
		ultimo_defensor_agregado = nivel.max_enemigos_nivel == cantidad_enemigos_actual;
	}

	return puede_agregar_algun_defensor(torres) && cantidad_enemigos_suficiente;
}

/*
	Hace ingresar al usuario un tipo valida de defensor en base a los que tiene disponibles.
*/
void obtener_tipo_defensor(char* tipo){
	scanf(" %c", tipo);
	while(!es_tipo_valido(*tipo)) {
		c_print("Ups! recuerda, solo puedes agregar ENANOS(%c) o ELFOS(%c): \n", ENANO, ELFO);
		scanf(" %c", tipo);
	}
}

/*
	Hace ingresar al usuario una coordenada que esté en los rangos validos del mapa.
*/
void obtener_coordenada_en_rango(coordenada_t* posicion, int nivel_actual){
	c_print("Ingresa la fila donde desea agregar el nuevo defensor.\n");
	scanf("%i", &(posicion -> fil));
	while(!componente_coordenada_en_rango(posicion -> fil, nivel_actual)) {
		c_print("Ups! Asegurese de que la fila este en el rango válido: %d a %d\n", 0, config(nivel_actual) -> tamanio_tablero - 1);
		scanf("%i", &(posicion -> fil));
	}

	c_print("Ingresa la columna donde desea agregar el nuevo defensor.\n");
	scanf("%i", &(posicion -> col));
	while(!componente_coordenada_en_rango(posicion -> col, nivel_actual)) {
		c_print("Ups! Asegurese de que la columna este en el rango válido: %d a %d\n", 0, config(nivel_actual) -> tamanio_tablero - 1);
		scanf("%i", &(posicion -> col));
	}
}

/*
	Hace ingresar al usuario una coordenada valida donde colocará al defensor del tipo pasado.
	Se le pedirá cuantas veces sea necesario que re ingrese la coordenada hasta que cumpla con los requisitos.
*/
void ingresar_defensor_con_tipo(nivel_t *nivel, int nivel_actual, int tope, char tipo){
	coordenada_t posicion;
	obtener_coordenada_en_rango(&posicion, nivel_actual);
	int pudo_agregar_defensor = agregar_defensor(nivel, posicion, tipo);

	while(pudo_agregar_defensor == INVALIDO) {
		c_print("Ups, esa posicion esta ocupada o pertenece al camino, ingrese otra coordenada: \n");
		obtener_coordenada_en_rango(&posicion, nivel_actual);
		pudo_agregar_defensor = agregar_defensor(nivel, posicion, tipo);
	}
}

/*
	Cargará en en el vector de defensores los tipos de defensores habilitados.
	Llenara al vector defensores_disponibles con los caracteres que identifican a los defensores.
	En la posicion 0 al ENANO, si es que esta disponible.
	En la posicion 1 al ELFO, si es que esta disponible.
*/
void cargar_defensores_disponibles(char defensores_disponibles[MAX_ARGUMENTO], torres_t torres, int nivel_actual){
	defensores_disponibles[0] = '\0';
	if(puede_agregar_defensor_extra_de_tipo(torres, ENANO))
		strcat(defensores_disponibles, "ENANO");
	if(puede_agregar_defensor_extra_de_tipo(torres, ELFO)){
		if(defensores_disponibles[0] != '\0'){
			strcat(defensores_disponibles, " o ");
		} 
		strcat(defensores_disponibles, "ELFO");
	}
}

/*
	Agregará un defensor extra, dejandole elegir el tipo al usuario unicamente si tiene a ambos disponibles.
	Le hará el el daño a la torre que tiene el tipo de defensor agregado.
*/
void agregar_defensor_extra(nivel_t* nivel, int nivel_actual, torres_t* torres, juego_t juego, configuracion_t global_config){
	char tipo_agregado;
	if(juego.torres.enanos_extra > 0 && juego.torres.elfos_extra == 0){
		c_print("Agergarás un defensor de tipo ENANO(%c): \n", ENANO);
		ingresar_defensor_con_tipo(nivel, nivel_actual, config(nivel_actual) -> tamanio_tablero, ENANO);
		tipo_agregado = ENANO;
	} else if(juego.torres.enanos_extra == 0 && juego.torres.elfos_extra > 0){
		c_print("Agergarás un defensor de tipo ELFO(%c):\n", ELFO);
		ingresar_defensor_con_tipo(nivel, nivel_actual, config(nivel_actual) -> tamanio_tablero, ELFO);
		tipo_agregado = ELFO;
	} else {
		c_print("Elige el tipo de defensor ENANO(%c) o ELFO(%c): \n", ENANO, ELFO);
		obtener_tipo_defensor(&tipo_agregado);
		ingresar_defensor_con_tipo(nivel, nivel_actual, config(nivel_actual) -> tamanio_tablero, tipo_agregado);
	}

	if(tipo_agregado == ENANO){
		torres -> resistencia_torre_1 -= global_config.costo_torre_1_enano;
		torres -> resistencia_torre_1 -= global_config.costo_torre_2_enano;
		torres -> enanos_extra -= 1;
	} else if(tipo_agregado == ELFO){
		torres -> resistencia_torre_1 -= global_config.costo_torre_1_elfo;
		torres -> resistencia_torre_1 -= global_config.costo_torre_2_elfo;
		torres -> elfos_extra -= 1;
	}
}

/*
	Si salieron los suficentes enemigos, Agregará el defensor del tipo que sea permitido por el nivel, si el usuario lo desea.
*/
void agregado_de_defensores_extra(juego_t* juego, configuracion_t global_config) {
	if(puede_agregar_defensor_extra(juego -> nivel_actual, juego -> nivel.tope_enemigos, juego -> torres, juego -> nivel)){
		char defensores_disponibles[MAX_ARGUMENTO];
		cargar_defensores_disponibles(defensores_disponibles, juego -> torres, juego -> nivel_actual);
		c_print("puedes agregar defensor de tipo %s.\n", defensores_disponibles);
		if(jugador_toma_decision("Quieres agregar un defensor extra?")){
			agregar_defensor_extra(&(juego -> nivel), juego -> nivel_actual, &(juego -> torres), *juego, global_config);
			c_print("Defensor agregado correctamente!\n");
		}
	}
}

/*
	El usuario Agregará al mapa los defensores permitidos por el nivel actual.
*/
void agregar_defensores_iniciales(juego_t* juego){
	c_print("Agrega a tus defensores tacticamente para que los enemigos no lleguen a las torres!\n");
	if(config(juego -> nivel_actual) -> enanos_iniciales > 0){
		c_print("A continuación, agregaras %d ENANOS(%c).\n", config(juego -> nivel_actual) -> enanos_iniciales, ENANO);
		for(int i = 0; i < config(juego -> nivel_actual) -> enanos_iniciales; i++){
			ingresar_defensor_con_tipo(&(juego -> nivel), juego -> nivel_actual, config(juego -> nivel_actual) -> tamanio_tablero, ENANO);
			c_print("Defensor agregado correctamente!\n");
			mostrar_juego(*juego);
		}
	}

	if(config(juego -> nivel_actual) -> elfos_iniciales > 0){
		c_print("A continuación, agregaras %d ELFOS(%c).\n", config(juego -> nivel_actual) -> elfos_iniciales, ELFO);
		for(int i = 0; i < config(juego -> nivel_actual) -> elfos_iniciales; i++){
			ingresar_defensor_con_tipo(&(juego -> nivel), juego -> nivel_actual, config(juego -> nivel_actual) -> tamanio_tablero, ELFO);
			c_print("Defensor agregado correctamente!\n");
			mostrar_juego(*juego);
		}
	}

	c_print("Veo que sabes lo que haces! Praparate, la batalla está por empezar...\n");
}

void setear_defensores_iniciales(configuracion_t configuracion){
	for(int i = 1; i <= MAX_NIVEL; i++){
		config(i) -> enanos_iniciales = configuracion.enanos_inicio[i-1] == DEFAULT_NUM ? config(i) -> enanos_iniciales : configuracion.enanos_inicio[i-1];
		config(i) -> elfos_iniciales = configuracion.elfos_inicio[i-1] == DEFAULT_NUM ? config(i) -> elfos_iniciales : configuracion.elfos_inicio[i-1];
	}
}

int puntaje(juego_t juego, configuracion_t configuracion){
	int cantidad_enemigos_matados = 0;
	int cantidad_enanos_iniciales_total = 0;
	int cantidad_elfos_iniciales_total = 0;
	int i = 0;
	while(++i < juego.nivel_actual){
		cantidad_enemigos_matados += config(juego.nivel_actual) -> max_enemigos_nivel;
		cantidad_enanos_iniciales_total += configuracion.enanos_inicio[i-1] == DEFAULT_NUM ? config(i) -> enanos_iniciales : configuracion.enanos_inicio[i-1];
		cantidad_elfos_iniciales_total += configuracion.elfos_inicio[i-1] == DEFAULT_NUM ? config(i) -> elfos_iniciales : configuracion.elfos_inicio[i-1];
	}
	cantidad_enemigos_matados += juego.nivel.tope_enemigos;

	return (cantidad_enemigos_matados * FACTOR_ENEMIGOS_MATADOS) / 
		   ((configuracion.resistencia_torre_1 == DEFAULT_NUM ? RESISTENCIA_TORRE : configuracion.resistencia_torre_1) + 
		   (configuracion.resistencia_torre_2 == DEFAULT_NUM ? RESISTENCIA_TORRE : configuracion.resistencia_torre_2) + 
		   (cantidad_enanos_iniciales_total) +
		   (cantidad_elfos_iniciales_total) + 
		   (configuracion.enanos_extra == DEFAULT_NUM ? ENANOS_TORRE : configuracion.enanos_extra) +
		   (configuracion.elfos_extra == DEFAULT_NUM ? ELFOS_TORRE : configuracion.elfos_extra));
	}

int juego(configuracion_t configuracion, char* ruta_ranking, char* ruta_grabacion) {

	srand((unsigned)time(NULL));
	FILE* grabacion;
	int vel_viento, humedad;
	char animo_gimli, animo_legolas;
	animos(&vel_viento, &humedad, &animo_legolas, &animo_gimli, configuracion);
	juego_t juego;
	system("clear");
	grabacion = abrir_archivo(ruta_grabacion, "w", ACCESO_DENEGADO(ruta_grabacion));
	setear_defensores_iniciales(configuracion);

	caminos_t caminos[MAX_NIVEL];
	inicializar_caminos(caminos, configuracion.ruta_caminos);

		inicializar_juego(&juego, vel_viento, humedad, animo_legolas, animo_gimli, configuracion);
		inicializar_nivel(&(juego.nivel), juego.nivel_actual, caminos);
		while(estado_juego(juego) == JUGANDO){
			mostrar_juego(juego);
			agregar_defensores_iniciales(&juego);
			while(estado_nivel(juego.nivel) == JUGANDO && estado_juego(juego) == JUGANDO){
				agregado_de_defensores_extra(&juego, configuracion);
				jugar_turno(&juego);
				mostrar_juego(juego);
				fwrite(&juego, sizeof(juego_t), 1, grabacion);
				detener_el_tiempo(configuracion.velocidad == DEFAULT_NUM ? VELOCIDAD_DEFAULT : configuracion.velocidad);
			}
			if(estado_juego(juego) == JUGANDO && estado_nivel(juego.nivel) == VICTORIA){
				c_print("Genial! Mataste a todos los enemigos. Proximo nivel: %d.\n", ++juego.nivel_actual);
				inicializar_nivel(&(juego.nivel), juego.nivel_actual, caminos);
			}
		}

		if(estado_juego(juego) == DERROTA){
			c_print("Una de tus torres calló! Los enemigos se han infiltrado. Mas suerte la próxima!\n\n");
		} else if (estado_juego(juego) == VICTORIA){
			c_print("Felicidades! Lograste defender a las torres de todos los orcos!\n\n");
		}
		int puntos = puntaje(juego, configuracion);
		c_print("Tu puntaje es de %i.\n", puntos);
		if(jugador_toma_decision("¿Queres guardar tu puntaje en el ranking?")){
			guardar_puntaje(juego, ruta_ranking, puntos);
		}
	fclose(grabacion);
	
	c_print("Hasta lueginho!.\n");
	return 0;
}