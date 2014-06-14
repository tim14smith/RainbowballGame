sdl-prog:
	gcc -o sdl-prog sdl-rainbowball.c -lSDL -lSDL_image
clean:
	rm sdl-prog
