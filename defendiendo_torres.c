#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h> //para obtener dimension de la consola
#include <string.h>
#include "defendiendo_torres.h"
#include "commons.h"

#define ENANOS "G"
#define ELFOS "L"
#define ORCOS "O"

#define TAMANO_NIVEL_1 15
#define TAMANO_NIVEL_2 15
#define TAMANO_NIVEL_3 20
#define TAMANO_NIVEL_4 20

#define VIDA_BASE_ENEMIGO 200
#define MAX_VIDA_EXTRA_ENEMIGO 100

#define ATAQUE_ELFO 30
#define ATAQUE_ENANO 60

#define CRITICO_LEGOLAS 70
#define CRITICO_GIMLI 100

#define CHANCES_CRITICO_MALO 0
#define CHANCES_CRITICO_REGULAR 10
#define CHANCES_CRITICO_BUENO 25

#define VICTORIA 1
#define DERROTA -1
#define JUGANDO 0

#define MAX_DIALOGO 300
#define VALIDO 0
#define INVALIDO -1

#define DIVISOR_FALLO_LEGOLAS 2
#define DIVISOR_FALLO_GIMLI 2


#define RANGO_ELFO 3
#define RANGO_ENANO 1

int tope_mapa(int nivel){
	switch(nivel){
		case NIVEL_1: return TAMANO_NIVEL_1;
		case NIVEL_2: return TAMANO_NIVEL_2;
		case NIVEL_3: return TAMANO_NIVEL_3;
		case NIVEL_4: return TAMANO_NIVEL_4;
		default: return 0;
	}
}

/* 
	Calcula el porcentaje de fallo que tendrán los elfos en cada ataque, el cual dependera de la velocidad del viento
	Pre: el viento debe ser un entero positivo
	Post: devolverá un entero positivo correspondiente al viento
*/
int fallo_legolas(int viento) {
	return viento / DIVISOR_FALLO_LEGOLAS;
}

/* 
	Calcula el porcentaje de fallo que tendrán los enanos en cada ataque, el cual dependera de la humedad
	Pre: la humedad debe ser un entero positivo
	Post: devolverá un entero positivo correspondiente a la humedad
*/
int fallo_gimli(int humedad) {
	return humedad / DIVISOR_FALLO_GIMLI;
}

/*
	Calcular el porcentaje de ataques critico que realiza el defensor.
	Pre: el animo debe ser malo, bueno o regular
	Post: devolverá el porcentaje correspondiente acorde al animo 
*/
int critico(char animo) {
	if(animo == ANIMO_MALO)
		return CHANCES_CRITICO_MALO;
	else if(animo == ANIMO_REGULAR)
		return CHANCES_CRITICO_REGULAR;

	return CHANCES_CRITICO_BUENO;
}

/*
	Inicializar los valores fijos una sola vez en toda la partida.
	Recibe los atributos conseguidos en la parte 1, todos deben estar sus respectivos rangos validos.
*/
void inicializar_juego(juego_t* juego, int viento, int humedad, char animo_legolas, char animo_gimli, configuracion_t configuracion){
	juego -> nivel_actual				= NIVEL_1;
	juego -> torres.enanos_extra 		= configuracion.enanos_extra == DEFAULT_NUM ? ENANOS_TORRE : configuracion.enanos_extra;
	juego -> torres.elfos_extra 		= configuracion.enanos_extra == DEFAULT_NUM ? ELFOS_TORRE : configuracion.elfos_extra;
	juego -> torres.resistencia_torre_1 = configuracion.resistencia_torre_1 == DEFAULT_NUM ? RESISTENCIA_TORRE : configuracion.resistencia_torre_1;
	juego -> torres.resistencia_torre_2 = configuracion.resistencia_torre_2 == DEFAULT_NUM ? RESISTENCIA_TORRE : configuracion.resistencia_torre_2;
	juego -> critico_legolas 			= configuracion.critico_elfos == DEFAULT_NUM ? critico(animo_legolas) : configuracion.critico_elfos;
	juego -> critico_gimli 				= configuracion.critico_enanos == DEFAULT_NUM ? critico(animo_gimli) : configuracion.critico_enanos;
	juego -> fallo_legolas				= configuracion.fallo_elfos == DEFAULT_NUM ? fallo_legolas(viento) : configuracion.fallo_elfos;
	juego -> fallo_gimli				= configuracion.fallo_enanos == DEFAULT_NUM ? fallo_gimli(humedad) : configuracion.fallo_enanos;
}

