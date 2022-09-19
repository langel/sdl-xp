:LOOP
mingw32-make
main.exe
IF EXIST "devpipe_death" (
	GOTO DEATH
) ELSE (
	GOTO LOOP
)
:DEATH
del devpipe_death
