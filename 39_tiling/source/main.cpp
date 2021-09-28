#include <stdio.h>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Constantes de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Constantes del nivel
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

// Constantes de los tiles
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

// Los diferentes tiles del sprite
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

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

class Tile
{
    public:
        // Inicialza la posición y el tipo
        Tile( int x, int y, int tileType );

        // Muestra el tile
        void render( SDL_Rect& camera );

        // Obtiene el tipo del tile
        int getType();

        // Obtiene la caja de colisiones
        SDL_Rect getBox();

    private:
        // Atributos del tile
        SDL_Rect mBox;

        // Tipo del tile
        int mType;
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
        void move( Tile* tiles[] );

        // Camara
        void setCamera( SDL_Rect& camera );

        // Muestra el punto en la pantalla
        void render( SDL_Rect& camera );

        // Obtiene las cajas de colisiones
        /* int getPosX();
        int getPosY();

        void setPosX( int x );
        void setPosY( int y ); */

    private:
        // Caja de colisiones del punto - antes se usaba mBox.x-mBox.y
        SDL_Rect mBox;

        // La velocidad del punto
        int mVelX, mVelY;
};

// Inicia SDL y crea la ventana
bool init();

// carga los archivos
bool loadMedia( Tile* tiles[] );

// Libera la memoria y termina SDL
void close( Tile* tiles[] );

// Detector de colisiones
bool checkCollision( SDL_Rect a, SDL_Rect b );

// Revisa las cajas de colisiones de un set de tiles
bool touchesWall( SDL_Rect box, Tile* tiles[] );

bool setTiles( Tile* tiles[] );

// Ventana donde se renderizará
SDL_Window* gWindow = NULL;

// Ventana del renderizado
SDL_Renderer* gRenderer = NULL;

// Textura
LTexture gDotTexture;
LTexture gTileTexture;
SDL_Rect gTileClips[ TOTAL_TILE_SPRITES ];

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

Tile::Tile( int x, int y, int tileType )
{
    // Obtiene los offsets
    mBox.x = x;
    mBox.y = y;

    // Establece la caja de colisiones
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    // Obtiene el tipo de tile
    mType = tileType;
}

void Tile::render( SDL_Rect& camera )
{
    // Si el tile está en pantalla
    if( checkCollision( camera, mBox ) )
    {
        // Muestra el tile
        gTileTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips[ mType ] );
    }
}

int Tile::getType()
{
    return mType;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}

Dot::Dot()
{
    // Inicializa los offsets
    /*mBox.x = 0;
    mBox.y = 0; */

    mBox.x = 0;
    mBox.y = 0;
    mBox.w = DOT_WIDTH;
    mBox.h = DOT_HEIGHT;
 
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
            case SDLK_UP: case SDLK_w: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: case SDLK_s: mVelY += DOT_VEL; break;
            case SDLK_LEFT: case SDLK_a: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: case SDLK_d: mVelX += DOT_VEL; break;
        }
    }
    // Si la tecla ha sido liberada
    else if( event.type == SDL_KEYUP && event.key.repeat == 0 )
    {
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: case SDLK_w: mVelY += DOT_VEL; break;
            case SDLK_DOWN: case SDLK_s: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: case SDLK_a: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: case SDLK_d: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move( Tile *tiles[] )
{
    // Mueve el punto a la izquierda
    mBox.x += mVelX;
    
    // Si el punto se aleja a la izquierda o derecha, o está tocando el muro
    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_WIDTH ) || touchesWall( mBox, tiles ) )
    {
        // Moverlo de vuelta
        mBox.x -= mVelX;
    }

    // Arriba o abajo
    mBox.y += mVelY;
    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || touchesWall( mBox, tiles ) )
    {
        mBox.y -= mVelY;
    }
}

void Dot::setCamera( SDL_Rect& camera )
{
    // Camara centrada a la posición del punto
    /* camera.x = ( mBox.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
    camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - SCREEN_WIDTH / 2; */

    // Camara con movimiento suave no centrada en el punto
    camera.x += ( mBox.x - camera.x - 300 ) / 20;
    camera.y += ( mBox.y - camera.y - 200 ) / 20;
    
    // Mantener la camara en los límites del nivel
    if( camera.x < 0 )
        camera.x = 0;
    if( camera.y < 0 )
        camera.y = 0;
    if( camera.x > LEVEL_WIDTH - camera.w )
        camera.x = LEVEL_WIDTH - camera.w;
    if( camera.y > LEVEL_HEIGHT - camera.h )
        camera.y = LEVEL_HEIGHT - camera.h;
}