bool esta_en_ultimo_nivel(int nivel_actual) {
	return nivel_actual == MAX_NIVEL;
}

bool enemigo_esta_con_vida(enemigo_t enemigo) {
	return enemigo.vida > 0;
}

bool hay_alguna_torre_muerta(torres_t torres) {
	return torres.resistencia_torre_1 <= 0 || torres.resistencia_torre_2 <= 0;
}

/*
	Verifica si queda por lo menos un enemigo con vida en el nivel, o todavía no salieron todos los enemigos al terreno.
*/
bool quedan_enemigos(nivel_t nivel) {
	bool quedan_enemigos_vivos = false;
	for(int i = 0; i < nivel.tope_enemigos; i++){
		if(enemigo_esta_con_vida(nivel.enemigos[i])) 
			quedan_enemigos_vivos = true;
	}
	return quedan_enemigos_vivos || nivel.tope_enemigos != nivel.max_enemigos_nivel;
}

/*
	Verifica si el juego esta ganando si llego al ultimo nivel, no quedan enemigos y ya salieron todos los enemigos al tablero.
	El juego debe tener valores dentro de los rangos posibles.
*/
bool juego_ganado(juego_t juego) {
	return esta_en_ultimo_nivel(juego.nivel_actual) && !quedan_enemigos(juego.nivel);
}

/*
	Verifica si el juego esta perdido, que sera verdadero en caso de que una de las torres este muerta.
*/
bool juego_perdido(juego_t juego) {
	return hay_alguna_torre_muerta(juego.torres);
}

int estado_juego(juego_t juego) {
	if(juego_ganado(juego))
		return VICTORIA;
	else if(juego_perdido(juego))
		return DERROTA;
	else
		return JUGANDO;
}

int estado_nivel(nivel_t nivel){
	if(!quedan_enemigos(nivel))
		return VICTORIA;
	else
		return JUGANDO;
}

/*
	Verifica si dos coordenadas indican la misma posicion
*/
bool posicion_ocupada(coordenada_t pos1, coordenada_t pos2) {
	return pos1.fil == pos2.fil && pos1.col == pos2.col;
}

/*
	Verifica si la posicion pasada por parametros esta ocupada en el nivel por algun defensor
*/
bool hay_defensor_en_coordenada(nivel_t nivel, coordenada_t nueva_coordenada) {
	bool hay_defensor = false;
	for(int i = 0; i < nivel.tope_defensores; i++) 
		if(posicion_ocupada(nivel.defensores[i].posicion, nueva_coordenada))
				hay_defensor = true;

	return hay_defensor;
}

/*
	Verifica si la posicion pasada por parametros esta ocupada por algun camino
*/
bool hay_camino_en_coordenada(nivel_t nivel, coordenada_t posicion) {
	bool hay_camino = false;
	for(int i = 0; i < nivel.tope_camino_1; i++) 
		if(posicion_ocupada(nivel.camino_1[i], posicion))
				hay_camino = true;

	for(int i = 0; i < nivel.tope_camino_2; i++) 
		if(posicion_ocupada(nivel.camino_2[i], posicion))
				hay_camino = true;

	return hay_camino;
}

