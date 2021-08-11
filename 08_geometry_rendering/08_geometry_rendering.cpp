#include<stdio.h>
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

const int SCREEN_WIDTH = 750;
const int SCREEN_HEIGHT = 750;

// La ventana donde se renderizara 
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

bool init();

bool loadMedia();

void close();

bool init() {
    // Inicializar la bandera
    bool success = true;

    // Crea la ventana
    gWindow = SDL_CreateWindow( "SDL Tutorial - 08 Geometry Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if ( gWindow == NULL ) {
        printf( "No se pudo crear la ventana! SDL Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        // Crea el render para la ventana
        gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
        if( gRenderer == NULL ) {
            printf( "No se pudo crear el renderer! SDL Error: %s\n", SDL_GetError() );
            success = false;
        } else {
            // Inicializa el renderizador de color
            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        }
    }

    return success;
}

bool loadMedia() {
    // Inicia la bandera
    bool success = true;

    // Nada para cargar
    return success;
}

void close() {
    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    // Termina los subsistemas SDL
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if( !init() ) {
        printf( "Falló la inicialización!\n" );
        return -1;
    } else {
        // Carga los archivos
        if( !loadMedia() ) {
            printf( "Falló la carga de archivos!" );
            return -1;
        }
    }

    // Flag loop
    bool quit = false;
    
    // Manejador de Eventos
    SDL_Event e;

    while( !quit ) {
        // Maneja los eventos en cola
        while( SDL_PollEvent( &e ) != 0 ) {
            // Eventos del usuario
            switch( e.type ) {
                case SDL_QUIT:
                    printf( "Bye!\n" );
                    quit = true;
                    break;
                
                // Teclas
                case SDL_KEYDOWN:  
                    switch( e.key.keysym.sym ) {
                        case SDLK_q:
                            printf( "Bye!\n" );
                            quit = true;
                            break;

                        default:
                            break;
                    }             
    
                default:
                    break;    
            }
        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Renderiza una rectangulo rojo
        SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
        SDL_RenderFillRect( gRenderer, &fillRect );

        // Renderiza una cuadrado verde
        SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
        SDL_RenderDrawRect( gRenderer, &outlineRect );

        // Dibuja la línea horizontal
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
        SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

        // Dibuja una línea vertical de puntos amarillos
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF );
        for( int i = 0; i < SCREEN_HEIGHT; i += 4 ) {
            SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
        }
            
        // Actualiza la pantalla
        // Al no usar SDL_Surfaces no se puede usar SDL_UpdateWindowSurface
        // En su lugar se usa:
        SDL_RenderPresent( gRenderer );
    }
    
    close();

    return 0;
}
