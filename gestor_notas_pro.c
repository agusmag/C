#include "gestor_notas.h"

void obtener_ayuda()
{
	system("clear");
	printf("Ayuda Ejercicio 5 \n");
	printf("Entrega N°1\nFecha: 11/11/2018\n");
	printf("Descrión:\nSe pide realizar una versión local del sistema de notas que se comunique vía memoria compartida, utilizando la misma base de datos que el sistema remoto\n");
	printf("Forma de ejecución:\nSe llama de la siguiente manera ./gestor_notas_pro [nombre_materia]\n");
	printf("\nAUTORES\n");
	printf("Arias Pablo Alejandro       - DNI 32.340.341\n");
	printf("Feito Gustavo Sebastian     - DNI 27.027.190\n");
	printf("Magliano, Agustin           - DNI 39.744.938\n");
	printf("Rosmirez Juan Ignacio       - DNI 40.010.264\n");
	printf("Zambianchi Nicolas Ezequiel - DNI 39.770.752\n\n");
	exit(1);
}

void imprimir_banner(const char* materia_actual)
{
	system("clear");
	printf("##################################################\n");
	printf("| Bienvenido al Gestor de Notas de Alumnos v.Pro |\n");
	printf("##################################################\n\n");

	printf("Materia: %s\n\n", materia_actual);
}

void imprimir_error(char* mensaje)
{
	printf("%s\n\n", mensaje);
	exit(EXIT_FAILURE);
}

void imprimir_menu_general(const char* materia_actual)
{
	system("clear");
	imprimir_banner(materia_actual);
	printf("Menu de opciones:\n");
	printf("-----------------\n\n");
	printf("1. Agregar nota de alumno.\n");
	printf("2. Consultar promedio de notas de alumno.\n");
	printf("3. Salir.\n\n");
}

void validar_opt_string(char* opt, char min, char max)
{
	while ((getchar()) != '\n');
	*opt = getchar();
	while (*opt < min || *opt > max)
	{
		printf ("\nERROR -> ingrese una opcion valida!: ");
		while ((getchar()) != '\n');
		*opt = getchar();
	}
}

void validar_opt_long(long* num, long min, long max)
{
	while(*num < min || *num > max)
	{
		printf("ERROR -> Ingrese una opcion valida!: ");
		fflush(stdin);
		scanf("%ld", num);
	}
}

void validar_opt_int(int* num, int min, int max)
{
	while(*num < min || *num > max)
	{
		printf("ERROR -> Ingrese una opcion valida!: ");
		fflush(stdin);
		scanf("%d", num);
	}
}

void validar_opt_float(float* num, float min, float max)
{
	while(*num < min || *num > max)
	{
		printf("ERROR -> Ingrese una opcion valida!: ");
		fflush(stdin);
		scanf("%2f", num);
	}
}

void enviar_checkin(void* shm_ptr, const char* materia_actual)
{
	struct registro_bd alumno;
	alumno.instruccion = 'r';
	strcpy(alumno.nombre_materia, materia_actual);
	memcpy(shm_ptr, &alumno, sizeof(struct registro_bd));
}

void cargar_nota_en_base(struct registro_bd alumno, void* shm_ptr)
{
	//Informar al servidor que de de alta una nota a un alumno especificado en la materia actual.
	//Se copia el struct del alumno a la shm, con instruccion en a -> alta.
	 memcpy(shm_ptr, &alumno, sizeof(struct registro_bd));
}

struct registro_bd promedio_notas_por_alumno(struct registro_bd alumno, void* shm_ptr)
{
	//Consultarle al servidor todos los promedios que tiene el alumno por materia y general.
	//Se copia el struct del alumno a la shm, con la instruccion de p -> promedio.
	memcpy(shm_ptr, &alumno, sizeof(struct registro_bd));

	//Se crea una referencia del puntero al struct para consultar si hubo cambios.
	struct registro_bd* alumno_local = (struct registro_bd*)shm_ptr;
	//Despues tengo que leer y guardar los datos devueltos en el array de notas del alumno.
	int timeout = 0;
	while(1)
	{
		if (alumno_local->instruccion == 'l' || timeout > 30)
		{
			break;
		}
		else
		{
			sleep(1); //Espera 1 segundo.
			printf ("Informacion no disponible, reintentando nuevamente... %d restantes.\n", 30 - timeout);
			timeout++;
		}
	}
	alumno = *alumno_local;
	return alumno;
}

void agregar_nota_alumno(const char* materia_actual, void* shm_ptr)
{
	struct registro_bd alumno;
	int opt2=0;
	char opt;
	system("clear");
	imprimir_banner(materia_actual);
	strcpy(alumno.nombre_materia, materia_actual);
	printf("Menu de opciones [Agregar una nota a alaumno]:\n");
	printf("----------------------------------------------\n\n");
	printf("Indique el dni del alumno: ");
	fflush(stdin);
	scanf("%ld,", &alumno.dni);
	validar_opt_long(&alumno.dni, 1, 999999999);
	printf ("\nIndique la evaluación: \n");
	printf("\ta. Primer Parcial.\n");
	printf("\tb. Segundo Parcial.\n");
	printf("\tc. Recuperatorio.\n");
	printf("? ");
	fflush(stdin);
	validar_opt_string(&opt, 'a', 'c');
	switch(opt){
		case 'a':
			strcpy(alumno.tipo_evaluacion, "Primer Parcial");
		break;
		case 'b':
			strcpy(alumno.tipo_evaluacion, "Segundo Parcial");
		break;
		case 'c':
			strcpy(alumno.tipo_evaluacion, "Recuperatorio");
		break;
	}
	printf("\nIndique la nota de la materia: ");
	fflush(stdin);
	scanf("%f", &alumno.nota[0]);
	validar_opt_float(&alumno.nota[0], 1, 10);
	alumno.instruccion = 'a';

	cargar_nota_en_base(alumno, shm_ptr);
	printf("\nLa nota: %.2f, de la evaluación: %s,fue cargada exitosamente al alumno %ld en la materia %s.\n\n", alumno.nota[0],
																   alumno.tipo_evaluacion, alumno.dni, alumno.nombre_materia);

	printf("1. Volver al menú principal.");
	printf("? ");
	fflush(stdin);
	scanf("%d,", &opt2);
	validar_opt_int(&opt2, 1, 1);
}