/*
	Verifica que no haya tanto defensores como camino en la coordenada
*/
bool coordenada_esta_vacia(nivel_t nivel, coordenada_t posicion, char tipo){
	return !hay_defensor_en_coordenada(nivel, posicion) && !hay_camino_en_coordenada(nivel, posicion);
}


/*
	Agregar un defensor a la lista de defensores en la coordenada pasada con el tipo y ataque correspondiente.
	Esta funcion debe ser llamada cuando todas las validaciones ya se hayan hecho.
*/
void agregar_defensor_al_campo_de_batalla(defensor_t defensores[], int* tope_defensores, coordenada_t posicion, char tipo){
	defensores[*tope_defensores].tipo = tipo;
	defensores[*tope_defensores].posicion = posicion;

	if(tipo == ELFO)
		defensores[*tope_defensores].fuerza_ataque = ATAQUE_ELFO;
	if(tipo == ENANO)
		defensores[*tope_defensores].fuerza_ataque = ATAQUE_ENANO;

	(*tope_defensores)++;
}

int agregar_defensor(nivel_t* nivel, coordenada_t posicion, char tipo){
	if(coordenada_esta_vacia(*nivel, posicion, tipo)){
		agregar_defensor_al_campo_de_batalla(nivel -> defensores, &(nivel -> tope_defensores), posicion, tipo);
		return VALIDO;
	}

	return INVALIDO;
}

/*
	A partir de el daño basico de ataque, el el daño critico y los procentages de critico y fallo,
	devolverá el daño que se le sacará. Al basarse en procentaje, no será siempre el mismo.
*/
int ataque(int ataque, int golpe_critico, int porcentaje_critico, int porcentaje_fallo) {
	if(rand() % 101 >= porcentaje_fallo){
		if(rand() % 101 <= porcentaje_critico)
			return golpe_critico;
		return ataque;
	}
	return 0;
}

/*
	Le resta la vida al enemigo pasado por parametro, el valor del ataque dependerá de los valores de ataque del enano, fallo y critico de gimli.
*/
void enano_ataca_enemigo(defensor_t enano, enemigo_t* enemigo, juego_t juego){
	enemigo -> vida -= ataque(enano.fuerza_ataque, CRITICO_GIMLI, juego.critico_gimli, juego.fallo_gimli);
}

/*
	Determina si la posicion del enemigo esta suficientemente cerca para ser atacado por el enano.
	Dado que solo puede atacar los casilleros alrededor, la distancia entre el enemigo y el enano tendrá que ser menor a 1 casillero en ambas direcciones.
	Como caso mas extremo tendra distancia 1 en fila y en columna, por lo cual se encontrará diagonalmente.
*/
bool enano_llega_a_atacar(defensor_t defensor, coordenada_t pos_enemigo){
	return abs(defensor.posicion.fil - pos_enemigo.fil) <= RANGO_ENANO && abs(defensor.posicion.col - pos_enemigo.col) <= RANGO_ENANO;
}

/*
	Devuelve la coordenada del mapa en la cual se encuentra el enemigo, en base a la posicion del camino en el cual esta.
*/
coordenada_t posicion_enemigo (enemigo_t enemigo, juego_t juego) {
	if(enemigo.camino == 1)
		return juego.nivel.camino_1[enemigo.pos_en_camino];
	if(enemigo.camino == 2)
		return juego.nivel.camino_2[enemigo.pos_en_camino];
	return (coordenada_t){0,0};
}

/*
	Devuelve si el enemigo esta en rango de ataque del enano.
*/
bool enemigo_esta_en_rango_de_enano(defensor_t enano, enemigo_t* enemigo, juego_t* juego){
	return enano_llega_a_atacar(enano, posicion_enemigo(*enemigo, *juego));
}

