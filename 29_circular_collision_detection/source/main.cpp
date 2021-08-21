#include <stdio.h>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Estructura del circulo
struct Circle
{
    int x, y;
    int r;
};

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

class Dot
{
    public:
        // Dimensiones del punto
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        // Maximá eje de velocidad del punto
        static const int DOT_VEL = 1;
        
        // Inicialización de las variables
        Dot( int x, int y );

        // Manejador de las teclas y ajust de la velocidad del punto
        void handleEvent( SDL_Event &event );

        // Mueve el punto
        void move( SDL_Rect& square, Circle& circle );

        // Muestra el punto en la pantalla
        void render();

        // Obtiene las cajas de colisiones
        Circle& getCollider();

    private:
        // Los Offset X y Y del punto
        int mPosX, mPosY;

        // La velocidad del punto
        int mVelX, mVelY;

        // Circulo de colisiones
        Circle mCollider;

        // Mueve las cajas de colisiones relativas al offset del punto
        void shiftColliders();
};

// Inicia SDL y crea la ventana
bool init();

// carga los archivos
bool loadMedia();

// Libera la memoria y termina SDL
void close();

// Detector de colisiones circulo-circulo
bool checkCollision( Circle& a, Circle& b );

// Detector de colisiones circulo-caja
bool checkCollision( Circle& a, SDL_Rect& b );

// Calcula el cuadrado de la distancia entre dos puntos
double distanceSquared( int x1, int y1, int x2, int y2 );

// Ventana donde se renderizará
SDL_Window* gWindow = NULL;

// Ventana del renderizado
SDL_Renderer* gRenderer = NULL;

// Textura
LTexture gDotTexture;

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

Dot::Dot( int x, int y )
{
    // Inicializa los offsets
    mPosX = x;
    mPosY = y;

    // Crea los SDL_Rect necesarios
    mCollider.r = DOT_WIDTH / 2;

    // Inicializa la velocidad
    mVelX = 0;
    mVelY = 0;

    // Inicializa las cajas de colision relativas a la posición
    shiftColliders();
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

void Dot::move( SDL_Rect& square, Circle& circle )
{
    // Mueve el punto a la izquierda
    mPosX += mVelX;
    shiftColliders();

    // Si el punto fue muy lejos a la izquierda o derecha
    if( ( mPosX - mCollider.r < 0 ) || ( mPosX + mCollider.r > SCREEN_WIDTH )
                || checkCollision( mCollider, square ) || checkCollision( mCollider, circle ) )
    {
        mPosX -= mVelX;
        shiftColliders();
    }

    mPosY += mVelY;
    shiftColliders();
    
    // Si el punto fue muy arriba o abajo
    if( ( mPosY - mCollider.r < 0 ) || ( mPosY + mCollider.r > SCREEN_HEIGHT ) 
            || checkCollision( mCollider, square ) || checkCollision( mCollider, circle ) )
    {
        // Lo mueve de vuelta
        mPosY -= mVelY;
        shiftColliders();
    }   
}

void Dot::shiftColliders()
{
    // Alinea los colliders al centro del punto
    mCollider.x = mPosX;
    mCollider.y = mPosY;
}

void Dot::render()
{
    // Muestra el punto
    gDotTexture.render( mPosX - mCollider.r, mPosY - mCollider.r );
}

bool checkCollision( Circle& a, Circle& b )
{
    // Calcula el radio total al cuadrado(?)
    int totalRadiusSquared = a.r + b.r;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    // Si la distancia entre los centros de los circulos es menor que la suma de sus radios
    if( distanceSquared( a.x, a.y, b.x, b.y ) < ( totalRadiusSquared ) )
    {
        // Los circulos colisionan
        return true;
    }

    // Si no
    return false;
}

bool checkCollision( Circle& a, SDL_Rect& b )
{
    // Punto más cercano a la caja de colision
    int cX, cY;

    // Encuentra el offset x más cercano
    if( a.x < b.x )
    {
        cX = b.x;
    }
    else if ( a.x > b.x + b.w )
    {
        cX = b.x + b.w;
    }
    else
    {
        cX = a.x;
    }

    // Encontar el offset y más cercano
    if( a.y < b.y )
    {
        cY = b.y;
    }
    else if ( a.y > b.y + b.h )
    {
        cY = b.y + b.h;
    }
    else 
    {
        cY = a.y;
    }

    // Si el punto más cercano está dentro del circulo
    if( distanceSquared( a.x, a.y, cX, cY ) < a.r * a.r )
    {
        // La caja y el circulo colisionan
        return true;
    }
    
    // Si no
    return false;
}

Circle& Dot::getCollider()
{
    return mCollider;
}

double distanceSquared( int x1, int y1, int x2, int y2 )
{
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    return deltaX * deltaX + deltaY * deltaY;
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
        gWindow = SDL_CreateWindow( "SDL Tutorial 28 - Collision Per-Pixel Collision Detection", 
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
    // Bandera
    bool success = true;
    
    // Carga la textura
    if( !gDotTexture.loadFromFile( "romfs/dot.bmp" ) )
    {
        printf( "Falló la carga de la textura!\n" );
        success = false;
    }

    return success;
}

void close() {
    // Libera la textura cargada
    gDotTexture.free();

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
    Dot dot( Dot::DOT_WIDTH / 2, Dot::DOT_HEIGHT / 2 );

    // El punto con el que será colisionado
    Dot otherDot( SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 );

    // Pone el muro
    SDL_Rect wall;
    wall.x = 300;
    wall.y = 40;
    wall.w = 40;
    wall.h = 400;

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
        dot.move( wall, otherDot.getCollider() );

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Renderiza el muro
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderDrawRect( gRenderer, &wall );

        // Renderiza el punto
        dot.render();
        otherDot.render();

        // Actualiza la pantalla
        SDL_RenderPresent( gRenderer );
    }
    
    close();
    return 0;
}
