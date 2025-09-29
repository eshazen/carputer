executables = distance index_file_offsets read_from_offset make_working_files

all: ${executables}

clean: 
	rm -f *.o ${executables} *~ \#*

try_grid_size: try_grid_size.o places.o parse_csv.o
	cc -o try_grid_size $^ -lm

try_grid_size.o: try_grid_size.c places.h load_places.h

make_working_files: make_working_files.o places.o parse_csv.o file_util.o
	cc -o make_working_files $^ -lm

file_util.o: file_util.c file_util.h

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

make_working_files.o: make_working_files.c places.h load_places.h file_util.h

parse_csv.o: parse_csv.c parse_csv.h
	cc -c parse_csv.c

places.o: places.c places.h
	cc -c places.c

read_from_offset: read_from_offset.c
	cc -o read_from_offset $^

index_file_offsets: index_file_offsets.c read_line_offsets.h
	cc -o index_file_offsets index_file_offsets.c

distance: distance.o parse_csv.o
	cc -o distance distance.o parse_csv.o -lm

distance.o: distance.c
	cc -c distance.c

