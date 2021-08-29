#include <stdio.h>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// Constantes de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Constantes del nivel
const int LEVEL_WIDTH = 1920;
const int LEVEL_HEIGHT = 2382;

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

class Dot
{
    public:
        // Dimensiones del punto
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        // Maximá eje de velocidad del punto
        static const int DOT_VEL = 10;
        
        // Inicialización de las variables
        Dot();

        // Manejador de las teclas y ajust de la velocidad del punto
        void handleEvent( SDL_Event &event );

        // Mueve el punto
        void move();

        // Muestra el punto en la pantalla
        void render( int camX, int camY );

        // Obtiene las cajas de colisiones
        int getPosX();
        int getPosY();

        void setPosX( int x );
        void setPosY( int y );

    private:
        // Los Offset X y Y del punto
        int mPosX, mPosY;

        // La velocidad del punto
        int mVelX, mVelY;
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

// Textura
LTexture gDotTexture;
LTexture gBGTexture;

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

Dot::Dot()
{
    // Inicializa los offsets
    mPosX = 0;
    mPosY = 0;
 
    // Inicializa la velocidad
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event &event )
{
    // Si se presionó una tecla
    if( event.type == SDL_KEYDOWN && event.key.repeat == 0 )
    {
        // Ajusta la velocidad
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
            case SDLK_SPACE:
                printf( "X = %i\nY = %i\n", getPosX(), getPosY() );
                if( mVelY )
                    mVelY -= DOT_VEL;
        }
    }
    // Si la tecla ha sido liberada
    else if( event.type == SDL_KEYUP && event.key.repeat == 0 )
    {
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move()
{
    // Mueve el punto a la izquierda
    mPosX += mVelX;
    
    // Si el punto se aleja a la izquierda o derecha
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > LEVEL_WIDTH ) )
    {
        // Moverlo de vuelta
        mPosX -= mVelX;
    }

    // Arriba o abajo
    mPosY += mVelY;
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > LEVEL_HEIGHT ) )
    {
        mPosY -= mVelY;
    }
}

void Dot::render(int camX, int camY )
{
    // Muestra el punto relativo a la camara
    gDotTexture.render( mPosX - camX, mPosY - camY );
}

int Dot::getPosX()
{
    return mPosX;
}

int Dot::getPosY()
{
    return mPosY;
}

void Dot::setPosX( int x )
{
    mPosX = x; 
}

void Dot::setPosY( int y )
{
    mPosY = y;
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
    // Carga la textura
    if( !gDotTexture.loadFromFile( "romfs/dot.png" ) )
    {
        printf( "Falló la carga de la textura!\n" );
        return false;
    }

    if( !gBGTexture.loadFromFile( "romfs/bg.png" ) )
    {
        printf( "Falló la carga de la textura del fondo!\n" );
        return false;
    }

    return true;
}

void close() {
    // Libera la textura cargada
    gDotTexture.free();
    gBGTexture.free();

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

    // El punto que se moverá por la pantalla
    Dot dot;

    // Area de la camara
    SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

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
            
            dot.handleEvent( e );

        }
        
        // Mueve el punto y revisa la colision
        dot.move();

        // Centro de la camara sobre el punto
        // Dynamic
        camera.x += ( dot.getPosX() - camera.x - 300 ) / 20;
        camera.y += ( dot.getPosY() - camera.y - 200 ) / 20;
        // Estática
        // camera.x = ( dot.getPosX() + Dot::DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
        // camera.y = ( dot.getPosY() + Dot::DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

        // Mantener la camara en los limites
        if( camera.x < 0 )
            camera.x = 0;
        if( camera.y < 0 )
            camera.y = 0;
        if( camera.x > LEVEL_WIDTH - camera.w )
            camera.x = LEVEL_WIDTH - camera.w;
        if( camera.y > LEVEL_HEIGHT - camera.h )
            camera.y = LEVEL_HEIGHT - camera.h;

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        gBGTexture.render( 0, 0, &camera );


        // Renderiza objetos
        dot.render( camera.x, camera.y );

        // Actualiza la pantalla
        SDL_RenderPresent( gRenderer );
    }
    
    close();
    return 0;
}
