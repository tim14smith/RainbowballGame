// sdl-rainbowball.c - basic demo of an SDL window with a moveable ball
// Author: Tim Smith (tim14smith@gmail.com)

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <stdbool.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
char * IMAGE_FILE_NAME = "dots.png";

SDL_Surface *image = NULL;
SDL_Surface *screen = NULL;
SDL_Rect location;

SDL_Rect clip[4];
int clip_index = 0;

SDL_Event event;

int count;

// Gets an image as an SDL_Surface in an optimized way
SDL_Surface *load_image(char * filename) {
	SDL_Surface * loadedImage = NULL;

	SDL_Surface * optimizedImage = NULL;

	loadedImage = IMG_Load(filename);

	if( loadedImage != NULL ) {
		optimizedImage = SDL_DisplayFormat( loadedImage );

		SDL_FreeSurface( loadedImage );
	}
	
	if( optimizedImage != NULL ) {
		Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF );
		
		SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
	}

	return optimizedImage;
}

// Applies a surface to the destination at an offset of (x,y)
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* dest, SDL_Rect * source_clip ) {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface( source, source_clip, dest, &offset );
}

// Initializes the screen and window
bool init() { 
	//Initialize all SDL subsystems 
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) {
		return false; 
	} 
	//Set up the screen 
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE ); 

	if( screen == NULL ) { 
		return false; 
	} 

	//Set the window caption 
	SDL_WM_SetCaption( "Use The Arrow Keys to Move the Ball", NULL ); 

	//If everything initialized fine 
	return true; 
}

// Sets up initial image ("test_image.png" here) to be loaded
bool load_files() { 
	//Load the image 
	image = load_image( IMAGE_FILE_NAME ); 

	if( image == NULL ) { 
		return false; 
	} 
	
	//If everything loaded fine 
	return true; 
}

void clean_up() { 
	//Free the image 
	SDL_FreeSurface( image ); 
	//Quit SDL 
	SDL_Quit(); 
}

void whiteOutScreen() {
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
}

SDL_Rect * nextClip() {
	if (count == 50) {
		clip_index = clip_index + 1;
		clip_index = clip_index % 4;
		count = 0;
	}
	count++;
	return &clip[clip_index];
}

void initClipArray() {
	//Clip range for the top left 
	clip[ 0 ].x = 0; 
	clip[ 0 ].y = 0; 
	clip[ 0 ].w = 100; 
	clip[ 0 ].h = 100; 

	//Clip range for the top right 
	clip[ 1 ].x = 100; 
	clip[ 1 ].y = 0; 
	clip[ 1 ].w = 100; 
	clip[ 1 ].h = 100; 

	//Clip range for the bottom left 
	clip[ 2 ].x = 0; 
	clip[ 2 ].y = 100; 
	clip[ 2 ].w = 100; 
	clip[ 2 ].h = 100; 

	//Clip range for the bottom right 
	clip[ 3 ].x = 100; 
	clip[ 3 ].y = 100; 
	clip[ 3 ].w = 100; 
	clip[ 3 ].h = 100;
}

int main( int argc, char* args[] ) {
	bool quit = false;

	//Initialize
	if( init() == false ) {
		 return 1; 
	}
	
	initClipArray();
	
	//Load the files 
	if( load_files() == false ) { 
		return 1; 
	}

	// Make the background white
	whiteOutScreen();
	
	location.x = 0;
	location.y = 0;
	
	apply_surface( location.x, location.y, image, screen, nextClip() );

	if( SDL_Flip( screen ) == -1 ) { 
		return 1; 
	}

	while( quit == false ) {
		while( SDL_PollEvent( &event ) ) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
		
		Uint8 *keystates = SDL_GetKeyState( NULL );
		
		if (keystates[SDLK_RIGHT] && !keystates[SDLK_LEFT]) {
			if (location.x < SCREEN_WIDTH - (image->w / 2)) {
				location.x++;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
			}
		}
		
		if (keystates[SDLK_LEFT] && !keystates[SDLK_RIGHT]) {
			if (location.x > 0) {
				location.x--;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
			}
		}
		
		if (keystates[SDLK_UP] && !keystates[SDLK_DOWN]) {
			if (location.y > 0) {
				location.y--;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
			}
		}
		
		if (keystates[SDLK_DOWN] && !keystates[SDLK_UP]) {
			if (location.y < SCREEN_HEIGHT - (image->h / 2)) {
				location.y++;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
			}
		}
		
		
		
		if( SDL_Flip( screen ) == -1 ) { 
			return 1;
		}
	}
	
	clean_up();

	return 0;
}
