/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "animos.h"
#include "commons.h"

#define ANIMO_MALO 'M'
#define ANIMO_REGULAR 'R'
#define ANIMO_BUENO 'B'

const char LEGOLAS = 'L';
const char GIMLI = 'G';

const int MIN_DIA_MES = 1;
const int MAX_DIA_MES = 30;

const int ULTIMO_DIA_VEL_VIENTO_MAX = 10;
const int ULTIMO_DIA_VEL_VIENTO_MED = 20;
const int ULTIMO_DIA_VEL_VIENTO_MIN = 30;

const int VEL_VIENTO_MAX = 75;
const int VEL_VIENTO_MED = 50;
const int VEL_VIENTO_MIN = 25;

const char MANIANA = 'M';
const char TARDE = 'T';
const char NOCHE = 'N';

const int HUMEDAD_MANIANA = 75;
const int HUMEDAD_TARDE = 25;
const int HUMEDAD_NOCHE = 50;

const char PIE_IZQ = 'I';
const char PIE_DER = 'D';

const char ENSALADA = 'E';
const char HAMBURGUESA = 'H';
const char PIZZA = 'P';
const char GUISO = 'G';

const int MIN_HORAS_DORMIDAS = 0;
const int MAX_HORAS_DORMIDAS_MIN = 4;
const int MAX_HORAS_DORMIDAS_MED = 8;
const int MAX_HORAS_DORMIDAS_MAX = 12;

const int PUNTOS_BIENESTAR_PIE_IZQ = 0;
const int PUNTOS_BIENESTAR_PIE_DER = 10;
const int PUNTOS_BIENESTAR_ENSALADA = 20;
const int PUNTOS_BIENESTAR_HAMBURGUESA = 15;
const int PUNTOS_BIENESTAR_PIZZA = 10;
const int PUNTOS_BIENESTAR_GUISO = 5;
const int PUNTOS_BIENESTAR_HORAS_DORMIDAS_MIN = 0;
const int PUNTOS_BIENESTAR_HORAS_DORMIDAS_MED = 10;
const int PUNTOS_BIENESTAR_HORAS_DORMIDAS_MAX = 20;

const int MAX_PUNTOS_BIENESTAR_MALO = 20;
const int MAX_PUNTOS_BIENESTAR_REGULAR = 35;
const int MAX_PUNTOS_BIENESTAR_BUENO = 50;

const int POS_X_INICIO = 0;
const int ANCHO_BARRA = 15;
const int ESPACIO_ENTRE_BARRAS = 10;
const int ALTURA_GRAFICO = 100;


//Avisa de que personaje son los atributos ingresados a continuacion
void pedir_valores_personaje(char personaje){
    if(personaje == LEGOLAS){
        printf("\nA continuacion va a ingresar datos de %s.\n", "Legolas");
    } else if(personaje == GIMLI){
        printf("\nA continuacion va a ingresar datos de %s.\n", "Gimli");
    }
}


//Avisa el rango de valores en los que puede estar el input del usuario. Esta abstraccion permite no repetir el mismo mensaje cada vez que se requiera este tipo de input
void pedir_valor_entre_rangos(char valor[],int min, int max, bool primer_intento){
    if(primer_intento) {
        printf("Ingrese %s entre %i y %i (inclusive): ", valor, min, max);
    } else {
        printf("Porfavor, verifique que su entrada este entre %i y %i (inclusive): ", min, max);
    }
}

//Avisa las posibles opciones del input. Esta abstraccion permite no repetir el mismo mensaje cada vez que se requiera este tipo de input
void pedir_valor_de_opciones_limitadas(char valor[], char opciones[], bool primer_intento){
    if(primer_intento) {
        printf("Ingrese %s. Las opciones son %s: ", valor, opciones);
    } else {
        printf("Porfavor, verifique que su entrada sea alguna de las siguientes: %s: ", opciones);
    }
}

