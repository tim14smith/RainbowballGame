sdl-prog:
	gcc -o rainbowball-game sdl-rainbowball.c -lSDL -lSDL_image
clean:
	rm rainbowball-game
