

all:
	# gcc -g -Wall -Wextra gsaca.c sa.c -o main
	gcc -DNDEBUG -Wall -Wextra -O3 -ffast-math -funroll-loops gsaca.c sa.c -shared -o out/sa.so
