#include <stdio.h>
#include <string>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Cuenta de particulas
const int TOTAL_PARTICLES = 30;
const int NUM_STARS       = 200;

SDL_Surface* mySurface = NULL;
SDL_Renderer* myRenderer = NULL;
SDL_Texture* myTexture = NULL;
typedef struct Star
{
    int x, y, speed;
} Star;

Star stars[ NUM_STARS ];

int getStarColor( int );

void resetStars()
{
    for( int i = 0; i < NUM_STARS; ++i )
    {
        stars[ i ].x = rand()% SCREEN_WIDTH;
        stars[ i ].y = rand()% SCREEN_HEIGHT;
        stars[ i ].speed = 1 + ( rand()%12 );
    }
}

void doStars()
{
    for( int i = 0; i < NUM_STARS; ++i )
    {
        stars[ i ].x -= stars[ i ].speed;
        if( stars[ i ].x < 0 )
        {
            stars[ i ].x = SCREEN_WIDTH;
            stars[ i ].y = rand()% SCREEN_HEIGHT;
            stars[ i ].speed = 1 + ( rand()% 12 );
        }
    }
}

void updateStars()
{
    SDL_Rect rect;

    for( int i = 0; i < NUM_STARS; ++i )
    {
        if( stars[ i ].x < SCREEN_WIDTH )
        {
            rect.x = stars[ i ].x;
            rect.y = stars[ i ].y;
            rect.w = 2;
            rect.h = 2;

            SDL_FillRect( mySurface, &rect, getStarColor( stars[ i ].speed ) );
        }
    }
}

int getStarColor( int speed )
{
    SDL_Color color;
    switch(speed)
    {
        case 1: case 2: case 3:
            color.r = 159;
            color.b = 159;
            color.g = 159;
            break;
        case 4: case 5: case 6:
            color.r = 191;
            color.b = 191;
            color.g = 191;
            break;
        default:
            color.r = 255;
            color.b = 255;
            color.g = 255;
            break;
    }
    return SDL_MapRGB( mySurface->format, color.r, color.g, color.b );
}

void delay( unsigned int frameLimit )
{
    unsigned int ticks = SDL_GetTicks();

    if( frameLimit < ticks )
    {
        return;
    }
    if( frameLimit > ticks + 16 )
        SDL_Delay( 16 );
    else
        SDL_Delay( frameLimit - ticks );
}

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
                SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, int scale = 1 );
    
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

class Particle
{
    public:
        // Inicializa la posición y animación
        Particle( int x, int y );

        int mVelX = 1;
        int mVelY = 1;

        // Muestra la particula
        void render();

        // Revisa si la particula está muerta
        bool isDead();

        // Mueve la particula 
        void move();

    private:
        // Offsets
        int mPosX, mPosY;

        // Frame actual de la animación
        int mFrame;

        // Tipo de Particula
        LTexture *mTexture;
};

class Dot
{
    public:
        // Dimensiones del punto
        static const int DOT_WIDTH = 52;
        static const int DOT_HEIGHT = 60;

        // Maximá eje de velocidad de la particula
        static const int DOT_VEL = 10;
        
        // Inicialización de las variables
        Dot();

        // Libera las particulas
        ~Dot();

        // Manejador de las teclas y ajust de la velocidad del punto
        void handleEvent( SDL_Event &event );

        // Mueve el punto
        void move();

        // Muestra el punto en la pantalla
        void render();

        int getPosX();
        int getPosY();

        void setPosX( int x );
        void setPosY( int y );
        
        int getVelX();
        int getVelY();

        void setVelX( int x );
        void setVelY( int y );


    private:
        // Particulas
        Particle* particles[ TOTAL_PARTICLES ];

