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
        
        // Establece la modulación de color
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        // Libera la textura
        void free();
        
        int x;
        int y;
    
        // Renderiza la textura en un punto dado
        void render( int x, int y, SDL_Rect* clip = NULL );
    
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
LTexture gModulatedTexture;

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

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue ) {
    // Textura modulada
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::free() {
    // Libera la textura si existe
    if( mTexture != NULL ) {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render( int x, int y, SDL_Rect* clip ) {
    // Espacio para renderizar y el render en pantalla
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    // Establece las dimensiones del clip rendering
    if( clip != NULL ) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    // Render to screen
    SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );

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
        gWindow = SDL_CreateWindow( "SDL Tutorial 09 - Color Keying", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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
    
    // Carga la textura de la hoja de sprites
    if( !gModulatedTexture.loadFromFile( "romfs/colors.png" ) ) {
        printf( "Falló la carga de la textura!\n" );
        success = false;
    }
    return success;
}

void close() {
    // Libera la textura cargada
    gModulatedTexture.free();

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

    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;

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

                          case SDLK_a:
                              r += 32;
                              break;
                          case SDLK_s:
                              g += 32;
                              break;
                          case SDLK_d:
                              b += 32;
                              break;

                          case SDLK_z:
                              r -= 32;
                              break;
                          case SDLK_x:
                              g -= 32;
                              break;
                          case SDLK_c:
                              b -= 32;
                              break;
                      }
            }

        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );
        
        // Modulación y render de la texura
        gModulatedTexture.setColor( r, g, b );
        gModulatedTexture.render( 0, 0 );

        // Render bottom left sprite
        SDL_RenderPresent( gRenderer );
    }
    
    close();
    return 0;
}
