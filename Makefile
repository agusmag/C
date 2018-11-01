all: servidor_notas_pro gestor_notas_pro

servidor_notas_pro: servidor_notas_pro.o gestor_notas.h
		gcc -Iinclude servidor_notas_pro.o -o servidor_notas_pro -lrt -pthread

gestor_notas_pro: gestor_notas_pro.o gestor_notas.h
		gcc -Iinclude gestor_notas_pro.o -o gestor_notas_pro -lrt -pthread

servidor_notas_pro.o: servidor_notas_pro.c
		gcc -Iinclude -c servidor_notas_pro.c -o $@

gestor_notas_pro.o: gestor_notas_pro.c
		gcc -Iinclude -c gestor_notas_pro.c -o $@

clean:
	rm -f gestor *.o 