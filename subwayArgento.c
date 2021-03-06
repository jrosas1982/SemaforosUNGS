
#include <stdio.h>	// libreria estandar
#include <stdlib.h> // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>   // para usar threads
#include <semaphore.h> // para usar semaforos
#include <unistd.h>

#define LIMITE 50

// creo estructura de semaforos
struct semaforos
{
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_freir;
	sem_t sem_hornear; 
	sem_t sem_preparar;  
	sem_t sem_final;
	// poner demas semaforos aqui
};

// creo los pasos con los ingredientes
struct paso
{
	char accion[LIMITE];
	char ingredientes[4][LIMITE];
};

// creo los parametros de los hilos
struct parametro
{
	int equipo_param;
	struct semaforos semaforos_param;
	struct paso pasos_param[8];
};

// funcion para imprimir las acciones y los ingredientes de la accion
void *imprimirAccion(void *data, char *accionIn)
{
	struct parametro *mydata = data;
	// calculo la longitud del array de pasos
	int sizeArray = (int)(sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	// indice para recorrer array de pasos
	int i;
	for (i = 0; i < sizeArray; i++)
	{
		// pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if (strcmp(mydata->pasos_param[i].accion, accionIn) == 0)
		{
			printf("\tEquipo %d - accion %s \n ", mydata->equipo_param, mydata->pasos_param[i].accion);
			// calculo la longitud del array de ingredientes
			int sizeArrayIngredientes = (int)(sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]));
			// indice para recorrer array de ingredientes
			int h;
			printf("\tEquipo %d -----------ingredientes : ----------\n", mydata->equipo_param);
			for (h = 0; h < sizeArrayIngredientes; h++)
			{
				// consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion
				if (strlen(mydata->pasos_param[i].ingredientes[h]) != 0)
				{
					printf("\tEquipo %d ingrediente  %d : %s \n", mydata->equipo_param, h, mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
}

// funcion para tomar de ejemplo
void *cortar(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "cortar";
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	usleep(20000);
	// doy la se??al a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_mezclar);

	pthread_exit(NULL);
}

void *mezclar(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "mezclar";
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	usleep(10000);
	// doy la se??al a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_salar);

	pthread_exit(NULL);
}
void *salar(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "salar";
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	sem_wait(&mydata->semaforos_param.sem_salar);

	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	printf("Usando la sal por equipo %d \n ", mydata->equipo_param);
	usleep(50000);

	// doy la se??al a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_salar);
	sem_post(&mydata->semaforos_param.sem_freir);
	pthread_exit(NULL);
}
void *freir(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "freir";
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_freir);
	printf("Friendo equipo %d \n ", mydata->equipo_param);

	imprimirAccion(mydata, accion);
	usleep(50000);

	// doy la se??al a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_freir);
	sem_post(&mydata->semaforos_param.sem_hornear);
	pthread_exit(NULL);
}
void *hornear(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "hornear";
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	sem_wait(&mydata->semaforos_param.sem_hornear);
	printf("Horneando el equipo %d \n ", mydata->equipo_param);
	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	usleep(50000);

	// doy la se??al a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_hornear);
	sem_post(&mydata->semaforos_param.sem_preparar);
	pthread_exit(NULL);
}
void *preparar(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "preparar";
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	printf("Preparando el equipo  %d \n ", mydata->equipo_param);

	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	usleep(50000);

	sem_post(&mydata->semaforos_param.sem_final);

	pthread_exit(NULL);
}
void *final(void *data)
{
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_final);

	// creo el nombre de la accion de la funcion
	char *accion = "final";
	printf("Ganador  el equipo %d \n ", mydata->equipo_param);

	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	usleep(50000);
	pthread_exit(NULL);
}

void *ejecutarReceta(void *i)
{

	// variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_freir;
	sem_t sem_hornear; 
	sem_t sem_preparar;
	sem_t sem_final;
	// crear variables semaforos aqui

	// variables hilos
	pthread_t p1;
	pthread_t p2;
	// crear variables hilos aqui

	// numero del equipo (casteo el puntero a un int)
	int p = *((int *)i);

	printf("Ejecutando equipo %d \n", p);

	// reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	// seteo los valores al struct

	// seteo numero de grupo
	pthread_data->equipo_param = p;

	// seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_freir = sem_freir;
	pthread_data->semaforos_param.sem_hornear = sem_hornear;
	pthread_data->semaforos_param.sem_preparar = sem_preparar;
	pthread_data->semaforos_param.sem_preparar = sem_final;

	// setear demas semaforos al struct aqui

	// seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ??Se ve hardcodeado no? ??Les parece bien?
	strcpy(pthread_data->pasos_param[0].accion, "cortar");
	strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo");
	strcpy(pthread_data->pasos_param[0].ingredientes[1], "perejil");

	strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
	strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
	strcpy(pthread_data->pasos_param[1].ingredientes[2], "huevo");
	strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne");

	strcpy(pthread_data->pasos_param[2].accion, "salar");
	strcpy(pthread_data->pasos_param[2].ingredientes[0], "sal");

	strcpy(pthread_data->pasos_param[3].accion, "freir");
	strcpy(pthread_data->pasos_param[3].ingredientes[0], "aceite");

	strcpy(pthread_data->pasos_param[3].accion, "hornear");
	strcpy(pthread_data->pasos_param[3].ingredientes[0], "panes");

	strcpy(pthread_data->pasos_param[4].accion, "preparar");
	strcpy(pthread_data->pasos_param[4].ingredientes[0], "lechuga");
	strcpy(pthread_data->pasos_param[4].ingredientes[1], "tomate");
	strcpy(pthread_data->pasos_param[4].ingredientes[2], "cebolla");
	strcpy(pthread_data->pasos_param[4].ingredientes[3], "pepino");
	// inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_mezclar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_salar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_freir), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_hornear), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_preparar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_final), 0, 0);



	// inicializar demas semaforos aqui

	// creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos
	int rc;
	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						cortar,		   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						mezclar,	   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						salar,	   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						freir,	   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia

	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						hornear,	   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia	
	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						preparar,	   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia
	rc = pthread_create(&p1,		   // identificador unico
						NULL,		   // atributos del thread
						final,	   // funcion a ejecutar
						pthread_data); // parametros de la funcion a ejecutar, pasado por referencia					
	// crear demas hilos aqui

	// join de todos los hilos
	pthread_join(p1, NULL);
	// crear join de demas hilos

	// valido que el hilo se alla creado bien
	if (rc)
	{
		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	}

	// destruccion de los semaforos
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_freir);
	sem_destroy(&sem_hornear);
	sem_destroy(&sem_preparar);
	// destruir demas semaforos

	// salida del hilo
	pthread_exit(NULL);
}

int main()
{
	// creo los nombres de los equipos
	int rc;
	int *equipoNombre1 = malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 = malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 = malloc(sizeof(*equipoNombre3));
	int *equipoNombre4 = malloc(sizeof(*equipoNombre4));
	// faltan equipos

	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;
	*equipoNombre4 = 4;
	// creo las variables los hilos de los equipos
	pthread_t equipo1;
	pthread_t equipo2;
	pthread_t equipo3;
	pthread_t equipo4;

	// faltan hilos

	// inicializo los hilos de los equipos
	rc = pthread_create(&equipo1,		// identificador unico
						NULL,			// atributos del thread
						ejecutarReceta, // funcion a ejecutar
						equipoNombre1);

	rc = pthread_create(&equipo2,		// identificador unico
						NULL,			// atributos del thread
						ejecutarReceta, // funcion a ejecutar
						equipoNombre2);
	rc = pthread_create(&equipo3,		// identificador unico
						NULL,			// atributos del thread
						ejecutarReceta, // funcion a ejecutar
						equipoNombre3);
	rc = pthread_create(&equipo4,		// identificador unico
						NULL,			// atributos del thread
						ejecutarReceta, // funcion a ejecutar
						equipoNombre4);
	// faltn inicializaciones

	if (rc)
	{
		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	}

	// join de todos los hilos
	pthread_join(equipo1, NULL);
	pthread_join(equipo2, NULL);
	pthread_join(equipo3, NULL);
	pthread_join(equipo4, NULL);
	//.. faltan joins

	pthread_exit(NULL);
}

// Para compilar:   gcc subwayArgento.c -o ejecutable -lpthread
// Para ejecutar:   ./ejecutable
