build:
	gcc -Wall \
	-std=c99 \
	./main.c \
	-I"C:\libsdl\include" \
	-L"C:\libsdl\lib" \
	-lmingw32 \
	-lSDL2main \
	-lSDL2 \
	-lSDL2_ttf \
	-lSDL2_image \
	-o main.exe