// pre-cond: supone dia entre el rango valido
// post-cond: devolvera la velocidad del viento correspondiente al dia del mes
int velocidad_viento(int dia_del_mes){
    int vel_viento = 0;
    if(dia_del_mes <= ULTIMO_DIA_VEL_VIENTO_MAX){
        vel_viento = VEL_VIENTO_MAX;
    } else if (dia_del_mes <= ULTIMO_DIA_VEL_VIENTO_MED){
        vel_viento = VEL_VIENTO_MED;
    } else if (dia_del_mes <= ULTIMO_DIA_VEL_VIENTO_MIN) {
        vel_viento = VEL_VIENTO_MIN;
    }
    return vel_viento;
}

//pre-cond: supone caracteres válidos de la hora del dia posible
//post-cond: devolvera la humedad correspondiente a la hora del dia
int obtener_humedad(char hora_del_dia){
    int humedad = 0;
    if(hora_del_dia == MANIANA){
        humedad = HUMEDAD_MANIANA;
    } else if(hora_del_dia == TARDE){
        humedad = HUMEDAD_TARDE;
    } else if(hora_del_dia == NOCHE){
        humedad = HUMEDAD_NOCHE;
    }
    return humedad;
}

//pre-cond: supone caracteres válidos del pie
//post-cond: devolvera los puntos de bienstar correspondiente al pie ingresado
int obtener_puntos_bienestar_pie(char pie){
    int puntos = 0;
    if(pie == PIE_DER){
        puntos = PUNTOS_BIENESTAR_PIE_DER;
    } else if(pie == PIE_IZQ) {
        puntos = PUNTOS_BIENESTAR_PIE_IZQ;
    }
    return puntos;
}

//pre-cond: supone caracteres válidos de la cena
//post-cond: devolvera los puntos de bienstar correspondiente a la cena ingresada
int obtener_puntos_bienestar_cena(char cena){
    int puntos = 0;
    if(cena == ENSALADA) {
        puntos = PUNTOS_BIENESTAR_ENSALADA;
    } else if (cena == HAMBURGUESA){
        puntos = PUNTOS_BIENESTAR_HAMBURGUESA;
    } else if (cena == PIZZA){
        puntos = PUNTOS_BIENESTAR_PIZZA;
    } else if (cena == GUISO){
        puntos = PUNTOS_BIENESTAR_GUISO;
    }
    return puntos;
}

//pre-cond: supone horas entre el rango valido
//post-cond: devolvera los puntos de bienstar correspondiente a las horas dormidas
int obtener_puntos_bienestar_horas_dormidas(int horas){
    int puntos = 0;
    if(horas <= MAX_HORAS_DORMIDAS_MIN){
        puntos = PUNTOS_BIENESTAR_HORAS_DORMIDAS_MIN;
    } else if (horas <= MAX_HORAS_DORMIDAS_MED){
        puntos = PUNTOS_BIENESTAR_HORAS_DORMIDAS_MED;
    } else if (horas <= MAX_HORAS_DORMIDAS_MAX){
        puntos = PUNTOS_BIENESTAR_HORAS_DORMIDAS_MAX;
    }
    return puntos;
}

//pre-cond: supone entradas que el pie y la cena esten entre las opciones validas y que las horas dormidas esten en el rango valido
//post-cond: devolera los puntos de bienestar de un personaje con los atributos recibidos como argumentos
int obtener_puntos_bienestar(char primer_pie_amanecido, char cena, int horas_dormidas){
    return obtener_puntos_bienestar_pie(primer_pie_amanecido) + obtener_puntos_bienestar_cena(cena) + obtener_puntos_bienestar_horas_dormidas(horas_dormidas);
}

//post-cond: devolvera el animo correspondiente a los puntos debienestar de un personaje
char animo(int puntos_bienestar){
    char animo = ANIMO_REGULAR;
    if(puntos_bienestar <= MAX_PUNTOS_BIENESTAR_MALO){
        animo = ANIMO_MALO;
    } else if(puntos_bienestar <= MAX_PUNTOS_BIENESTAR_REGULAR) {
        animo = ANIMO_REGULAR;
    } else if(puntos_bienestar <= MAX_PUNTOS_BIENESTAR_BUENO) {
        animo = ANIMO_BUENO;
    }
    return animo;
}

