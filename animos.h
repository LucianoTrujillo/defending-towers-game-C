#ifndef __ANIMOS_H__
#define __ANIMOS_H__

#include "commons.h"

/* Interatúa con el usuario para calcular los valores iniciales de la aventura
// pre: recibe las referencias de memoria de los valores a calcular
// pos: guardará la información adecuada y dentro de los rangos posibles en cada variable*/
void animos(int* viento , int* humedad , char* animo_legolas , char* animo_gimli, configuracion_t configuracion);

#endif /* __ANIMOS_H__ */
