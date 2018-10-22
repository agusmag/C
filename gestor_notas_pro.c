#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct registro_bd {
  long dni;
  char nombre_materia[50];
  char tipo_evaluacion[30];
  float nota;
};

void imprimir_banner(const char* materia_actual)
{
	system("clear");
	printf("##################################################\n");
	printf("| Bienvenido al Gestor de Notas de Alumnos v.Pro |\n");
	printf("##################################################\n\n");

	printf("Materia: %s\n\n", materia_actual);
}

int imprimir_error(char* mensaje)
{
	printf("%s\n\n", mensaje);
	
	return EXIT_FAILURE;
}

void imprimir_menu_general(const char* materia_actual)
{
	system("clear");
	imprimir_banner(materia_actual);
	printf("Menu de opciones:\n");
	printf("-----------------\n\n");
	printf("1. Ver registros.\n");
	printf("2. Agregar nota de alumno.\n");
	printf("3. Consultar promedio de notas de alumno.\n");
	printf("4. Salir.\n\n");
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

void buscar_registros_por_materia(const char* materia_actual, FILE* arch_bd)
{

}

void cargar_alumno_en_base(struct registro_bd* alumno, FILE* arch_bd)
{

}

void mostrar_notas_alumnos(const char* materia_actual, FILE* arch_bd)
{
	int opt=0;
	system("clear");
	imprimir_banner(materia_actual);
	printf("Menu de opciones [Ver registros de %s]:\n", materia_actual);
	printf("---------------------------------------");
	int i=0;
	while(i < sizeof(materia_actual)-1){
		printf("-");
		i++;
	}
	printf("\n\n");
	printf("Registro de Notas:\n");
	printf("------------------\n\n");
	printf(" _________________________________________________________________________\n");
	printf("|     DNI    |         Nombre Materia         |     Evaluación     | Nota |\n");
	printf("|------------|--------------------------------|--------------------|------|\n");
	buscar_registros_por_materia(materia_actual, arch_bd);
	printf("|____________|________________________________|____________________|______|\n\n\n");

	printf("1. Volver al menú principal.");
	printf("? ");
	fflush(stdin);
	scanf("%d,", &opt);
	validar_opt_int(&opt, 1, 1);
}

float promedio_notas_por_alumno(struct registro_bd* alumno, FILE* arch_bd)
{
	float promedio_gen = 0;

	return promedio_gen;
}

void agregar_nota_alumno(const char* materia_actual, FILE* arch_bd)
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
	scanf("%f", &alumno.nota);
	validar_opt_float(&alumno.nota, 1, 10);

	cargar_alumno_en_base(&alumno, arch_bd);
	printf("\nLa nota: %.2f, de la evaluación: %s,fue cargada exitosamente al alumno %ld en la materia %s.\n\n", alumno.nota,
																   alumno.tipo_evaluacion, alumno.dni, alumno.nombre_materia);

	printf("1. Volver al menú principal.");
	printf("? ");
	fflush(stdin);
	scanf("%d,", &opt2);
	validar_opt_int(&opt2, 1, 1);
}

void promediar_notas_alumno(const char* materia_actual, FILE* arch_bd)
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
	printf("Promedio por materia:\n");
	printf("---------------------\n");
	printf(" ___________________________\n");
	printf("|     Materia    | Promedio |\n");
	printf("|----------------|----------|\n");
	promedio_gen = promedio_notas_por_alumno(&alumno, arch_bd);
	printf("|________________|__________|\n");
	printf("Promedio general: %.2f\n", promedio_gen);
	printf("----------------------\n\n");

	printf("1. Volver al menú principal.");
	printf("? ");
	fflush(stdin);
	scanf("%d,", &opt);
	validar_opt_int(&opt, 1, 1);
}

int main(int argc, char const *argv[])
{	
	int opt=0;
	imprimir_banner(argv[1]);
	
	if (argc < 2) 
	{
		if (imprimir_error("No se ingresó la materia.") == EXIT_FAILURE) return EXIT_FAILURE;
	}

	int key = 9876; //Se crea la clave para la memoria compartida.
	int shmid; //Se crea el id que tendrá la memoria compartida.
	char *shm; //Se crea el contenido de la memoria compartida.
	FILE *bd_notas; //Se crea el puntero al archivo de notas de los alumnos.

	//shmid = shmget(key, sizeof(registro_bd), 0775); //Trato de creat la memoria compartida.

	while(opt != 4)
	{
		imprimir_menu_general(argv[1]);
		scanf("%d", &opt);
		validar_opt_int(&opt, 1, 4);
		switch(opt){
			case 1:
				mostrar_notas_alumnos(argv[1], bd_notas);
				break;
			case 2:
				agregar_nota_alumno(argv[1], bd_notas);
				break;
			case 3:
				promediar_notas_alumno(argv[1], bd_notas);
				break;
			case 4:
				return EXIT_SUCCESS;
				break;
		}
	}
	return EXIT_SUCCESS;
}

