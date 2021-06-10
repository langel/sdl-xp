oh yeah mucking about SDL2 for fun and profit

finally got this compiling on windows w/o an ide
i highly suggest mingw and not compiling sdl for cygwin
dev-c++ looks like it'd work too ( https://www.bloodshed.net/ )


-- will i have to fix mac compiling now?!?!


notes for non square pixel rendering ideas:

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

cool demo alert:
circle half height of screen moving left/right & up/down on sine waves
random pixel density updated once a second?
needs a tiled or checkered background
