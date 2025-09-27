all: distance index_file_offsets read_from_offset

read_from_offset: read_from_offset.c
	cc -o read_from_offset read_from_offset.c

index_file_offsets: index_file_offsets.c read_line_offsets.h
	cc -o index_file_offsets index_file_offsets.c

distance: distance.o
	cc -o distance distance.o -lm

distance.o: distance.c parse_csv.h
	cc -c distance.c