/*
	El enano pasado por parametro ataca al enemigo vivo que mas cerca este de llegar a la torre y que esté en su rango.
*/
void ejecutar_juegada_enano(defensor_t enano, juego_t* juego) {
	int i = 0;
	bool ataco = false;
	while(i < (juego -> nivel.tope_enemigos) && !ataco){
		if(enemigo_esta_en_rango_de_enano(enano, &(juego -> nivel.enemigos[i]), juego) && juego -> nivel.enemigos[i].vida > 0){
			enano_ataca_enemigo(enano, &(juego -> nivel.enemigos[i]), *juego);
			ataco = true;
		}
		i++;
	}
}

/*
	Ejecuta la jugada de todos los enanos del nivel.
*/
void ejecutar_jugada_enanos(juego_t* juego) {
	for(int i = 0; i < juego -> nivel.tope_defensores; i++)
		if(juego -> nivel.defensores[i].tipo == ENANO)
			ejecutar_juegada_enano(juego -> nivel.defensores[i], juego);
}

/*
	Verifica si dos coordenadas estan a una distancia maxima especifica, tomando como medida al rango manhattan.
*/
bool esta_en_rango_manhattan(coordenada_t pos1, coordenada_t pos2, int distancia_maxima){
	return 	abs(pos1.fil - pos2.fil) + abs(pos1.col - pos2.col) <= distancia_maxima;
}

/*
	Verifica si el elfo llega a atacar al enemigo. 
*/
bool enemigo_esta_en_rango_de_elfo(defensor_t defensor, enemigo_t enemigo, juego_t* juego){
	return esta_en_rango_manhattan(defensor.posicion, posicion_enemigo(enemigo, *juego), RANGO_ELFO);
}

/*
	Le resta la vida al enemigo pasado por parametro, el valor del ataque dependerá de los valores de fallo y critico de legolas.
*/
void elfo_ataca_enemigo(defensor_t elfo, enemigo_t* enemigo, juego_t juego){
	enemigo -> vida -= ataque(elfo.fuerza_ataque, CRITICO_LEGOLAS, juego.critico_legolas, juego.fallo_legolas);
}

/*
	El elfo pasado por parametro ataca a todos los enemigos vivos que estén en su rango.
*/
void ejecutar_jugada_elfo(defensor_t elfo, juego_t* juego) {
	for(int i = 0; i < (juego -> nivel.tope_enemigos); i++)
		if(enemigo_esta_en_rango_de_elfo(elfo, juego -> nivel.enemigos[i], juego) && juego -> nivel.enemigos[i].vida > 0)
			elfo_ataca_enemigo(elfo,  &(juego -> nivel.enemigos[i]), *juego);
}

/*
	Ejecuta la jugada de todos los elfos del nivel.
*/
void ejecutar_jugada_elfos(juego_t* juego) {
	for(int i = 0; i < juego -> nivel.tope_defensores; i++)
		if(juego -> nivel.defensores[i].tipo == ELFO)
			ejecutar_jugada_elfo(juego -> nivel.defensores[i], juego);
}

/*
	Verifica si la posicion del enemigo esta a distancia de un casillero de la torre.
*/
bool enemigo_llego_a_torre(enemigo_t* enemigo, juego_t* juego){
	int tope_camino_enemigo;
	if(enemigo -> camino == 1)
		tope_camino_enemigo = juego -> nivel.tope_camino_1;
	if(enemigo -> camino == 2)
		tope_camino_enemigo = juego -> nivel.tope_camino_2;

	return enemigo -> pos_en_camino == tope_camino_enemigo - 2; 
}

void enemigo_avanza_camino(enemigo_t* enemigo) {
	(enemigo -> pos_en_camino)++;
}

/*
	Dependiendo del camino en el que se encuentre el enemigo, dañara la torre correspondiente.
	El daño producido seran los puntos de vida que le quedan al enemigo. Luego, morira.
*/
void enemigo_ataca_torre(enemigo_t* enemigo, juego_t* juego) {
	if(enemigo -> camino == 1){
		juego -> torres.resistencia_torre_1 -= enemigo -> vida;
		if(juego -> torres.resistencia_torre_1 <= 0){
			juego -> torres.resistencia_torre_1 = 0;
		}
	}
	if(enemigo -> camino == 2){
		juego -> torres.resistencia_torre_2 -= enemigo -> vida;
		if(juego -> torres.resistencia_torre_2 <= 0){
			juego -> torres.resistencia_torre_2 = 0;
		}
	}
	enemigo -> vida = 0; 
}

