#include <stdio.h>
#include <stdlib.h>

void imprimir_banner(){
	system("clear");
	printf("##################################################\n");
	printf("| Bienvenido al Gestor de Notas de Alumnos v.Pro |\n");
	printf("##################################################\n\n");
}

int main(int argc, char const *argv[])
{
	imprimir_banner();

	return EXIT_SUCCESS;
}

