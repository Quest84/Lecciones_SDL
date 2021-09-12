#include <stdio.h>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Constantes de la pantalla
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
 
        #if defined(SDL_TTF_MAJOR_VERSION)
        // Crea una imagen desde un font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
        #endif

        // Establece la modulación de color
        void setColor( Uint8 red, Uint8 green, Uint8 blue );
        
        // Set Blending 
        void setBlendMode( SDL_BlendMode blending );

        // Set alpha modulation
        void setAlpha( Uint8 alpha );

        // Libera la textura
        void free();
        
        // Renderiza la textura en un punto dado
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, 
                SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
 
        // Manupulación de Pixeles
        bool lockTexture();
        bool unlockTexture();
        void* getPixels();
        int getPitch();

        // Obtiene las dimensiones de la imagen
        int getWidth();
        int getHeight();

        void setWidth( int w );
        void setHeight( int h );

    private:
        // La textura actual
        SDL_Texture* mTexture;
        void* mPixels;
        int mPitch;

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

// Modificación de pixeles
void pixelManipulation( LTexture& texture, Uint8 oldColor[], Uint8 newColor[] );

// Ventana donde se renderizará
SDL_Window* gWindow = NULL;

// Ventana del renderizado
SDL_Renderer* gRenderer = NULL;

// Textura
LTexture gFooTexture;

LTexture::LTexture() 
{
    // Inicializa la textura
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
    mPixels = NULL;
    mPitch = 0;
}

LTexture::~LTexture() 
{
    // Libera la memoria
    free();
}

bool LTexture::loadFromFile( std::string path ) 
{
    // Maneja la textura preexistente
    free();

    // La textura final
    SDL_Texture* newTexture = NULL;

    // Carga la imagen del path especificado
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        printf( "No se pudo cargar la imagen %s! SDL_image Error: %s\n", path.c_str(), 
                IMG_GetError() );
    } 
    else 
    {
        // Convierte la superficie al display format
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, 
                SDL_GetWindowPixelFormat( gWindow ), 0 );
        if( formattedSurface == NULL )
        {
            printf( "No se pudo convertir surface al display format! SDL_Error: %s\n",
                    SDL_GetError() );
        }
        else
        {  
            // Crea la textura desde la superficie de pixeles
            newTexture = SDL_CreateTexture( gRenderer, SDL_GetWindowPixelFormat( gWindow ), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
            if( newTexture == NULL ) 
            {
                printf( "No se pudo crear la textura desde %s! SDL Error: %s\n", path.c_str(), 
                        SDL_GetError() );
            } 
            else 
            {
                // Bloquea la textura para la manipulación
                SDL_LockTexture( newTexture, NULL, &mPixels, &mPitch );

                // Copia cargando/formateando la superficie de pixeles
                memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch
                        * formattedSurface->h );
                
                // Debloquea la textura
                SDL_UnlockTexture( newTexture );
                mPixels = NULL;

                // Obtiene las dimensiones de la textura
                mWidth = loadedSurface->w;
                mHeight = loadedSurface->h;
            }

            // Maneja la anterior superficie formateda
            SDL_FreeSurface( formattedSurface );
        }

        // Maneja la anterior superficie cargada
        SDL_FreeSurface( loadedSurface );
    }

    // Devuelve success
    mTexture = newTexture;
    return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
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
#endif

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