//post-cond: devolvera un dia entre entre el valor minimo y maximo posible
void obtener_dia_del_mes(int* dia) {
    pedir_valor_entre_rangos("dia del mes", MIN_DIA_MES, MAX_DIA_MES, true);
    scanf("%i", dia);
    while(*dia < MIN_DIA_MES || *dia > MAX_DIA_MES){
        pedir_valor_entre_rangos("dia del mes", MIN_DIA_MES, MAX_DIA_MES, false);
        scanf("%i", dia);
    }
}

//post-cond: devolvera un caracter corrspondiente a la hora del dia engresada por el usuario
void obtener_hora_del_dia(char* hora_del_dia) {
    pedir_valor_de_opciones_limitadas("hora del dia", "Mañana (M), Tarde (T), Noche (N)", true);
    scanf(" %c", hora_del_dia);
    while(*hora_del_dia != MANIANA && *hora_del_dia != TARDE && *hora_del_dia != NOCHE){
        pedir_valor_de_opciones_limitadas("hora del dia", "Mañana (M), Tarde (T), Noche (N)", false);
        scanf(" %c", hora_del_dia);
    }
}

//post-cond: devolvera un caracter corrspondiente al pie con el que amanecio
void obtener_pie_amanecido(char* pie_amanecido) {
    pedir_valor_de_opciones_limitadas("con que pie amanecio", "Derecho (D), Izquierdo (I)", true);
    scanf(" %c", pie_amanecido);
    while(*pie_amanecido != PIE_DER && *pie_amanecido != PIE_IZQ){
        pedir_valor_de_opciones_limitadas("con que pie amanecio", "Derecho (D), Izquierdo (I)", false);
        scanf(" %c", pie_amanecido);
    }
}

//post-cond: devolvera un caracter corrspondiente a lo que ceno el personaje la noche anterior
void obtener_cena(char* cena) {
    pedir_valor_de_opciones_limitadas("que comio anoche", "Ensalada (E), Hamburguesa (H), Pizza (P), Guiso (G)", true);
    scanf(" %c", cena);
    while(*cena != ENSALADA && *cena != HAMBURGUESA && *cena != PIZZA && *cena != GUISO){
        pedir_valor_de_opciones_limitadas("que comio anoche", "Ensalada (E), Hamburguesa (H), Pizza (P), Guiso (G)", false);
        scanf(" %c", cena);
    }
}

//post-cond: devolvera cantidad de horas dormidas del personaje entre el rango valido
void obtener_horas_dormidas(int* horas_dormidas) {
    pedir_valor_entre_rangos("horas dormidas", MIN_HORAS_DORMIDAS, MAX_HORAS_DORMIDAS_MAX, true);
    scanf("%i", horas_dormidas);
    while(*horas_dormidas < MIN_HORAS_DORMIDAS || *horas_dormidas > MAX_HORAS_DORMIDAS_MAX){
        pedir_valor_entre_rangos("horas dormidas", MIN_HORAS_DORMIDAS, MAX_HORAS_DORMIDAS_MAX, false);
        scanf("%i", horas_dormidas);
    }
}

//analisis: Se obtienen los atritubtos de un personaje por input para luego devolver el puntaje de bienestar total. 
//pre-cond: que el nombre del personaje sea de los que existen
//post-cond: devolvera los puntos de bienestar del personaje ingresado
int puntos_animo_personaje(char nombre){
    char pie_amanecido, cena;
    int horas_dormidas;

    pedir_valores_personaje(nombre);
    obtener_pie_amanecido(&pie_amanecido);
    obtener_cena(&cena);
    obtener_horas_dormidas(&horas_dormidas);
    return obtener_puntos_bienestar(pie_amanecido, cena, horas_dormidas);
}

//post-cond devolvera el animo correspondiente a los puntos de bienestar
char animo_personaje(int puntos_bienestar){
    return animo(puntos_bienestar);
}

