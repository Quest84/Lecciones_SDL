#include<stdio.h>
#include<string>
#include<SDL2/SDL.h>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;

//Constantes de las teclas presionadas
enum KeyPressSurfaces {
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_TOTAL
};

// Inicia SDL y crea la ventana
bool init();

// Carga los archivos
bool loadMedia();

// Libera los archivos y deinicializa los punteros, termina SDL
void close();

// Carga una imagen individual
SDL_Surface* loadSurface( std::string path );

// La ventana donde se renderizará la imagen
SDL_Window* gWindow = NULL;

// La superficie contenida por la ventana
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gStretchedSurface = NULL;
// las imagenes que corresponden a la tecla presionada
SDL_Surface* gKeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ];

// Imagen actualmente mostrada
SDL_Surface* gCurrentSurface = NULL;

SDL_Surface* loadSurface( std::string path ) {
	// La imagen final optimizada
    SDL_Surface* optimizedSurface = NULL;
    
    // Carga la imagen de la ruta especificado
	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL ) {
		printf( "No se pudo cargar la imagen %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
    } else {
        // Convierte la superficie al formato de la pantalla
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL ) {
            printf( "No se pudo optimizar la imagen %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        // Manejar la superficie anteriormente cargada
        SDL_FreeSurface( loadedSurface );
    }

	return optimizedSurface;
}

bool init() {
	// Inicializa la bandera
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL no pudo iniciarse! SDL Error: %s\n", SDL_GetError() );
		success = false;
	} else {
		// Crea la ventana
		gWindow = SDL_CreateWindow("Tutorial SDL - 05 Optimización y escalado de la imagen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
				SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if ( gWindow == NULL ) {
			printf( "No pudo crearse la ventana! SDL Error: %s\n", SDL_GetError() );
			success = false;
		} else {
			// Obtener la superficie de la ventana
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

bool loadMedia() {
	// Cargando la bandera
	bool success = true;

	// Carga la superficie por defecto
	gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface( "stretch.bmp" );
	if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == NULL ) {
		printf( "Falló en cargar la imagen por defecto!\n" );
		success = false;
	}


	return success;
}

void close() {
	// Deallocate surfaces
	
	for( int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++ ) {
		SDL_FreeSurface( gKeyPressSurfaces[ i ] );
		gKeyPressSurfaces[ i ] = NULL;
	}

	// Destruye la ventana
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	// Termina los subsistemas SDL
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	if ( !init() ){
		printf( "Falló la inicialización!\n" );
		return -1;
	} else {
		// Carga los archivos
		if( !loadMedia() ) {
			printf( "Falló la carga de archivos!\n" );
			return -1;
		}
	}

	// Flag loop
	bool quit = false;

	// Manejador de eventos
	SDL_Event e;

	// Ppone la superficie por defecto
	gStretchedSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];

	// Mientras la aplicacion esté funcionando
	while( !quit ) {
		// Maneja los eventos en la cola
		while( SDL_PollEvent( &e ) != 0 ) {
			// Eventos del usuario
			switch (e.type) {
				case SDL_QUIT:
					printf( "Bye!\n" );
					quit = true;
					break;
				
				// Teclas presionadas por el usuario
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_q:
							printf( "Bye!\n" );
						        quit = true;
							break;	

						default:
							gStretchedSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
							break;
					}

				default:
					// printf( "¡Evento no manejado!\n" );
					break;
			}
		}
	    
        // Aplica la imagen reducida
        SDL_Rect stretchRect;
        stretchRect.x = 0;
        stretchRect.y = 0;
        stretchRect.w = SCREEN_WIDTH;
        stretchRect.h = SCREEN_HEIGHT;
        SDL_BlitScaled( gStretchedSurface, NULL, gScreenSurface, &stretchRect );

		// Actualiza la superficie
		SDL_UpdateWindowSurface( gWindow );
	}

	close();

}