bool LTexture::lockTexture()
{
    bool success = true;

    // La textura ya está bloqueada
    if( mPixels != NULL )
    {
        printf( "La textura ya está bloqueada!\n" );
        success = false;
    }

    // Bloquea la textura
    else
    {
        if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
        {
            printf( "No se pudo desbloquear la textura! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
    }

    return success;
}

bool LTexture::unlockTexture()
{ 
    bool success = true;

    // La textura no está bloqueada
    if( mPixels == NULL )
    {
        printf( "La textura no está bloqueada!\n" );
        success = false;
    }

    // Desbloquea la textura
    else
    {
        SDL_UnlockTexture( mTexture );
        mPixels = NULL;
        mPitch = 0;
    }

    return success;
}

void* LTexture::getPixels()
{
    return mPixels;
}

int LTexture::getPitch()
{
    return mPitch;
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
        gWindow = SDL_CreateWindow( "SDL Tutorial 30 - Scrolling", 
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

            }

        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    // Carga la textura
    if( !gFooTexture.loadFromFile( "romfs/foo.png" ) )
    {
        printf( "Falló al cargar de la textura!\n" );
        return false;
    }
    else
    {
        // Cyan
        Uint8 colorKey[3] = { 0x00, 0xFF, 0xFF };
        Uint8 transparent[4] = { 0xFF, 0xFF, 0xFF, 0x00 };

        pixelManipulation( gFooTexture, colorKey, transparent );
        
        /*
        // Bloquea la texture
        if( !gFooTexture.lockTexture() )
        {
            printf( "No se pudo bloquear la textura Foo!\n" );
            success = false;
        }
        // Manual color key
        else
        {
            // Guarda el formato desde la ventana
            Uint32 format = SDL_GetWindowPixelFormat( gWindow );
            SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );

            // Get pixel data
            Uint32* pixels = ( Uint32* )gFooTexture.getPixels();
            int pixelCount = ( gFooTexture.getPitch() / 4 ) * gFooTexture.getHeight();

            // Mapea los colores
            Uint32 colorKey = SDL_MapRGB( mappingFormat, 0x00, 0xFF, 0xFF );
            Uint32 transparent = SDL_MapRGBA( mappingFormat, 0xFF, 0xFF, 0xFF, 0x00 );

            // For key pixels
            for( int i = 0; i < pixelCount; ++i )
            {
                if( pixels[ i ] == colorKey )
                {
                    pixels[ i ] = transparent;
                }
            }

            // Desbloquea la textura
            gFooTexture.unlockTexture();

            // Free format
            SDL_FreeFormat( mappingFormat );
        }*/
    }

    return success;
}

void pixelManipulation( LTexture& texture, Uint8 oldColor[], Uint8 newColor[] )
{
    if( !texture.lockTexture() )
    {
        printf( "No se pudo bloquear la textura!" );
    }
    else
    {
        Uint32 format = SDL_GetWindowPixelFormat( gWindow );
 
        SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );

        Uint32* pixels = ( Uint32* )texture.getPixels();
        int pixelCount = ( texture.getPitch() / 4 ) * texture.getHeight();

        Uint32 colorKey = SDL_MapRGB( mappingFormat, oldColor[0], oldColor[1], oldColor[2] );
        Uint32 nextColor = SDL_MapRGBA( mappingFormat, newColor[0], newColor[1], 
                newColor[2], newColor[3] );

        for( int i = 0; i < pixelCount; ++i )
        {
            if( pixels[ i ] == colorKey )
            {
                pixels[ i ] = nextColor;
            }
        }
        // Desbloquea la textura
        texture.unlockTexture();

        // Libera el formato
        SDL_FreeFormat( mappingFormat );  
    }
}


void close() 
{
    // Libera la textura cargada
    gFooTexture.free();

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
    } 
    else 
    {    
        // Los tiles del nivel
        if( !loadMedia() ) {
            printf( "Falló la carga de archivos!\n" );
            return -1;
        }
        else
        {        
            bool quit = false;

            SDL_Event e;    

            Uint8 oldColor[3] = { 0x00, 0x00, 0x00 };
            Uint8 newColor[4] = { 0x0F, 0xCC, 0xAA, 0x00 };

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
                              }
                    }
                }
                
                // Limpia la pantalla
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );


                pixelManipulation( gFooTexture, oldColor, newColor );
                
                // Renderiza la textura
                gFooTexture.render( ( SCREEN_WIDTH -gFooTexture.getWidth() ) / 2,
                        ( SCREEN_HEIGHT - gFooTexture.getHeight() ) / 2 );
 
                // Actualiza la pantalla
                SDL_RenderPresent( gRenderer );
            }
        }   

        // Libera los recursos y cierra
        close();
    }

    return 0;
}
