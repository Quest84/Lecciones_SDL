/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>

//Constantes para las dimensiones de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Inicia SDL y crea la ventana
bool init();

//Carga los archivos
bool loadMedia();

//Libera los archivos y cerra SDL
void close();

//La ventana donde se renderizará
SDL_Window* gWindow = NULL;
	
//La superficie que contiene la ventana
SDL_Surface* gScreenSurface = NULL;

//La imagen cargada y mostrada en pantalla
SDL_Surface* gXOut = NULL;

bool init() {
	//Initializa la bandera
	bool success = true;

	//Initializa SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL no pudo inicialiarse! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	} else {
		//Crea la ventana
		gWindow = SDL_CreateWindow( "SDL Tutorial 3 - Programación orientada a evento", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL ) {
			printf( "La ventana no pudo crearse! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		} else {
			//Obtiene la superficie de la ventana
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

bool loadMedia() {
	//Carga la bandera
	bool success = true;

	//Carga la imagen splash
	gXOut = SDL_LoadBMP( "floppa.bmp" );
	if( gXOut == NULL ) {
		printf( "No se pudo cargar la imagen %s! SDL Error: %s\n", "floppa.bmp", SDL_GetError() );
		success = false;
	}

	return success;
}

void close() {
	//Deallocate surface
	SDL_FreeSurface( gXOut );
	gXOut = NULL;

	//Destruye la ventana
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Termina los subsistemas SDL
	SDL_Quit();
}

int main( int argc, char* args[] ) {
	//Inicia SDL y crea la ventana
	if( !init() ) {
		printf( "Falló la inicialización!\n" );
	} else {
		//Carga los archivos
		if( !loadMedia() ) {
			printf( "Falló la carga de archivos!\n" );
		} else {			
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Mientras la aplicación está funcionando 
			while( !quit ) {
				//Maneja los eventos de la cola
				while( SDL_PollEvent( &e ) != 0 ) {
					//User requests quit
					/* if( e.type == SDL_QUIT ) {
						quit = true;
					} */
					switch(e.type){
						case SDL_QUIT:
							quit = true;
							printf( "Bye!" );
							break;

						case SDL_MOUSEMOTION:
							printf( "Obtiene el evento de movimiento del mouse.\n" );
							printf( "La posición actual del mouse es: (%d, %d)\n", e.motion.x, e.motion.y );
							break;

						default:
							printf( "Evento no manejado!\n" );
							break;
					}
				}

				//Aplica la imagen
				SDL_BlitSurface( gXOut, NULL, gScreenSurface, NULL );
			
				//Actualiza la superficie
				SDL_UpdateWindowSurface( gWindow );
			}
		}
	}

	//Libera los recursos y termina SDL
	close();

	return 0;
}
