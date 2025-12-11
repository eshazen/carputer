execs = grid_eval dump_shape read_shapefile2 \
build_tree dump_rtree_file find_by_tree

all: $(execs)

CC = gcc

clean: 
	rm -f *.o $(execs)

headers = shape.h filetree.h rtree.h

objects = rtree.o grid_eval.o shape.o read_shapefile2.o \
dump_shape.o inside.o build_tree.o dump_rtree_file.o find_by_tree.o \
fake_data.o

$(objects): %.o: %.c $(headers)
	$(CC) -c $(CFLAGS) $<

# fake_data: fake_data.o shape.o test_file_reader.o
# 	$(CC) -o $@ $^

dump_rtree_file: dump_rtree_file.o shape.o
	$(CC) -o $@ $^

find_by_tree: find_by_tree.o shape.o
	$(CC) -o $@ $^

build_tree: build_tree.o rtree.o shape.o
	$(CC) -o $@ $^ -lshp

read_shapefile2: read_shapefile2.o
	$(CC) -o $@ $^ -lshp

dump_shape: dump_shape.o shape.o
	$(CC) -o $@ $^ -lshp

grid_eval: grid_eval.o inside.o shape.o
	$(CC) -o $@ $^ -lshp
