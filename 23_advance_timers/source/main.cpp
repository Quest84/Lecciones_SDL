#include <stdio.h>
#include <string>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int FONT_SIZE = 28;
// Texture weapper class
class LTexture {
    public:
        // Inicalización de variables
        LTexture();

        // Libera la memoria
        ~LTexture();

        // Carga la imagen en el path especificado
        bool loadFromFile( std::string path );
 
        // Crea una imagen desde un font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor );

        // Establece la modulación de color
        void setColor( Uint8 red, Uint8 green, Uint8 blue );
        
        // Set Blending 
        void setBlendMode( SDL_BlendMode blending );

        // Set alpha modulation
        void setAlpha( Uint8 alpha );

        // Libera la textura
        void free();
        
        int x;
        int y;
    
        // Renderiza la textura en un punto dado
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, 
                SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
    
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

class LTimer
{
    public:
        LTimer();

        // Acciones del contador
        void start();
        void stop(); 
        void pause();
        void unpause();

        // Obtiene el tiempo
        Uint32 getTicks();

        // Revisa el estado del contador
        bool isStarted();
        bool isPaused();
    private:
        // La hora del reloj cuando el contador inicia
        Uint32 mStartTicks;

        // The ticks stored when the timer was paused
        Uint32 mPausedTicks;