/*
	Si el enemigo llegó a la torre, este atacara, sino, avanzará en el camino.
*/
void ejecutar_juegada_enemigo(enemigo_t* enemigo, juego_t* juego) {
	if(enemigo_llego_a_torre(enemigo, juego))
		enemigo_ataca_torre(enemigo, juego);
	else
		enemigo_avanza_camino(enemigo);
}


/*
	Agrega un nuevo enemigo a la lista de enemigos, inicilizandolo con un vida pseudo aleatoria, en la primer posicion del camino recibido.
*/
void agregar_enemigo(enemigo_t enemigos[MAX_ENEMIGOS], int camino, int* tope_enemigos){
	enemigos[*tope_enemigos].camino = camino;
	enemigos[*tope_enemigos].pos_en_camino = 0;
	enemigos[*tope_enemigos].vida = VIDA_BASE_ENEMIGO + rand() % (MAX_VIDA_EXTRA_ENEMIGO + 1);
	(*tope_enemigos)++;
}

/*
	Ejecuta la jugada de todos los enemigos vivos y crea mas si la cantidad del nivel no llegó a la maxima. En caso de poder agregar enemigos:
		Creará 1 si solo hay un camino.
		Creará 2 si hay dos caminos.
*/
void ejecutar_jugada_enemigos(juego_t* juego) {
	for(int i = 0; i < (juego -> nivel.tope_enemigos); i++)
		if(juego -> nivel.enemigos[i].vida > 0)
			ejecutar_juegada_enemigo(&(juego -> nivel.enemigos[i]), juego);

	if(juego -> nivel.tope_enemigos < juego -> nivel.max_enemigos_nivel){
		agregar_enemigo(juego -> nivel.enemigos, 1, &(juego -> nivel.tope_enemigos));
		if(juego -> nivel_actual > 2)
			agregar_enemigo(juego -> nivel.enemigos, 2, &(juego -> nivel.tope_enemigos));
	}
}

/*
	Ejecuta los turnos de los personajes en el orden correspondiente.
*/
void jugar_turno(juego_t* juego) {
	ejecutar_jugada_enanos(juego);
	ejecutar_jugada_elfos(juego);
	ejecutar_jugada_enemigos(juego);
}

/*
	borra la consola y resetea todo el mapa poniendo todos sus valores en tierra.
*/
void limpiar_tablero(char* tablero[MAX_FILAS][MAX_COLUMNAS], int tope){
	system("clear");
	for(int i = 0; i < tope; i++){
		for(int j = 0; j < tope; j++){
			tablero[i][j] = TIERRA;
		}
	}
}