void promediar_notas_alumno(const char* materia_actual, void* shm_ptr)
{
	int opt=0;
	struct registro_bd alumno;
	float promedio_gen = 0;
	system("clear");
	imprimir_banner(materia_actual);
	printf("Menu de opciones [Promedio de notas de alumno]:\n");
	printf("-----------------------------------------------\n\n");
	printf("Ingrese un D.N.I de un alumno: ");
	fflush(stdin);
	scanf("%ld", &alumno.dni);
	validar_opt_long(&alumno.dni, 1, 99999999);
	strcpy(alumno.nombre_materia, materia_actual);
	strcpy(alumno.tipo_evaluacion, "");
	alumno.instruccion = 'p';
	alumno = promedio_notas_por_alumno(alumno, shm_ptr);
	if (alumno.instruccion != 'l')
	{
		printf ("Hubo un problema al calcular los promedios. Lo siento!\n\n");
		exit(ERROR_CARGA_ARCHIVO);
	}
	printf("\nPromedio(General y de %s.)\n", materia_actual);
	printf("---------------------\n");
	printf(" ___________________________\n");
	printf("|          Promedio         |\n");
	printf("|---------------------------|\n");
	if (alumno.nota[2] > 0)
		printf("| General:         %.2f     |\n", alumno.nota[2]);
	else
		printf("| General:         0.00     |\n");
	printf("|---------------------------|\n");
	if (alumno.nota[1] > 0)
		printf("| Materia act:     %.2f     |\n", alumno.nota[1]);
	else
		printf("| Materia act:     0.00     |\n");
	printf("|___________________________|\n\n");
	printf("1. Volver al menú principal.");
	printf("? ");
	fflush(stdin);
	scanf("%d,", &opt);
	validar_opt_int(&opt, 1, 1);
}

void dejar_servidor(const char profesor[50], void* shm_ptr)
{
	struct registro_bd alumno;
	alumno.instruccion = 's';
	strcpy(alumno.nombre_materia, profesor);
	memcpy(shm_ptr, &alumno, sizeof(struct registro_bd));
}

int main(int argc, char const *argv[])
{	
	int opt=0;
	char usr[50] = "";
	sem_t* mutex_d;
	sem_t* mutex_o;

	//Obtencion de la ayuda 
    if(strcmp(argv[1],"-h")==0 || strcmp(argv[1],"h")==0)
        obtener_ayuda();
    if(strcmp(argv[1],"-help")==0 || strcmp(argv[1],"help")==0)
        obtener_ayuda();
    if(strcmp(argv[1],"-?")==0 || strcmp(argv[1],"?")==0)
        obtener_ayuda();


    mutex_d = sem_open("mutex_d", O_CREAT, 0660, 1);
    mutex_o = sem_open("mutex_o", O_CREAT);

	imprimir_banner(argv[1]);

    if (argc < 2) 
	{
		imprimir_error("No se ingresó la materia.");
	}

 	int fd;
 	void* shm_ptr;

    //Otorga la llave de la, memoria compartida, quienes tengan la misma llave compartiran la memoria compartida.
    fd = shm_open(NAME_SHM, O_RDWR, 0666);
    if (fd < 0){
    	imprimir_error("Error al tratar de unirse a la memoria compartida.");	
    }

    ftruncate(fd, SIZE_SHM);

    shm_ptr = mmap(NULL, SIZE_SHM, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
    	imprimir_error("Error al tratar de asignar la memoria compartida");
    }
	
	strcpy(usr, "Profesor de ");
	strcat(usr, argv[1]);
	
	sem_wait(mutex_d);
	sem_wait(mutex_o);
	enviar_checkin(shm_ptr, argv[1]);
	sem_post(mutex_d);

	while(opt != 3)
	{
		imprimir_menu_general(argv[1]);
		scanf("%d", &opt);
		validar_opt_int(&opt, 1, 3);
		switch(opt){
			case 1:
				sem_wait(mutex_d);
				sem_wait(mutex_o);
				agregar_nota_alumno(argv[1], shm_ptr);
				munmap(NAME_SHM, SIZE_SHM);
				sem_post(mutex_d);
				break;
			case 2:
				sem_wait(mutex_d);
				sem_wait(mutex_o);
				promediar_notas_alumno(argv[1], shm_ptr);
				munmap(NAME_SHM, SIZE_SHM);
				sem_post(mutex_d);
				break;
			case 3:
				//Desasociar el proceso actual a la memoria compartida.
				sem_wait(mutex_d);
				sem_wait(mutex_o);
				dejar_servidor(usr, shm_ptr);
				sem_post(mutex_d);
				break;
		}
	}
	return EXIT_SUCCESS;
}
