#ifndef UTILES_H
#define UTILES_H

/*
 * Generará un camino aleatorio desde la entrada hasta la torre.
 * Las coordenadas de entrada y de la torre deben ser válidas. 
 */
void obtener_camino(coordenada_t camino[MAX_LONGITUD_CAMINO], int* tope_camino, coordenada_t entrada, coordenada_t torre);

/*
 * Detendrá el tiempo los segundos indicados como parámetro.
 * Ejemplos: 
 * - Para detener medio segundo se deberá llamar detener_el_tiempo(0.5)
 * - Para detener dos segundos se deberá llamar detener_el_tiempo(2)  
 */
void detener_el_tiempo(float segundos);

#endif
