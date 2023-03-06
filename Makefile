build:
	gcc \
	-Wall \
	main.c -o main.exe \
	-std=c99 \
	-IC:/Users/puke7/source/w64devkit/include/SDL2 -Dmain=SDL_main -LC:/Users/puke7/source/w64devkit/lib -lmingw32 -lSDL2main -lSDL2 -mwindows \
	-lSDL2_image