        // Muestra las particulas
        void renderParticles();

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
LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;
LTexture gFishyTexture;
LTexture gSpriteSheetTexture;
SDL_Rect gSpriteClips[ 97 ];

LTexture::LTexture() {
    // Inicializa la textura
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
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

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, int scale ) {
    // Espacio para renderizar y el render en pantalla
    SDL_Rect renderQuad = { x, y, mWidth*scale, mHeight*scale };

    // Establece las dimensiones del clip rendering
    if( clip != NULL ) {
        renderQuad.w = clip->w * scale;
        renderQuad.h = clip->h * scale;
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

Particle::Particle( int x, int y )
{
    // Establece los offsets
    mPosX = x - 10 + ( rand() % 25 );
    mPosY = y + 25 + ( rand() % 25 );

    // Inicializa la animación
    mFrame = rand() % 5;

    // Establece el tipo
    switch( rand() % 3 )
    {
        case 0: mTexture = &gRedTexture; break;
        case 1: mTexture = &gGreenTexture; break;
        case 2: mTexture = &gBlueTexture; break;
    }
}

void Particle::render()
{
    // Muestra la imagen
    mTexture->render( mPosX, mPosY );

    // Muestra el shimmer
    if( mFrame % 2 == 0 )
        gShimmerTexture.render( mPosX, mPosY );

    // Animación
    mFrame++;
}

void Particle::move()
{
    mPosX -= mVelX;
    mPosY += mVelY + rand() % 2;
}

bool Particle::isDead()
{
    return mFrame > 15;
}

Dot::Dot()
{
    // Inicializa los offsets
    mPosX = 0;
    mPosY = 0;

    // Inicaliza la velocidad
    mVelX = 0;
    mVelY = 0;

    // Inicialización de las particulas
    for( int i = 0; i < TOTAL_PARTICLES; ++i )
    {
        particles[ i ] = new Particle( mPosX, mPosY );
        particles[ i ]->move();
    }
}


Dot::~Dot()
{
    // Elimina las particulas
    for( int i = 0; i < TOTAL_PARTICLES; ++i )
        delete particles[ i ];
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

void Dot::move()
{
    // Mueve el punto a la izquierda
    mPosX += mVelX;

    // Si el punto fue muy lejos a la izquierda o derecha
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        mPosX -= mVelX;
    }

    mPosY += mVelY;

    // Si el punto fue muy arriba o abajo
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        // Lo mueve de vuelta
        mPosY -= mVelY;
    }
    
}

void Dot::render()
{
    // Muestra el punto
    gDotTexture.render( mPosX, mPosY );

    renderParticles();
}

void Dot::renderParticles()
{
    // Avanza por las particulas
    for( int i = 0; i < TOTAL_PARTICLES; ++i )
    {
        // Elimina y reemplaza las particulas muertas
        if( particles[ i ]->isDead() )
        {
            delete particles[ i ];
            particles[ i ] = new Particle( mPosX, mPosY );
        }
    }

    // Muestra las particulas
    for( int i = 0; i < TOTAL_PARTICLES; ++i )
    {
        particles[ i ]->render();
        particles[ i ]->move();
    }
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

int Dot::getVelX()                                                                               
{                                                                                                
    return mVelX;                                                                                
}                                                                                                
                                                                                                 
int Dot::getVelY()                                                                               
{                                                                                                
    return mVelY;                                                                                
}                                                                                                
                                                                                                 
void Dot::setVelX( int x )                                                                       
{                                                                                                
    mVelX = x;                                                                                   
}                                                                                                
                                                                                                 
void Dot::setVelY( int y )                                                                       
{                                                                                                
    mVelY = y;                                                                                   
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
        gWindow = SDL_CreateWindow( "SDL Tutorial 38 - Particulas", 
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
                SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );

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
    if( !gDotTexture.loadFromFile( "romfs/dave_cat.png" ) )
    {
        printf( "Falló la carga de la textura!\n" );
        success = false;
    }

    // Carga la textura
    if( !gRedTexture.loadFromFile( "romfs/red.bmp" ) )
    {
        printf( "Falló la carga de la textura roja!\n" );
        success = false;
    }

    // Carga la textura
    if( !gGreenTexture.loadFromFile( "romfs/green.bmp" ) )
    {
        printf( "Falló la carga de la textura verde!\n" );
        success = false;
    }

    // Carga la textura
    if( !gBlueTexture.loadFromFile( "romfs/blue.bmp" ) )
    {
        printf( "Falló la carga de la textura azul!\n" );
        success = false;
    }

    // Carga la textura
    if( !gShimmerTexture.loadFromFile( "romfs/shimmer.bmp" ) )
    {
        printf( "Falló la carga de la textura shimmer!\n" );
        success = false;
    }
    
    // Carga la textura
    if( !gFishyTexture.loadFromFile( "romfs/fishy.png" ) )
    {
        printf( "Falló la carga de la textura shimmer!\n" );
        success = false;
    }

    gSpriteSheetTexture.loadFromFile( "romfs/sheet.png" );
    gSpriteSheetTexture.setAlpha( 64 );
    for( int i = 0; i < 97; i++ )
    {
        gSpriteClips[ i ].x = i * 160;
        gSpriteClips[ i ].y = 0;
        gSpriteClips[ i ].w = 160;
        gSpriteClips[ i ].h = 160;
    }
    // Establece la transparencia de la textura
    gBlueTexture.setAlpha( 192 );
    gRedTexture.setAlpha( 192 );
    gShimmerTexture.setAlpha( 192 );
    
    return success;
}

void close() {
    // Libera la textura cargada
    gDotTexture.free();
    gRedTexture.free();
    gBlueTexture.free();
    gGreenTexture.free();
    gShimmerTexture.free();
    gFishyTexture.free();
    gSpriteSheetTexture.free();
    SDL_DestroyTexture( myTexture );
    myTexture = NULL;

    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;
    myRenderer = NULL;
    mySurface = NULL;

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

    Dot dot;
    
    int fishyX = 0;
    int fishyY = 0;
    unsigned int Limit = SDL_GetTicks() + 16;
    
    myRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED
            | SDL_RENDERER_PRESENTVSYNC );
    mySurface = SDL_CreateRGBSurface( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                      0,
                                      0,
                                      0,
                                      0 );
    myTexture = SDL_CreateTexture( gRenderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   SCREEN_WIDTH, SCREEN_HEIGHT );
    resetStars();
    double flipFish = 0.0;


    int frame = 0;

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

                          case SDLK_w:
                              for( int i = 0; i < NUM_STARS; ++i )
                                  stars[ i ].speed += ( stars[ i ].speed ) / 2;
                              break;

                          case SDLK_e:
                              for( int i = 0; i < NUM_STARS; ++i )
                                  stars[ i ].speed -= ( stars[ i ].speed ) / 2;

                          case SDLK_r:
                              //fishVelRot += 1;
                              break;

                          case SDLK_t:
                              //fishVelRot -= 1;
                              break;
                              }
            }
            
