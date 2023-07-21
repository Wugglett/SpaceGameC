main: *.c *.h
	gcc -W -Wall -Wextra -pedantic -o main *.c *.h -Isrc/include -lpthread -Lsrc/lib -lSDL2 -lSDL2main -Isrc/include-image -Lsrc/lib-image -lSDL2_image -Isrc/include-mixer -Lsrc/lib-mixer -lSDL2_mixer

clean:
	rm *.exe