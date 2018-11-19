#include "gestor_notas.h"

void imprimir_banner()
{
	system("clear");
	printf("##################################################\n");
	printf("| Bienvenido al Servidor de Notas de Alumnos v.Pro |\n");
	printf("##################################################\n\n");
}

void imprimir_error(char* mensaje)
{
	printf("%s\n\n", mensaje);
}

void profesor_checkin(int* cantidad_clientes)
{
	(*cantidad_clientes) += 1;
}

void profesor_checkout(int* cantidad_clientes)
{
	(*cantidad_clientes) -= 1;
}

int alta_nota(void* query, FILE* arch_bd, char path[100])
{
	struct registro_bd* alumno = (struct registro_bd*)query;
	if (alumno->dni > 99999999 || alumno->dni < 1){
		imprimir_error("ERROR -> DNI no válido.");
		return ERROR_CARGA_ARCHIVO;
	}

	//Abrir archivo
	arch_bd = fopen(path, "a");
	if(!arch_bd)
		exit(ERROR_APERTURA_ARCHIVO);
	//grabar alumno en archivo en el formato dni;nombre_materia;tipo_evaluacion;nota
	fseek(arch_bd, 0, SEEK_END);
	fprintf(arch_bd, "\n%ld;%s;%s;%.2f", alumno->dni, alumno->nombre_materia, alumno->tipo_evaluacion, alumno->nota[0]);
	struct registro_bd alumno_local = *alumno;
	alumno_local.instruccion = 'z';
	memcpy(query, &alumno_local, sizeof(struct registro_bd));
	//Cerrar archivo
	fclose(arch_bd);
	return EXIT_SUCCESS;
}

struct registro_bd parsear_alumno(char* registro, char delimitador[2])
{
	struct registro_bd alumno_arch;
	char* token = strtok(registro, delimitador);
	int i = 0;

	//se declara una estructura, y un token, y se recorre por ese token cada registro del archivo.
	while(token != NULL)
	{
		if (i==0)
		{
			alumno_arch.dni = atoi(token);
		}else if (i == 1)
		{
			strcpy(alumno_arch.nombre_materia, token);
		}else if (i == 2)
		{
			strcpy(alumno_arch.tipo_evaluacion, token);
		}else if (i == 3)
		{
			alumno_arch.nota[0] = atof(token);
		}
		token = strtok(NULL, delimitador);
		i++;
	}

	return alumno_arch;
}

void query_promedios_alumno(void* query, FILE* arch_bd, char path[100])
{
	struct registro_bd* alumno;
	struct registro_bd alumno_local;

	//instancia de la estructura creada para borrar basura.
	alumno = (struct registro_bd*)query;
	alumno_local.dni = alumno->dni;
	strcpy(alumno_local.nombre_materia, alumno->nombre_materia);
	alumno_local.nota[0] = 0;
	alumno_local.nota[1] = 0;
	alumno_local.nota[2] = 0;
	alumno_local.instruccion = 'l';
	int cont_materias = 0;
	int cont_instancias = 0;

	//Abrir archivo
	arch_bd = fopen(path, "r+");
	if(!arch_bd)
		exit(ERROR_APERTURA_ARCHIVO);
	char line[MAX_LINE];

	//Consultar por el dni y obtener todos los promedios de ese alumno.
	while (fgets(line, MAX_LINE, arch_bd) != NULL)
	{
		struct registro_bd alumno_arch = parsear_alumno(line, ";");
		if (alumno_arch.dni == alumno_local.dni)
		{
			alumno_local.nota[2] += alumno_arch.nota[0];
			cont_materias++;
			if (strcmp(alumno_arch.nombre_materia, alumno_local.nombre_materia) == 0)
			{
				alumno_local.nota[1] += alumno_arch.nota[0];
				cont_instancias++;
			}
		}
	}
	alumno_local.nota[2] = alumno_local.nota[2] / cont_materias;
	alumno_local.nota[1] = alumno_local.nota[1] / cont_instancias;
	memcpy(query, &alumno_local, sizeof(struct registro_bd));
	//Cerrar archivo
	fclose(arch_bd);
}

