#include<stdio.h>
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

const int SCREEN_WIDTH = 750;
const int SCREEN_HEIGHT = 750;

bool init();

bool loadMedia();

void close();

SDL_Texture* loadTexture( std::string path );

// La ventana donde se renderizara 
SDL_Window* gWindow = NULL;

// El renderizador de la ventana
SDL_Renderer* gRenderer = NULL;

// El display actual de la textura
SDL_Texture* gTexture = NULL;


bool init() {
    // Inicializar la bandera
    bool success = true;
    
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL no pudo inicializarse! SDL Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        // Establece el filtrado de textura lineal
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
            printf( "Warning: El filtrado de textura linea no está activado!" );
        }    
        // Crea la ventana
        gWindow = SDL_CreateWindow( "SDL Tutorial - 09 Viewport", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if ( gWindow == NULL ) {
            printf( "No se pudo crear la ventana! SDL Error: %s\n", SDL_GetError() );
            success = false;
        } else {
            // Crea el renderizador para la ventana
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL ) {
                printf( "No se pudo crear el renderizador! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } else {
                // Inicializa el color renderer
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                
                // Inicializa la carga del PNG
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                    printf( "SDL_image no pudo inicializarse! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    // Inicia la bandera
    bool success = true;

    // Carga la textura
    gTexture = loadTexture( "viewport.png" );
    if( gTexture == NULL ) {
        printf( "Falló la carga de la imagen!\n" );
        success = false;
    }

    return success;
}

void close() {
    // Libera la imagen cargada
    SDL_DestroyTexture( gTexture );
    gTexture = NULL;

    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    // Termina los subsistemas SDL
    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture( std::string path ) {
    // La textura final
    SDL_Texture* newTexture = NULL;

    // Carga la imagen en el path especificado
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        printf( "No se pudo cargar la imagen %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    } else {
        // crea la textura en la superficie de pixeles
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL ) {
            printf( "No se pudo crear la textura desde %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        // Maneja la anterior superficie cargada
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
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
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderClear( gRenderer );
        
        // Viewport de arriba a la izquierda
        SDL_Rect topLeftViewport;
        topLeftViewport.x = 0;
        topLeftViewport.y = 0;
        topLeftViewport.w = SCREEN_WIDTH / 2;
        topLeftViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport( gRenderer, &topLeftViewport );
    
        // Renderiza la textura en pantalla
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );

        // Viewport del arriba a la izquierda
        SDL_Rect topRightViewport;
        topRightViewport.x = SCREEN_WIDTH / 2;
        topRightViewport.y = 0;
        topRightViewport.w = SCREEN_WIDTH / 2;
        topRightViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport( gRenderer, &topRightViewport );

        // Renderiza la textura en pantalla
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );

        // Viewport del fondo
        SDL_Rect bottomViewport;
        bottomViewport.x = 0;
        bottomViewport.y = SCREEN_HEIGHT / 2;
        bottomViewport.w = SCREEN_WIDTH;
        bottomViewport.h = SCREEN_HEIGHT / 2;
        SDL_RenderSetViewport( gRenderer, &bottomViewport );

        // Renderiza la textura en pantalla
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );
        
        // Actualiza la pantalla
        // Al no usar SDL_Surfaces no se puede usar SDL_UpdateWindowSurface
        // En su lugar se usa:
        SDL_RenderPresent( gRenderer );
    }
    
    close();

    return 0;
}
