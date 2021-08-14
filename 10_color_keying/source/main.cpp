#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Texture weapper class
class LTexture {
    public:
        // Inicalización de variables
        LTexture();

        // Libera la memoria
        ~LTexture();

        // Carga la imagen en el path especificado
        bool loadFromFile( std::string path );

        // Libera la textura
        void free();
        
        int x;
        int y;

        // Renderiza la textura en un punto dado
        void render( int x, int y );
    
        // Obtiene las dimensiones de la imagen
        int getWidth();
        int getHeight();

        void setWidth( int w );
        void setHeight( int h );

    private:
        // La textura actual
        SDL_Texture* mTexture;

        // Dimensiones de la imagen
        int mWidth;
        int mHeight;
};

// Inicia SDL y crea la ventana
bool init();

// carga los archivos
bool loadMedia();

// Libera la memoria y termina SDL
void close();

// Ventana donde se renderizará
SDL_Window* gWindow = NULL;

// Ventana del renderizado
SDL_Renderer* gRenderer = NULL;

// Texturas de la Escena
LTexture gFooTexture;
LTexture gBackgroundTexture;

LTexture::LTexture() {
    // Inicializa la textura
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
    x = 0;
    y = 0;
}

LTexture::~LTexture() {
    // Libera la memoria
    free();
}

bool LTexture::loadFromFile( std::string path ) {
    // Maneja la textura preexistente
    free();

    // La textura final
    SDL_Texture* newTexture = NULL;

    // Carga la imagen del path especificado
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        printf( "No se pudo cargar la imagen %s! SDL_image Error: %s\n", path.c_str(), 
                IMG_GetError() );
    } else {
        // Color key image
        // Arg[1] Fomato de pixeles 
        // Arg[2] Los componentes del color, en este caso cyan
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 
                    0xFF, 0xFF ) );

        // Crea la textura desde la superficie de pixeles
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL ) {
            printf( "No se pudo crear la textura desde %s! SDL Error: %s\n", path.c_str(), 
                    SDL_GetError() );
        } else {
            // Obtiene las dimensiones de la textura
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        // Maneja la anterior superficie cargada
        SDL_FreeSurface( loadedSurface );
    }

    // Devuelve success
    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free() {
    // Livera la textura si existe
    if( mTexture != NULL ) {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render( int x, int y ) {
    // Espacio para renderizar y el render en pantalla
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };
    SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );
}

int LTexture::getWidth() {
    return mWidth;
}

int LTexture::getHeight() {
    return mHeight;
}

void LTexture::setWidth( int w ) {
    mWidth = w;
}

void LTexture::setHeight( int h ) {
    mHeight = h;
}

bool init() {
    // Bandera
    bool success = true;

    // Inicia SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL no pudo inicializarse! SDL Error: %s\n", SDL_GetError() );
        success = false; 
    } else {
        // Filtrado
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY , "1") ) {
            printf( "Warning: Filtrado lineal de texturas no disponible!" );
        }

        // Crea la ventana
        gWindow = SDL_CreateWindow( "SDL Tutorial 10 - Color Keying", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL ) {
            printf( "No se pudo iniciar la ventana! SDL Error: %s\n", SDL_GetError() );
            success = false;
        } else {
            // Crea el renderizador
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL ) {
                printf( "No se pudo crear el renderizador! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } else {
                // Inica el renderizador de color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                // Inicializa la carga de PNG
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                    printf( "SDL_Image no pudo inicializarse! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
            }

        }
    }

    return success;
}

bool loadMedia() {
    // Bandera
    bool success = true;

    // Carga la textura foo.png
    if( !gFooTexture.loadFromFile( "romfs/foo.png" ) ) {
        printf( "Falló en cargar la textura foo!\n" );
        success = false;
    }

    // Carga la textura del fondo
    if( !gBackgroundTexture.loadFromFile( "romfs/background.png" ) ) {
        printf( "Falló en cargar la textura del fondo!\n" );
        success = false;
    }

    return success;
}

void close() {
    // Libera las texturas cargadas
    gFooTexture.free();
    gBackgroundTexture.free();

    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    // Termina SDL
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* argv[] ) {
    // Inicia SDL y crea la ventana
    if( !init() ) {
        printf( "Falló la inicialización!\n" );
        return -1;
    } else {
        if( !loadMedia() ) {
            printf( "Falló la carga de archivos!\n" );
            return -1;
        }
    }

    bool quit = false;

    SDL_Event e;

    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            switch( e.type ) {
                case SDL_QUIT:
                    printf( "Bye!\n" );
                    quit = true;
                    break;
                
                case SDL_KEYDOWN:
                      switch( e.key.keysym.sym ) {
                          case SDLK_q:
                              printf( "Bye!\n" );
                              quit = true;
                              break;

                          case SDLK_UP:
                              gFooTexture.y -= 10;
                              if ( gFooTexture.y < 0) {
                                  gFooTexture.y = 0;
                              }
                              break;
                          case SDLK_DOWN:
                              gFooTexture.y += 10;
                              if ( ( gFooTexture.y + gFooTexture.getHeight() ) > SCREEN_HEIGHT ){
                                  gFooTexture.y = SCREEN_HEIGHT - gFooTexture.getHeight();
                              }
                              break;

                          case SDLK_LEFT:
                              gFooTexture.x -= 10;
                              if ( gFooTexture.x < 0 ) {
                                  gFooTexture.x = 0;
                              }
                              break;

                          case SDLK_RIGHT:
                              gFooTexture.x += 10;
                              if( (gFooTexture.x + gFooTexture.getWidth() ) > SCREEN_WIDTH ) {
                                  gFooTexture.x = SCREEN_WIDTH - gFooTexture.getWidth();
                              }
                              break;
                      }
            }

        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Renderiza la textura de fondo
        gBackgroundTexture.setHeight( SCREEN_HEIGHT );
        gBackgroundTexture.setWidth( SCREEN_WIDTH );
        gBackgroundTexture.render( 0, 0 );

        // Renderiza la textura foo
        gFooTexture.render( gFooTexture.x, gFooTexture.y );

        // actualiza la pantalla
        SDL_RenderPresent( gRenderer );
    }
    
    close();
    return 0;
}