/*  DISCLAIMER: las funciones a continuacion son unicamente para mostrar de linda manera los resultados del programa, no tienen que ver con los requisitos del TP.
    Se muestra un grafico de barras donde cada barra tiene la altura del valor correspondiente. Ejemplo: si la velocidad del viento es 75km/h, la barra tendra una altura de 75

    A continuacion se explica brevemente como funciona:
        - Recorre una cuadricula en dos ciclos anidados
            * El primero recorre la cuadricula en Y
            * El anidado recorre la cuadricula en X para cada Y
        - Dentro de cada coordenada se fija que es lo que tiene que dibujar, si un eje, un espacio, etc.
    */

//pre-cond: espera que el eje izq, el ancho de la  barra y la posicion actual en X sea correcta
//post-cond: devuelve si esta o no "dentro" de la barra en la posicion X
bool en_rango_x_techo(int eje_izq, int ancho_barra, int posicion_x){
     bool en_rango = (posicion_x >= eje_izq) && (posicion_x <= eje_izq + ancho_barra);
     return en_rango;
 }
//pre-cond: espera que el eje izq, el ancho de la  barra y la posicion actual en X sea correcta
//post-cond: devuelve si esta o no exactamente en los bordes de la barra
bool en_pos_x_eje(int eje_izq, int ancho_barra, int posicion_x){
     bool en_rango = posicion_x == eje_izq || posicion_x == eje_izq + ancho_barra;
     return en_rango;
 }
 
//pre-cond: espera la posicion actual en Y, la altura del grafico y el valor de lo que queremos pintar
//post-cond: devuelve si estamos parados sobre el techo de la barra
bool es_techo_barra(int posicion_y, int altura_grafico, int valor) {
     int altura_actual = (altura_grafico - posicion_y);
     int resta = altura_actual - valor;
     return (resta == 1);
 }
 
//pre-cond: espera la posicion actual en Y, la altura del grafico y el valor de lo que queremos pintar
//post-cond: devuelve si estamos dentro de la barra en cuanto a Y
bool pos_y_en_barra(int posicion_y, int altura_grafico, int valor) {
     int altura_actual = (altura_grafico - posicion_y);
     int resta = altura_actual - valor;
     return resta <= 1;
 }

//pre-cond: espera el valor de una barra y sus delimitaciones
//post-cond: pinta la barra y devuelve si true si hay que pintar un espacio
bool pintar_barra(int valor, int eje_izq, int pos_y, int pos_x, int altura_grafico, int ancho_barra ) {
     bool pintar_espacio = true;
     if(pos_y_en_barra(pos_y, altura_grafico, valor)){
         if((es_techo_barra(pos_y, altura_grafico, valor) && en_rango_x_techo(eje_izq, ancho_barra, pos_x)) ) {
             printf("_");
             pintar_espacio = false;
         } else if(en_pos_x_eje(eje_izq, ancho_barra, pos_x)){
             printf("|");
             pintar_espacio = false;
         } 
     }
     return pintar_espacio;
  }

//pre-cond: espera la posicion actual en X, la posicion actual en Y, y el tamaño del grafico
//post-cond: pinta los bordes del grafico y devuelve true si hay que pintar un espacio
bool pintar_bordes(int pos_x, int pos_y, int altura_grafico, int ancho_grafico){
    bool pintar_espacio = true;
    if(pos_x == 0) {
        if(ALTURA_GRAFICO - pos_y < 10){
            printf("║ %i  ", ALTURA_GRAFICO - pos_y);
        } else if(pos_y != 0){
             printf("║ %i ", ALTURA_GRAFICO - pos_y);
        } else {
            printf("║ %i", ALTURA_GRAFICO);
        }
        pintar_espacio = false;
    } else if(pos_x == ancho_grafico - 1){
        printf("║");
        pintar_espacio = false;
    } 
    return pintar_espacio;
  }

//pre-cond: recibe el ancho del gafico
//post-cond: no devuelve nada, solo pinta el techo
void pintar_techo(int ancho_grafico){
    printf("╔");
    for(int i = 0; i <= ancho_grafico + 1; i++){
        printf("═");
    }
    printf("╗\n");
}