void Dot::render( SDL_Rect& camera )
{
    // Muestra el punto relativo a la camara
    gDotTexture.render( mBox.x - camera.x, mBox.y - camera.y );
}

/*void Dot::setPosX( int x )
{
    mBox.x = x; 
}

void Dot::setPosY( int y )
{
    mBox.y = y;
}*/
    
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

bool loadMedia( Tile* tiles[] ) {
    bool success = true;

    // Carga la textura
    if( !gDotTexture.loadFromFile( "romfs/dot.png" ) )
    {
        printf( "Falló al cargar de la textura!\n" );
        return false;
    }

    if( !gTileTexture.loadFromFile( "romfs/tiles.png" ) )
    {
        printf( "Falló al cargar de la textura de los tiles!\n" );
        return false;
    }

    // Carga el tile map
    if( !setTiles( tiles ) )
    {
        printf( "Falló al cargar" );
        success = false;
    }

    return success;
}

void close( Tile* tiles[] ) 
{
    // Libera los tiles    
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        if( tiles[ i ] != NULL )
        {
            delete tiles[ i ];
            tiles[ i ] = NULL;
        }
    }

    // Libera la textura cargada
    gDotTexture.free();
    gTileTexture.free();

    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    // Termina SDL
    IMG_Quit();
    SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    // Lados del rectangulo
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    // Calcula los lados del Rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    // Si los lados desde A hacia fuera de B
    if ( bottomA <= topB )
        return false;
    
    if( topA >= bottomB )
        return false;
    
    if( rightA <= leftB )
        return false;

    if( leftA >= rightB )
        return false;

    // Si no se cumple ninguna condición
    return true;
}

