sdl-prog:
	gcc -o rainbowball-game sdl-rainbowball.c -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
clean:
	rm rainbowball-game
