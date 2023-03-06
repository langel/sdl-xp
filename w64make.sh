#!/bin/bash
while [ ! -f devpipe_death ]
do
	gcc \
	main.c -o main.exe \
	-std=c99 \
	$(sdl2-config --cflags --static-libs) \
	-lSDL2_image
	main.exe
done
rm devpipe_death
