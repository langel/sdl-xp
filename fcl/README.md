placeholder text for ideas around this project

## Audio

- ability to set base clock
	- pokey has 15kHz and 64kHz (and 1.79mHz?)
	- tia has 30kHz
	- vic20 has NTSC: 14318181/14 Hz or PAL: 4433618/4 Hz
	- sms psg @ 3.579545
	- genesis ym @ 53.267 kHz (NTSC), 52.781 kHz (PAL)
	- nes @ 
- set clock division bit depth
	- pokey has 8bit
	- tia has 5bit
	- vic20 has 7bit (but additional divisors per channel)
	- sms psg 12bit
	- genesis ym @
	- nes @ 11bit
- 8 audio channels of various abilities
	- various oscillator modes
		- 2 oscillator mode
			- 4 selectable waveforms (sqr/saw/tri/sin)
			- 2nd osc fine or coarse tune relative to 1st
		- 2op fm mode
		- 8bit pcm playback
		- noise mode (lfsr)
	- adsr envelope (exp/log/lin/rlog)
	- output routing
- a small selection of effects
	- 2 delays (2 mono or combine for stereo)
	- 2 multimode filter (low/band/high/notch)
	- (could maybe have 8 effects total to match mixer inputs?)
- routing options / mixer
	- 8 inputs with vol/pan
	- effects send
	- master volume


## Video

- single program window
	- dimensions can be whatever
	- fullscreen letterboxes mismatched dimensions
- single output canvas
	- dimensions have limitations?
	- automatically scaled / stretched to fit window
	- 256 indexed colors palette 
		- 24bit RGB values
		- color 255 is transparent?
	- 256 on screen sprites
		- max size: 256x256
		- x,y pos off screen
		- x,y origin (pos doesn't have to be top-left of sprite)
		- collision detection
	- background/foreground tiling layers?
	- fps setting (affects game/audio loops? probably!)
	- retro shader effects (CRT curve / scanlines)

## Inputs
- read controller without window focus