            dot.handleEvent( e );

        }

        SDL_RenderClear( gRenderer );
        
        SDL_Rect* currentClip = &gSpriteClips[ frame / 6 ];

        doStars();
        SDL_FillRect( mySurface, NULL, 0 );
        updateStars();
        SDL_UpdateTexture( myTexture, NULL, mySurface->pixels, mySurface->pitch );
        SDL_RenderCopy( gRenderer, myTexture, NULL, NULL ); 
        SDL_Delay( 1 );
        delay( Limit );
        Limit = SDL_GetTicks() + 16;
        
        gSpriteSheetTexture.render( 0, 0, currentClip, 0, NULL, SDL_FLIP_NONE, 4 );

        dot.move();

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
        // SDL_RenderClear( gRenderer );

        // Renderiza el objeto
        dot.render();

        
/*        if( dot.getVelX() == 0 && dot.getVelY() == 0 ){
            fishyX += ( dot.getPosX() - fishyX - 30 )/ 10;
            fishyY += ( dot.getPosY() - fishyY - rand()%30 ) / 10;
        } else
        {
            fishyX += ( dot.getPosX() - fishyX - 30 ) / 20;
            fishyY += ( dot.getPosY() - fishyY - 30 ) / 20;
        } */
        fishyX += ( dot.getPosX() - fishyX - 30 ) / 15;
        fishyY += ( dot.getPosY() - fishyY - 30 ) / 15;


        if( dot.getPosX() < fishyX - 20 )
            gFishyTexture.render( fishyX, 
                                  fishyY, 
                                  NULL, 
                                  flipFish + 90.0, 
                                  NULL, 
                                  SDL_FLIP_HORIZONTAL,
                                  1 );
        else
            gFishyTexture.render( fishyX, 
                                  fishyY, 
                                  NULL, 
                                  flipFish, 
                                  NULL, 
                                  SDL_FLIP_NONE,
                                  1 );

        flipFish += 4;
        if( flipFish > 360.0 )
            flipFish = 360 - flipFish;
        
        ++frame;
        if( frame / 6 >= 97 )
            frame = 0;
        // Actualiza la pantalla
        SDL_RenderPresent( gRenderer );
    }
    
    close();
    return 0;
}
