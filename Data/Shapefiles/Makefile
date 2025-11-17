all: grid_eval dump_shape read_shapefile2

clean: 
	rm -f *.o grid_eval

read_shapefile2: read_shapefile2.c
	$(CC) -o read_shapefile2 read_shapefile2.c -lshp

grid_eval.o: grid_eval.c shape.h

shape.o: shape.c shape.h

dump_shape.o: dump_shape.c shape.h

dump_shape: dump_shape.o shape.o
	$(CC) -o dump_shape dump_shape.o shape.o

inside.o: inside.c

grid_eval: grid_eval.o inside.o shape.o
	$(CC) -o grid_eval grid_eval.o inside.o shape.o -lshp