//pre-cond: recibe el ancho del gafico
//post-cond: no devuelve nada, solo pinta el piso
void pintar_piso(int ancho_grafico){
    printf("╚");
    for(int i = 0; i <= ancho_grafico + 1; i++){
        printf("═");
    }
    printf("╝\n\n");
}

//pre-cond: valores entre 0 y 100
//post-cond: un grafico en consola con los diferentes valores
void mostrar_grafico(int velocidad, int humedad, int humor_legolas, int humor_gimli){
   //los ejes izquirdos de las barras
   int eje_izq_velocidad = (POS_X_INICIO + ESPACIO_ENTRE_BARRAS);
   int eje_izq_humedad = (eje_izq_velocidad + ANCHO_BARRA + ESPACIO_ENTRE_BARRAS);
   int eje_izq_legolas = (eje_izq_humedad + ANCHO_BARRA + ESPACIO_ENTRE_BARRAS);
   int eje_izq_gimli = (eje_izq_legolas + ANCHO_BARRA + ESPACIO_ENTRE_BARRAS);
   int ancho_grafico = eje_izq_gimli + ANCHO_BARRA + ESPACIO_ENTRE_BARRAS;
   pintar_techo(ancho_grafico);
   for(int pos_y = 0; pos_y < ALTURA_GRAFICO; pos_y++){ //por cada y del grafico
        for(int pos_x = 0; pos_x < ancho_grafico; pos_x++){ // por cada lugar en x de la fila
          /* pinta las barras si es que se necesita
               si no se tiene que pintar ninguna, printea un espacio y sigue*/
            if(pintar_barra(velocidad, eje_izq_velocidad, pos_y, pos_x, ALTURA_GRAFICO, ANCHO_BARRA) &&
                pintar_barra(humedad, eje_izq_humedad, pos_y, pos_x, ALTURA_GRAFICO, ANCHO_BARRA) &&
                pintar_barra(humor_legolas, eje_izq_legolas, pos_y, pos_x, ALTURA_GRAFICO, ANCHO_BARRA) &&
                pintar_barra(humor_gimli, eje_izq_gimli, pos_y, pos_x, ALTURA_GRAFICO, ANCHO_BARRA) &&
                pintar_bordes(pos_x, pos_y, ALTURA_GRAFICO, ancho_grafico)){
                    printf(" ");
             }
         }
         printf("\n");
    }
    printf("║              Viento: %ikm/h            Humedad: %i%%              Legolas: %i               Gimli: %i           ║\n", velocidad, humedad, humor_legolas, humor_gimli);
    pintar_piso(ancho_grafico);
}

//pre-cond: la velicudad del viento, la humedad, el animo de legolas y gimli en los rangos de valores posibles
 //post-cond: Printea un resumen de los datos en consola
void mostrar_datos(int velocidad_viento, int humedad, char animo_legolas, char animo_gimli){
    //printf("Resumen: %i %i %c %c.\n", velocidad_viento, humedad, animo_legolas, animo_gimli);
}

void animos (int* viento, int* humedad, char* animo_legolas, char* animo_gimli, configuracion_t configuracion){

    int dia_del_mes;
    char hora_del_dia;

    if(configuracion.fallo_elfos == DEFAULT_NUM){
        obtener_dia_del_mes(&dia_del_mes);
        *viento = velocidad_viento(dia_del_mes);
    }
    
    if(configuracion.fallo_enanos == DEFAULT_NUM){
        obtener_hora_del_dia(&hora_del_dia);
        *humedad = obtener_humedad(hora_del_dia);   
    }

    if(configuracion.critico_elfos == DEFAULT_NUM){
        *animo_legolas = animo_personaje(puntos_animo_personaje(LEGOLAS));
    }

    if(configuracion.critico_enanos == DEFAULT_NUM){
        *animo_gimli = animo_personaje(puntos_animo_personaje(GIMLI));
    }
}