        // El estado del contador
        bool mPaused;
        bool mStarted;
};

LTimer::LTimer()
{
    // Inicializa las variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    // Inicio del contador
    mStarted = true;

    // Despausa el contador
    mPaused = false;

    // Obtiene el tiempo actual del reloj
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::pause()
{
    // Si el contador está corriendo y no está pausado
    if( mStarted && !mPaused )
    {
        // Pausa el contador
        mPaused = true;

        // Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    // Si el contador está corriendo y pausado
    if( mStarted && mPaused )
    {
        // Despausa el contador
        mPaused = false;

        // Reinicia la cuenta de los ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        // Reinicia los ticks pausados
        mPausedTicks = 0;
    }
}

void LTimer::stop()
{
    // Detiene el contador
    mStarted = false;

    // Despausa el contador
    mPaused = false;

    // Limpia las variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

Uint32 LTimer::getTicks()
{
    // El tiempo actual del contador
    Uint32 time = 0;

    // Si el contador está corriendo
    if( mStarted )
    {
        // Si el contador está pausado
        if( mPaused )
        {
            // Devuelve el numero de ticks cuando el contador está pausado
            time = mPausedTicks;
        }
        else 
        {
            // Devuelve el tiempo actual menos el tiempo de inicial
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
    // El contador está corriendo y pausado o despausado
    return mStarted;
}

bool LTimer::isPaused()
{
    // El contador está corriendo y pausado
    return mPaused && mStarted;
}

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

// Fuente usada globalmente
TTF_Font *gFont = NULL;

// Textura
LTexture gStartPromptTexture;
LTexture gPausePromptTexture;
LTexture gTimeTextTexture;
LTexture gTicksTextTexture;

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

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor ) {
    // Manejar la textura existente
    free();

    // Renderiza desde la superficie del texto
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL ) {
        printf( "No se pudo renderizar la superficie del texto! SDL_ttf Error: %s\n", 
                TTF_GetError() );
    } else {
        // Crea una textura desde la superficie de pixeles
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL ) {
            printf( "No se pudo crear la textura desde el texto renderizado! SDL Error: %s\n",
                    SDL_GetError() );
        } else {
            // Obtiene las dimensiones de la imagen
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        // Manejar la anterior superficie
        SDL_FreeSurface( textSurface );
    }

    // Devuelve la textura
    return mTexture != NULL;
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue ) {
    // Textura modulada
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending ) {
    // Set Blending function
    SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha ) {
    // Modula la textura alpha
    SDL_SetTextureAlphaMod( mTexture, alpha );
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

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip ) {
    // Espacio para renderizar y el render en pantalla
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    // Establece las dimensiones del clip rendering
    if( clip != NULL ) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    // Render to screen, similar a RenderCopy pero con argumentos extra para girar y rotar
    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );

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
        gWindow = SDL_CreateWindow( "SDL Tutorial 23 - Advance Timers", 
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL ) {
            printf( "No se pudo iniciar la ventana! SDL Error: %s\n", SDL_GetError() );
            success = false;
        } else {
            // Crea el renderizador
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED 
                    | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL ) {
                printf( "No se pudo crear el renderizador! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } else {
                // Inica el renderizador de color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                // Inicializa la carga de PNG
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                    printf( "SDL_Image no pudo inicializarse! SDL_image Error: %s\n", 
                            IMG_GetError() );
                    success = false;
                }

                // Inicializa SDL_ttf
                if( TTF_Init() == -1 ) {
                    printf( "SDL_ttf no pudo inicializarse! SDL_ttf Error: %s\n", 
                            TTF_GetError() );
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
    
    // Abre la fuente
    gFont = TTF_OpenFont( "romfs/lazy.ttf", FONT_SIZE );
    if( gFont == NULL ) {
        printf( "Falló la carga de la fuente! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    } else {
        // Renderiza el texto
        SDL_Color textColor = { 0x00, 0x00, 0x00, 0xFF };
        if( !gStartPromptTexture.loadFromRenderedText( "Press S to Start or Stop the Timer", 
                    textColor ) ) {
            printf( "Falló la carga de la textura del texto!\n" );
            success = false;
        }
        gPausePromptTexture.loadFromRenderedText( "Press P to Pause or Unpause the Timer", textColor );
        // gTicksTextTexture.loadFromRenderedText( "", textColor );
    }

    return success;
}

void close() {
    // Libera la textura cargada
    gStartPromptTexture.free();
    gPausePromptTexture.free();
    gTimeTextTexture.free();

    // Libera la fuente global
    TTF_CloseFont( gFont );
    gFont = NULL;

    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    // Termina SDL
    TTF_Quit();
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

    // Establece el color del texto en negro
    SDL_Color textColor = { 0, 0, 0, 255 };

    // La aplicación del contador
    LTimer timer;

    // Stream de texto en memoria
    std::stringstream timeText;
    std::stringstream ticksText;

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

                          case SDLK_s:
                              if( timer.isStarted() )
                              {
                                  timer.stop();
                              }
                              else {
                                  timer.start();
                              }

                          case SDLK_p:
                              if( timer.isPaused() )
                              {
                                  timer.unpause();
                              }
                              else 
                              {
                                  timer.pause();
                              }
                      }           
            }

        }

        // Establee el tiempo a renderizar
        timeText.str( "" );
        timeText << "Millisegundos desde que inicio:";
        
        ticksText.str( "" );
        ticksText << (timer.getTicks() / 1000.f );

        // Renderiza el texto
        if( !gTimeTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor ) ) {
            printf( "No se pudo renderizar la textura del tiempo!\n" );
        }

        gTicksTextTexture.loadFromRenderedText( ticksText.str().c_str(), textColor );
        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );
        
        // Renderiza la textura del texto
        gStartPromptTexture.render( ( SCREEN_WIDTH - gStartPromptTexture.getWidth() ) / 2, 0 );
        gPausePromptTexture.render( ( SCREEN_WIDTH - gPausePromptTexture.getWidth() ) / 2,
                ( gStartPromptTexture.getHeight() ) );
        gTimeTextTexture.render( ( SCREEN_WIDTH - gTimeTextTexture.getWidth() ) / 2,
                ( SCREEN_HEIGHT - gTimeTextTexture.getHeight() ) / 2 );
        gTicksTextTexture.render( ( SCREEN_WIDTH - gTicksTextTexture.getWidth() ) / 2,
                ( SCREEN_HEIGHT - gTicksTextTexture.getHeight() ) / 2 + FONT_SIZE );

        // Actualiza la pantalla
        SDL_RenderPresent( gRenderer );
    }
    
    close();
    return 0;
}
