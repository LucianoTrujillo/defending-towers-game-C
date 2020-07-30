#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__
#include "commons.h"
#include <stdio.h>

/* Máximos Configurables*/
#define MAX_ARGUMENTO 100
#define MAX_CAMINOS 10
#define MAX_LONGITUD_CAMINO 200

#define MAX_VIDA_TORRE 100000
#define MAX_DEFENSORES_INICIALES 100
#define MAX_DEFENSORES_EXTRAS 100
#define MAX_COSTO_TORRE_POR_DEFENSOR 1000
#define MAX_CRITICO 100
#define MAX_FALLO 100
#define MAX_VELOCIDAD 5
#define MAX_TEXTO 200
#define OFFSET 12

/* Acciones */
#define JUGAR "jugar"
#define RANKING "ranking"
#define REPE "poneme_la_repe"
#define CREAR_CAMINO "crear_camino"
#define CREAR_CONFIG "crear_configuracion"

/* Defaults */
#define RANKING_DEFAULT "ranking_default.csv"
#define CONFIG_DEFAULT "config_default.txt"
#define REPE_DEFAULT "repe_default.dat"
#define RANKING_AUX "ranking_aux.csv"
#define PRE_RUTA_RANKING "ranking_"

#define CAMINO_1 1
#define CAMINO_2 2


/* Flags */
#define LISTAR "listar="
#define CONFIG "config="
#define GRABACION "grabacion="
#define VELOCIDAD "velocidad="


/* Formatos */
#define FORMATO_RANKING "%[^;];%[^\n]\n"
#define ELEMENTOS_LINEA_RANKING 2
#define ELEMENTOS_LINEA_NIVEL 1
#define ELEMENTOS_LINEA_CAMINO 1
#define ELEMENTOS_LINEA_COORDENADA 2
#define CANTIDAD_LECTURAS_JUEGO 1
#define CONFIG_PARAM_RESISTENCIA_TORRES "RESISTENCIA_TORRES"
#define CONFIG_PARAM_ENANOS_INICIO "ENANOS_INICIO"
#define CONFIG_PARAM_ELFOS_INICIO "ELFOS_INICIO"
#define CONFIG_PARAM_ENANOS_EXTRA "ENANOS_EXTRA"
#define CONFIG_PARAM_ELFOS_EXTRA "ELFOS_EXTRA"
#define CONFIG_PARAM_ENANOS_ANIMO "ENANOS_ANIMO"
#define CONFIG_PARAM_ELFOS_ANIMO "ELFOS_ANIMO"
#define CONFIG_PARAM_VELOCIDAD "VELOCIDAD"
#define CONFIG_PARAM_CAMINOS "CAMINOS"
#define FORMATO_NIVEL "NIVEL="
#define FORMATO_CAMINO "CAMINO="
#define FORMATO_COORDENADA "%i;%i\n"

/* Mensajes al usuario*/
#define SIN_RANKING_ERROR "Todavía no hay ranking, juega partidas para ir llenando la tabla!\n"
#define SIN_RANKING_CONFIG_ERROR "Todavía no hay ranking con esa configuracion, juega partidas para ir llenando la tabla!\n"
#define SIN_GRABACION_ERROR "No se encontró la repe de esa partida!\n"
#define SIN_CONFIG_ERROR "No se encontró esa configuración de juego!\n"
#define SIN_CAMINO "No se encontró el archivo con el camino!\n"
#define ACCESO_DENEGADO(archivo) "No se pudo abir el archivo " #archivo ".\n"
#define FALTA_ARG_OBLIGATORIO(arg) "El argumento " #arg " es obligatorio.\n"
#define VERIFICAR_RUTA "Verifica que tu ruta sea apropiada.\n"
#define ACCION_FRACASO "No se pudo ejectuar la acción.\n"

/* Extensiones */
#define EXTENSION_RANKING ".csv"
#define EXTENSION_CONFIG ".txt"
#define EXTENSION_CAMINOS ".txt"


#define PRIMER_ARGUMENTO 2
#define SUR 0
#define NORTE 1
#define ESTE 2
#define OESTE 3
#define ARRIBA 'w' 
#define ABAJO 's'
#define DERECHA 'd'
#define IZQUIERDA 'a' 
#define CANCELAR 27
#define DESHACER 26

/* Registro de caminos configurables de un nivel*/
typedef struct caminos {
	coordenada_t camino_1[MAX_LONGITUD_CAMINO];
	int tope_camino_1;
	coordenada_t camino_2[MAX_LONGITUD_CAMINO];
	int tope_camino_2;
} caminos_t;


/*
	Se ejecutará el juego "Defendiendo Torres", con la configuración pasada por argumento.
	Si se desea, se puede guardar la partida en un archivo, que tendrá el nombre pasado por argumento.
	En el caso de que la configuracion no sea pasada por argumento, se utilizará la default del juego.

	Ejemplo de comando: $ ./ defendiendo jugar config=mi_configuracion.txt grabacion=mi_partida.dat
*/
void jugar(int argc , const char* argv[]);

/*
	Imprime una lista ordenada de los usuarios con sus respectivos puntajes que hayan jugado una determinada configuración de juego.
	Listará la cantidad pedida por el argumento listar, en caso de default, mostrará todos.
	Si no se especifica la configuración, se elegirá la default.
	El puntaje de cada jugador se obtiene en base a la cantidad de orcos que mató y quan facil fue la configuracion que utilizó.
	Ambos parametros son opcionales.

	Ejemplo de comando: $ ./ defendiendo ranking listar =25 config= mi_configuración.txt
*/
void ranking(int argc, const char* argv[]);

/*
	Se le solicita al usuario la creación de *todos* los caminos del juego.
	Se inicia una manera de creado interactiva y rápida.
	Los caminos se guardaran en un archivo .txt, y estarán completamente validados.
	El nombre del nuevo archivo es obligatorio.

	Ejemplo de comando: $ ./ defendiendo crear_camino mis_caminos.txt
*/
void crear_camino(int argc, const char* argv[]);

/*
	Se reproduce (imprimiendose en pantalla) la partida pasada como archivo, con la velocidad especificada.
	En caso de no elegir uan velocidad, se utilizará la velocidad default.
	El nombre del archivo es obligatorio.

	Ejemplo de comando: $ ./ defendiendo poneme_la_repe grabacion=mi_partida.dat velocidad =0.5

*/
void poneme_la_repe(int argc, const char* argv[]);

/*
	Le permitirá al jugador crear una configuración inicial para utilizar luego.
	La configuración guardada estará completamente validada.
	En caso de que el usuario no eliga un valor, se asignara el valor de -1, el cual luego será manejado por el juego.
	El nombre del archivo es obligatorio.

	Ejemplo de comando: $ ./ defendiendo crear_configuracion mi_configuracion.txt
*/
void crear_configuracion(int argc, const char* argv[]);

/*
	Guarda el puntaje del jugador una vez que finalizó la partida.
	Este puntaje se guarda ordenadamente en el archivo ruta_ranking.
	El orden tiene por al puntaje y luego por nombre descendentemente.
*/
void guardar_puntaje(juego_t juego, char ruta_ranking[MAX_RUTA], int puntaje_nuevo);

/*
	Trata de abrir el archivo en la ubicacion de la ruta y con el modo pasado por parametros.
	En caso de fallar, imprime el mensaje de error.
	En caso de abrirlo con exito, devuelve la referencia del archivo para ser utilizado.
*/
FILE* abrir_archivo(const char* ruta, char* modo, char* mensaje_error);

#endif