/*
	llena a la matriz tablero de los caracteres correspondientes al juego.
		En los caminos pondra un espacio.
		En los enemigos pondra el caracter de orco.
		En el primer y ultimo casillero del camino pondrá correspondientemente la entrada y la torre.
		En los defensores el caracter que los representa.
		Todo lo que no sea ninguno de los anteriores quedará como estaba.
*/
void llenar_tablero(char* tablero[MAX_FILAS][MAX_COLUMNAS], int tope, juego_t juego){
	for(int i = 0; i < juego.nivel.tope_camino_1; i++){
		coordenada_t pos = juego.nivel.camino_1[i];
		tablero[pos.fil][pos.col] = " ";
		if(i == 0)
			tablero[pos.fil][pos.col] = ENTRADA;
		if(juego.nivel.tope_camino_1 - i == 1)
			tablero[pos.fil][pos.col] = TORRE;
	}

	for(int i = 0; i < juego.nivel.tope_camino_2; i++){
		coordenada_t pos = juego.nivel.camino_2[i];
		tablero[pos.fil][pos.col] = " ";
		if(i == 0)
			tablero[pos.fil][pos.col] = ENTRADA;
		if(juego.nivel.tope_camino_2 - i == 1)
			tablero[pos.fil][pos.col] = TORRE;
	}

	coordenada_t pos_enemigo;
	for(int i = 0; i < juego.nivel.tope_enemigos; i++){
		if(juego.nivel.enemigos[i].vida > 0){
		pos_enemigo = posicion_enemigo(juego.nivel.enemigos[i], juego);
		tablero[pos_enemigo.fil][pos_enemigo.col] = ORCOS;
		}
	}

	for(int i = 0; i < juego.nivel.tope_defensores; i++){
		coordenada_t pos = juego.nivel.defensores[i].posicion;
		if(juego.nivel.defensores[i].tipo == ELFO)
			tablero[pos.fil][pos.col] = ELFOS;
		if(juego.nivel.defensores[i].tipo == ENANO)
			tablero[pos.fil][pos.col] = ENANOS;
	}
}

/*
	Imprime una tabla en el medio de la pantalla conteniendo informacion relevante del nivel. Vida de torres, enemigos restantes, nivel actual.
*/
void imprimir_tabla_datos(juego_t juego){
	char tabla[MAX_DIALOGO];
	size_t offset = 12; // dado el uso de barritas se rompe un poco el formato y me vi obligado a usar este magic number como offset para que este alineado
	struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

	sprintf(tabla, "║  Orcos restantes: %d  ║  Vida torre 1: %d  ║  Vida torre 2: %d  ║  nivel: %d  ║", juego.nivel.max_enemigos_nivel - juego.nivel.tope_enemigos, juego.torres.resistencia_torre_1, juego.torres.resistencia_torre_2, juego.nivel_actual);
	
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
	Imprime el tablero y la tabla de datos siempre en el medio de la pantalla, no importa el tamaño de la consola.
	El tablero debe estar previamente llenado con todos los valores del juego.

	Nota: Hago uso de  <sys/ioctl.h> para obtener dimension de la consola, luego mete espacios hasta la mitad y ahi recien imprime todo.
*/
void imprimir_tablero(char* tablero[MAX_FILAS][MAX_COLUMNAS], int tope, juego_t juego){
	struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
	for (int i = 0; i < (w.ws_row - tope) / 2; i++)
        printf("\n");

	imprimir_tabla_datos(juego);
	printf("\n\n");

	for(int i = 0; i < tope; i++){
		printf("%*s", (int)((w.ws_col - (tope * 3)) / 2), " ");
		printf("%5i |", i);
		for(int j = 0; j < tope; j++)
				printf(" %s ", tablero[i][j]);
		printf("\n");
	}

	printf("%*s", (int)((w.ws_col - (tope * 3)) / 2), " ");
	printf("%7c", ' ');
	for(int i = 0; i < tope * 3; i++)
		printf("-");
	printf("\n");

	printf("%*s", (int)((w.ws_col - (tope * 3)) / 2), " ");
	printf("%7c", ' ');
	for(int i = 0; i < tope; i++){
		if(i < 10)
			printf("%2i ", i);
		else	
			printf("%3i", i);
	}
			

	for (int i = 0; i < (w.ws_row - tope) / 2; i++)
        printf("\n");
}

/* 
	Mostrará el mapa y tabla de datos dependiendo del nivel en que se encuentra el jugador.
 */
void mostrar_juego(juego_t juego){
	char* tablero[MAX_FILAS][MAX_COLUMNAS];
	int tope = tope_mapa(juego.nivel_actual);
	limpiar_tablero(tablero, tope);
	llenar_tablero(tablero, tope, juego);
	imprimir_tablero(tablero, tope, juego);
	printf("\n");
}
