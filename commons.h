#ifndef  __COMMONS_H__
#define  __COMMONS_H__

#define SUCCESS 0
#define FAILURE -1

#define NIVEL_1 1
#define NIVEL_2 2
#define NIVEL_3 3
#define NIVEL_4 4
#define MAX_NIVEL 4

#define TAMANO_NIVEL_1 15
#define TAMANO_NIVEL_2 15
#define TAMANO_NIVEL_3 20
#define TAMANO_NIVEL_4 20

#define MAX_CAMINO 30
#define MAX_RUTA 50
#define MAX_NOMBRE 200

#define TIERRA "■"
#define ANIMO_MALO 'M'
#define ANIMO_REGULAR 'R'
#define ANIMO_BUENO 'B'

#define ELFO 'L'
#define ENANO 'G'
#define TORRE "T"
#define ENTRADA "E"

#define VELOCIDAD_DEFAULT (float)0.1
#define DEFAULT_STRING "-1"
#define DEFAULT_NUM -1

#define MAX_FILAS 30
#define MAX_COLUMNAS 30
#define MAX_LONGITUD_CAMINO 200
#define MAX_ENEMIGOS 500
#define MAX_DEFENSORES 50

typedef struct coordenada {
	int fil;
	int col;
} coordenada_t;

typedef struct defensor {
	char tipo;
	int fuerza_ataque;
	coordenada_t posicion;
} defensor_t;

typedef struct torres {
	int resistencia_torre_1;
	int resistencia_torre_2;
	int enanos_extra;
	int elfos_extra;
} torres_t;

typedef struct enemigo {
	int camino;
	int pos_en_camino;
	int vida;
} enemigo_t;

typedef struct nivel {
	coordenada_t camino_1[MAX_LONGITUD_CAMINO];
	int tope_camino_1;
	
	coordenada_t camino_2[MAX_LONGITUD_CAMINO];
	int tope_camino_2;

	defensor_t defensores[MAX_DEFENSORES];
	int tope_defensores;

	enemigo_t enemigos[MAX_ENEMIGOS];
	int tope_enemigos;

	int max_enemigos_nivel;
} nivel_t;

typedef struct juego {
	nivel_t nivel;
	torres_t torres;
	int nivel_actual;
	int critico_legolas;
	int critico_gimli;
	int fallo_legolas;
	int fallo_gimli;
} juego_t;

typedef struct configuracion {
	int resistencia_torre_1;
	int resistencia_torre_2;
	int enanos_inicio[MAX_NIVEL];
	int elfos_inicio[MAX_NIVEL];
	int enanos_extra;
	int costo_torre_1_enano;
	int costo_torre_2_enano;
	int costo_torre_1_elfo;
	int costo_torre_2_elfo;
	int elfos_extra;
	int fallo_enanos;
	int critico_enanos;
	int fallo_elfos;
	int critico_elfos;
	float velocidad;
	char ruta_caminos[MAX_RUTA];
}  configuracion_t;


#endif