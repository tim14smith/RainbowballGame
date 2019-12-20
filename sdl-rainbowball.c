// sdl-rainbowball.c - basic demo of an SDL window with a moveable ball
// Author: Tim Smith (tim14smith@gmail.com)

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <stdbool.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

const int PIXELS_PER_MOVE = 10;
const int MS_PER_FRAME = 30;

const int MESSAGE_X_Y = 80;
char * IMAGE_FILE_NAME = "dots.png";
char * FONT_FILE_NAME = "retro-edge.ttf";
char * MUSIC_FILE_NAME = "rock_beat.wav";

SDL_Surface *message = NULL;
TTF_Font *font = NULL;
SDL_Color textColor = { 0, 0, 0 };

SDL_Surface *image = NULL;
SDL_Surface *screen = NULL;
SDL_Rect location;

SDL_Rect clip[4];
int clip_index = 0;

SDL_Event event;

int count;
int frame_start;

// Whether or not we're trying to play music this time
bool got_music;

// Music to play
Mix_Music *music = NULL;

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
		printf("Init everything failed.\n");
		return false; 
	} 
	//Set up the screen 
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE ); 

	if( screen == NULL ) { 
		printf("Failed to load screen.\n");
		return false; 
	}
	
	//Initialize SDL_ttf 
	if( TTF_Init() == -1 ) { 
		printf("Init SDL TTF failed.\n");
		return false;
	}
	
	//Initialize SDL_mixer 
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
		printf("Init audio failed.\n");
		return false;
	}

	//Set the window caption 
	SDL_WM_SetCaption( "Rainbowball Game", NULL ); 

	//If everything initialized fine 
	return true; 
}

// Open and set up initial files
bool load_files() { 
	//Load the image 
	image = load_image( IMAGE_FILE_NAME ); 

	if( image == NULL ) { 
		printf("Image not found at %s.\n", IMAGE_FILE_NAME);
		return false; 
	} 
	
	font = TTF_OpenFont(FONT_FILE_NAME, 28);
	
	if( font == NULL ) {
		printf("Font not found at %s.\n", FONT_FILE_NAME);
		return false;
	}
	
	music = Mix_LoadMUS( MUSIC_FILE_NAME );
	
	 if( music == NULL ) { 
		 printf("Music not found at %s.\n", MUSIC_FILE_NAME);
		 return false; 
	}
	
	//If everything loaded fine 
	return true; 
}

void clean_up() { 
	//Free all the surfaces 
	SDL_FreeSurface( image ); 
	
	SDL_FreeSurface( message );
	
	SDL_FreeSurface( screen );
	
	Mix_FreeMusic( music ); 
	
	//Close the font 
	TTF_CloseFont( font ); 
	
	//Quit SDL_mixer 
	Mix_CloseAudio(); 
	
	//Quit SDL_ttf 
	TTF_Quit();
	
	//Quit SDL 
	SDL_Quit(); 
}

void whiteOutScreen() {
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
}

SDL_Rect * nextClip() {
	if (count == PIXELS_PER_MOVE/2) {
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

bool try_playing_music() {
	// There is no music playing
	if( Mix_PlayingMusic() == 0 ) { 
		//Play the music 
		if( Mix_PlayMusic( music, -1 ) == -1 ) {
			return false; 	
		}
	} else {
		if (Mix_PausedMusic() == 1) {
			Mix_ResumeMusic();
		}
	}
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
	
	message = TTF_RenderText_Solid( font, "Arrow keys to move, R to restart", textColor );
	
	apply_surface(MESSAGE_X_Y, MESSAGE_X_Y, message, screen, NULL);

	location.x = 0;
	location.y = 0;
	
	apply_surface( location.x, location.y, image, screen, nextClip() );

	if( SDL_Flip( screen ) == -1 ) { 
		return 1; 
	}

	while( quit == false ) {
		frame_start = SDL_GetTicks();
		while( SDL_PollEvent( &event ) ) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			if (event.type == SDL_VIDEORESIZE) {
				SCREEN_WIDTH = event.resize.w;
				SCREEN_HEIGHT = event.resize.h;
				screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE );
				whiteOutScreen();
				// Show the arrow key message again
				apply_surface(MESSAGE_X_Y, MESSAGE_X_Y, message, screen, NULL);
				// Show the ball
				location.x = 0;
				location.y = 0;
				apply_surface( location.x, location.y, image, screen, nextClip() );
			}
		}
		
		Uint8 *keystates = SDL_GetKeyState( NULL );
		
		if (keystates[SDLK_r]) {
			clean_up();
			main(0, NULL);
		}
		
		got_music = false;
		
		if (keystates[SDLK_RIGHT] && !keystates[SDLK_LEFT]) {
			if (location.x < SCREEN_WIDTH - (image->w / 2)) {
				location.x+=PIXELS_PER_MOVE;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
				try_playing_music();
				got_music = true;
			}
		}
		
		if (keystates[SDLK_LEFT] && !keystates[SDLK_RIGHT]) {
			if (location.x > 0) {
				location.x-=PIXELS_PER_MOVE;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
				try_playing_music();
				got_music = true;
			}
		}
		
		if (keystates[SDLK_UP] && !keystates[SDLK_DOWN]) {
			if (location.y > 0) {
				location.y-=PIXELS_PER_MOVE;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
				try_playing_music();
				got_music = true;
			}
		}
		
		if (keystates[SDLK_DOWN] && !keystates[SDLK_UP]) {
			if (location.y < SCREEN_HEIGHT - (image->h / 2)) {
				location.y+=PIXELS_PER_MOVE;
				whiteOutScreen();
				apply_surface(location.x, location.y, image, screen, nextClip());
				try_playing_music();
				got_music = true;
			}
		}
		
		if (!got_music) {
			// If we shouldn't be playing music, pause any currently playing
			if (Mix_PlayingMusic() != 0) {
				Mix_PauseMusic();
			}
		}
		
		if( SDL_Flip( screen ) == -1 ) { 
			return 1;
		}
		
		int time_elapsed = SDL_GetTicks() - frame_start;
		if (time_elapsed < MS_PER_FRAME) {
			SDL_Delay(MS_PER_FRAME-time_elapsed);
		}
	}
	
	clean_up();

	return 0;
}