bool setTiles( Tile* tiles[] )
{
    // Bandera
    bool tilesLoaded = true;

    // Offsets de los tiles
    int x = 0, y = 0;
    
    // Abre el archivo del mapa
    std::ifstream map( "romfs/lazy.map" );

    // Si el mapa no se pudo cargar
    if( map.fail() )
    {
        printf( "No se pudo cargar el archivo del mapa!\n" );
        tilesLoaded = false;
    }
    else
    {
        // Inicializa los tiles
        for( int i = 0; i < TOTAL_TILES; ++i )
        {
            // Determina que tipo de tile será
            int tileType = -1;

            // Lee el archivo del mapa
            map >> tileType;

            // Si hay un problema con la lectura
            if( map.fail() )
            {
                // Detiene la carga del mapa
                printf( "Error cargando el mapa. Fin de archivo inesperado!\n" );
                tilesLoaded = false;
                break;
            }

            // Si el número de tiles es válido
            if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
            {
                tiles[ i ] = new Tile( x, y, tileType );
            }
            // Si no se reconoce el patrón de algún tile
            else
            {
                // Detiene la carga del mapa(?)
                printf( "Error cargando el mapa. Tipo invalido de tile en: %d!\n", i );
                tilesLoaded = false;
                break;
            }

            // Mueve al siguiente espacio de tile
            x += TILE_WIDTH;

            // Si llega al límite del nivel
            if( x >= LEVEL_WIDTH )
            {
                // Regresa
                x = 0;

                // Mueve al siguiente fila
                y += TILE_HEIGHT;
            }
        }

        // Recortado de la hoja de tiles
        if( tilesLoaded )
        {
            gTileClips[ TILE_RED ].x = 0;
            gTileClips[ TILE_RED ].y = 0;
            gTileClips[ TILE_RED ].w = TILE_WIDTH;
            gTileClips[ TILE_RED ].h = TILE_HEIGHT;


            gTileClips[ TILE_GREEN ].x = 0;
            gTileClips[ TILE_GREEN ].y = 80;
            gTileClips[ TILE_GREEN ].w = TILE_WIDTH;
            gTileClips[ TILE_GREEN ].h = TILE_HEIGHT;


            gTileClips[ TILE_BLUE ].x = 0;
            gTileClips[ TILE_BLUE ].y = 160;
            gTileClips[ TILE_BLUE ].w = TILE_WIDTH;
            gTileClips[ TILE_BLUE ].h = TILE_HEIGHT;

            gTileClips[ TILE_TOPLEFT ].x = 80;
            gTileClips[ TILE_TOPLEFT ].y = 0;
            gTileClips[ TILE_TOPLEFT ].w = TILE_WIDTH;
            gTileClips[ TILE_TOPLEFT ].h = TILE_HEIGHT;

            gTileClips[ TILE_LEFT ].x = 80;
            gTileClips[ TILE_LEFT ].y = 80;
            gTileClips[ TILE_LEFT ].w = TILE_WIDTH;
            gTileClips[ TILE_LEFT ].h = TILE_HEIGHT;

            gTileClips[ TILE_BOTTOMLEFT ].x = 80;
            gTileClips[ TILE_BOTTOMLEFT ].y = 160;
            gTileClips[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
            gTileClips[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

            gTileClips[ TILE_TOP ].x = 160;
            gTileClips[ TILE_TOP ].y = 0;
            gTileClips[ TILE_TOP ].w = TILE_WIDTH;
            gTileClips[ TILE_TOP ].h = TILE_HEIGHT;

            gTileClips[ TILE_CENTER ].x = 160;
            gTileClips[ TILE_CENTER ].y = 80;
            gTileClips[ TILE_CENTER ].w = TILE_WIDTH;
            gTileClips[ TILE_CENTER ].h = TILE_HEIGHT;

            gTileClips[ TILE_BOTTOM ].x = 160;
            gTileClips[ TILE_BOTTOM ].y = 160;
            gTileClips[ TILE_BOTTOM ].w = TILE_WIDTH;
            gTileClips[ TILE_BOTTOM ].h = TILE_HEIGHT;

            gTileClips[ TILE_TOPRIGHT ].x = 240;
            gTileClips[ TILE_TOPRIGHT ].y = 0;
            gTileClips[ TILE_TOPRIGHT ].w = TILE_WIDTH;
            gTileClips[ TILE_TOPRIGHT ].h = TILE_HEIGHT;
            
            gTileClips[ TILE_RIGHT ].x = 240;
            gTileClips[ TILE_RIGHT ].y = 80;
            gTileClips[ TILE_RIGHT ].w = TILE_WIDTH;
            gTileClips[ TILE_RIGHT ].h = TILE_HEIGHT;

            gTileClips[ TILE_BOTTOMRIGHT ].x = 240;
            gTileClips[ TILE_BOTTOMRIGHT ].y = 160;
            gTileClips[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
            gTileClips[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
        }
    }
    
    // Cierra el mapa
    map.close();

    // Si el mapa se cargó bien
    return tilesLoaded;
}

bool touchesWall( SDL_Rect box, Tile* tiles[] )
{
    // Avanza a través de los tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        // Si el tile es de tipo tile muro
        if( ( tiles[ i ]->getType() > TILE_CENTER ) && ( tiles[ i ]->getType() <= TILE_TOPLEFT) )
        {
            // Revisa si la caja de colisiones toca el muro
            if( checkCollision( box, tiles[ i ]->getBox() ) )
            {
                return true;
            }
        }
    }

    // Si no se tocó ningún muro
    return false;
}

int main( int argc, char* argv[] ) {
    // Inicia SDL y crea la ventana
    if( !init() ) {
        printf( "Falló la inicialización!\n" );
        return -1;
    } 
    else 
    {    
        Tile* tileSet[ TOTAL_TILES ];

        // Los tiles del nivel
        if( !loadMedia( tileSet ) ) {
            printf( "Falló la carga de archivos!\n" );
            return -1;
        }
        else
        {        
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
                dot.move( tileSet );
                dot.setCamera( camera );

                // Limpia la pantalla
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                // Renderiza el nivel
                for( int i = 0; i < TOTAL_TILES; ++i )
                {
                    tileSet[ i ]->render( camera );
                }

                // Renderiza objetos
                dot.render( camera );

                // Actualiza la pantalla
                SDL_RenderPresent( gRenderer );
            }

        }   
    
        // Libera los recursos y cierra
        close( tileSet );
    }

    return 0;
}
