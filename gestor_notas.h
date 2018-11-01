#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Para memoria compartida
#include <sys/shm.h>
#include <sys/mman.h>

//Para sincronización
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

//Para señales
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>

#define ERROR_APERTURA_ARCHIVO 21
#define ERROR_CARGA_ARCHIVO 22
#define MAX_LINE 500

#define ERROR_CLAVESHM 12
#define MEM_NOLISTA 13
#define MEM_CARGADA 0
#define MEM_TOMADA 15

#define SIZE_SHM 31457280 //30 MB
#define NAME_SHM "notas-alumnos"

struct registro_bd {
  long dni;
  char nombre_materia[50];
  char tipo_evaluacion[30];
  float nota[3];
  char instruccion;
};

/* GUIA DE INSTRUCCIONES

a -> alta de nota de un alumno.
p -> promedio de notas, general y por materia, de un alumno.
e -> fin del server, ya no hay clientes.
s -> fin de cliente, se informa desconexion.
l -> Se le envía l al cliente cuando el promedio ya está listo.
*/

static volatile bool keepRunning = true;