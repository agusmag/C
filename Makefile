
all: gestor_notas

gestor_notas: servidor_notas_pro.o gestor_notas_pro.o
	gcc -o -g gestor_notas servidor_notas_pro.o gestor_notas_pro.o -lrt -pthread

servidor_notas_pro.o: servidor_notas_pro.c gestor_notas.h
		gcc -c servidor_notas_pro.c

gestor_notas_pro.o: gestor_notas_pro.c gestor_notas.h
		gcc -c gestor_notas_pro.c

clean:
	rm -f gestor_notas *.o 