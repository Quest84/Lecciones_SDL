// Programa para mostrar una imagen en pantalla

#include<stdio.h>
#include<SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Inicia SDL y crea una ventana
bool init();

// Load media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// La ventana donde se renderizará
SDL_Window* gWindow = NULL;

// La superficie que contiene la ventana
SDL_Surface* gScreenSurface = NULL;

// La imagen que se cargará y mostrará en la ventana
SDL_Surface* gHelloWorld = NULL;
/* SDL Surace es un tipo de dato, solo es una imagen que contiene los pixeles de la imagen
 * junto con los datos a renderizar. Se rederizan por software. */

bool init() {
	// Initializa la bandera
	bool success = true;

	// Inicializa SDL
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf("SDL no pudo inicializarse! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	} else {
		// Crea la ventana
		gWindow = SDL_CreateWindow( "SDL Tutorial 2 - Imagen en pantalla", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if ( gWindow == NULL ) {
			printf( "No se pudo crear la ventana! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		} else {
			// Obtiene la superficie de la ventana
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}
	
	return success;
}

bool loadMedia() {
	// Carga exitosamente la bandera
	bool success = true;

	// Carga una imagen splash
	gHelloWorld = SDL_LoadBMP( "floppa.bmp" );
	if( gHelloWorld == NULL ) {
		printf( "No se pudo cargar la imagen %s! SDL Error: %s\n", "02_image_on_screen/floppa.bmp", SDL_GetError() );
		success = false;
	}

	return success;
}

void close() {
	// Deallocate surface
	SDL_FreeSurface( gHelloWorld );
	gHelloWorld = NULL;

	// Destruye la ventana (y la supericie)
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	// Termina los subsistemas SDL
	SDL_Quit();
}

int main( int argc, char* args[] ) {
	// Inicia SDL y crea la ventana
	if( !init() ) {
		printf( "Fallo la inicialización!\n" );
	} else {
		// Load media
		if ( !loadMedia() ) {
			printf( "Falló la carga de la imagen!\n" );
		} else {
			// Aplica la imagen
			SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );

			// Actualiza la superficie
			SDL_UpdateWindowSurface( gWindow );

			// Espera 2 segundos
			SDL_Delay( 2000 );
		}
	}

	// Libera los recursos y cierra SDL
	close();

	return 0;
}
