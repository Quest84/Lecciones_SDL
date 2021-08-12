#include<stdio.h>
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

const int SCREEN_WIDTH = 750;
const int SCREEN_HEIGHT = 750;

// Carga una imagen individual como textura
SDL_Texture* loadTexture( std::string path );

// La ventana donde se renderizara 
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

// El display de la textura
SDL_Texture* gTexture = NULL;

/* Las texturas tiene su propio tipo de dato (SDL_Texture). Cuando se trabaja con texturas
 * se necsita un SDL_Renderer para renderizarlas en pantalla, el cual se declara con el
 * nombre de gRenderer */

bool init();

bool loadMedia();

void close();

bool init() {
    // Inicializar la bandera
    bool success = true;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL no pudo inicializarse! SDL Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        // Crea la ventana
        gWindow = SDL_CreateWindow( "SDL Tutorial 07 - Carga y render de textura", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
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
                // Inicializa la carga de PNG
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                    printf( "SDL Image no se pudo inicializar! SDL_image Error: %s\n", IMG_GetError() );
                }      
            }
        }
    }

    return success;
}

SDL_Texture* loadTexture( std::string path ) {
    // La texturea final 
    SDL_Texture* newTexture = NULL;

    // Carga la textura en el path especficado
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        printf( "No se pudo cargar la imagen %s\n SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    } else {
        // Crea la textura desde una superficie de pixeles
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL ) {
            printf( "No se pudo crear la textura desde %s! SDl Error: %s\n", path.c_str(), SDL_GetError() );          
        } 
        
        // Manejar la superficie antes cargada
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

bool loadMedia() {
    // Inicia la bandera
    bool success = true;

    // Carga la texture PNG
    gTexture = loadTexture( "romfs/image.png" );
    if( gTexture == NULL ) {
        printf( "Falló al cargar la imagen de la textura\n" );
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
        SDL_RenderClear( gRenderer );
        

        // Renderiza la textura en pantalla
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL );

        // Actualiza la pantalla
        // Al no usar SDL_Surfaces no se puede usar SDL_UpdateWindowSurface
        // En su lugar se usa:
        SDL_RenderPresent( gRenderer );
    }

    return 0;
}