void intHandler(int signal) {
	if (signal == SIGINT)
    	keepRunning = false;
}

int main(int argc, char const *argv[])
{
	//declaración de variables.
	int cantidad_clientes = 0;
	int entrada = 0;
	char path[100] = "./bd_alumnos.txt"; 
	FILE* arch_bd = NULL;
	sem_t* mutex_i;
	sem_t* mutex_o;
	sem_t* mutex_d;
	signal(SIGINT, intHandler);
	int fd;
	long dni;
	char shm_instruction;
 	struct registro_bd* query;

	//instancia de variables.
	dni = 0;
	shm_instruction = 'z';
	query = NULL;
 	mutex_i = sem_open("mutex_i", O_CREAT, 0660, 1);
 	mutex_o = sem_open("mutex_o", O_CREAT, 0660, 1);
 	mutex_d = sem_open("mutex_d", O_CREAT, 0660, 1);
 	
	sem_wait(mutex_i);
	imprimir_banner();

    //otorga la llave de la, memoria compartida, quienes tengan la misma llave compartiran la memoria compartida.
    fd = shm_open(NAME_SHM, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd < 0){
		sem_post(mutex_i);
		sem_close(mutex_i);
		sem_close(mutex_o);
		sem_close(mutex_d);
		sem_unlink("mutex_i");
		sem_unlink("mutex_o");
		sem_unlink("mutex_d");
    	imprimir_error("Error al tratar de crear/ejecutar la memoria compartida.");
    }

    ftruncate(fd, SIZE_SHM);

	//se vincula la estructura local a la memoria compartida.
    query = (struct registro_bd*)mmap(0, SIZE_SHM, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
 
	//mientras no reciba una señal de finalización sigue en el ciclo.
    while(keepRunning)
    {	
		//consulta por la estructura del area compartida.
    	if (query != NULL )
   		{
   			shm_instruction = query->instruccion;
			//filtra por la instrucción dentro de la shm, para saber que consultar/accionar.
    		switch(shm_instruction)
    		{
    			case 'r':
    				printf("\nRegistrando ingreso, Profesor de %s\n", query->nombre_materia);
    				profesor_checkin(&cantidad_clientes);
    				entrada++;
    				sem_post(mutex_o);
    				query->instruccion = '0';
    			break;
    			case 'a':
    				alta_nota(query, arch_bd, path);
    				sem_post(mutex_o);
    				break;
    			case 'p':
    				query_promedios_alumno(query, arch_bd, path);
    				sem_post(mutex_o);
    				break;
    			case 's':
    				printf("\nRegistrando salida, Profesor de %s.\n", query->nombre_materia);
    				profesor_checkout(&cantidad_clientes);
    				sem_post(mutex_o);
    				query->instruccion = '0';
    				break;
    		}
    		if (query->instruccion == 'e')
    		{
    			break;
    		}
    		if (cantidad_clientes == 0 && entrada > 0)
    		{	
    			munmap(query,SIZE_SHM);
    			shm_unlink(NAME_SHM);
    			close(fd);
    			printf("\nSin usuarios conectados, cerrando el servidor compartido...\n");
    			break;
    		}
    	}else
    	{
			//libera recursos.
    	    munmap(query,SIZE_SHM);
   			shm_unlink(NAME_SHM);
   			close(fd);
   			printf("\nCerrando el servidor compartido...\n");
    		sem_post(mutex_i);
			sem_close(mutex_i);
			sem_close(mutex_o);
			sem_close(mutex_d);
   		 	sem_unlink("mutex_i");
			sem_unlink("mutex_o");
			sem_unlink("mutex_d");
			return EXIT_FAILURE;
    	}
    }
	//libera recursos.
    munmap(query,SIZE_SHM);
   	shm_unlink(NAME_SHM);
   	close(fd);
   	printf("\nCerrando el servidor compartido...\n");
    sem_post(mutex_i);
	sem_close(mutex_i);
	sem_close(mutex_o);
	sem_close(mutex_d);
    sem_unlink("mutex_i");
	sem_unlink("mutex_o");
	sem_unlink("mutex_d");
	return EXIT_SUCCESS;
}