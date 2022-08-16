oh yeah mucking about SDL2 for fun and profit

## windows
- finally got this compiling on windows w/o an ide
- probably went with the wrong mingw : https://sourceforge.net/projects/mingw/ (does not support 64bit compiling with SDL2_image loading pngs!? :shrug:)
- this guy explains the mingw/sdl install : https://www.youtube.com/watch?v=UvJt9MZs_M8
- 32 bit dlls!!!  (i686 == 32 bit duh!!!)
- `make.bat` builds and executes mian.c

## osx
- `brew install SDL2` `brew install SDL2_image` etc
- `./build_osx.sh` to build and run main.c
(magically comes out 64 bit wowow)

## linux
:shrug: need to setup a machine or vm


to-do:
- clipboard
- particle system
- saving window position/size to config
- window events https://wiki.libsdl.org/SDL_WindowEvent


fcl/ - fantasy console lib
fac/ - fantasy audio chip? jajajaa


## non-square pixel thought stream

4/3 or 12/9  vs 16/9

320x200 has a 1.6 aspect ratio 

However, when the resolution was used all displays were 4:3, and most users would stretch the 200 vertical lines to fill up the screen.  Instead of perfectly square pixels, you would get pixels 1.2 times as vertical compared with the horizontal width on a monitor where the vertical width had been stretched to the edges of the monitor.

(width1 / height1) / (width2 / height2) = pixel ratio?

320x200 = 8/5 but its for a 4/3 screen has a 1.2 pixel aspect ratio

420x200 = 21/10 but its for a 16/9 screen has a 1.18125(?) pixel aspect ratio

426x200 = 213/100 but its for a 16/9 screen has a 1.198125(?) pixel aspect ratio

427x200 = 427/200 but its for a 16/9 screen has a 1.2009375(?) pixel aspect ratio

432x200 = 54/25 but its for a 16/9 screen has a 1.215(?) pixel ratio

480x200 = 12/5 but its for a 16/9 screen has a 1.35(?) pixel aspect ratio

720x200 = 18/5 but its for a 16/9 screen has a 2.025(?) pixel aspect ratio

https://en.wikipedia.org/wiki/Pixel_aspect_ratio

Pixel Aspect Ratio = Display Aspect Ratio / Storage Aspect Ratio

320x200 = 8:5 / 4:3 = 1.2

320x200 = 8:5 / 16:9 = 0.9

420x200 = 21:10 / 16:9 = 1.18125

Quad the Size (4x) : 1680x800

hardware limitations: resizing the pixel density costs one visible frame of the old pixel density in the new pixel density; if new one is bigger missing data is filled with noise

### 4 Video Modes?
each mode has 84,000 pixels and a different display aspect ratio and pixel aspect ratio

| id | width | height | dar | par | potential tile size | tile fill |
| --- | --- | --- | --- | --- |
| 0 | 420 | 200 | 21/10 | 1.18125 | 10x10 | 42x20 |
| 1 | 400 | 210 | 40/21 | 1.07143 | 10x10 | 40x21 |
| 2 | 375 | 224 | gross | 0.94168 | 15x16 | 25x14 |
| 3 | 350 | 240 | 35/24 | 0.82031 | 10x16 | 35x15 | 


