#include <stdio.h>
#include <string>
#include <sstream>
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

class LWindow
{
    public:
        // Constructor
        LWindow();

        // Crea la ventana
        bool init();

        // Maneja los eventos de la ventana
        void handleEvent( SDL_Event& e );

        // Enfoca en la ventana
        void focus();

        // Libera los recursos
        void free();

        void render();

        // Dimensiones de la ventana
        int getWidth();
        int getHeight();

        // Window focii(?)
        bool hasMouseFocus();
        bool hasKeyboardFocus();
        bool isMinimized();
        bool isShown();

    private:
        // Datos de la ventana
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        int mWindowID;
        int mWindowDisplayID;

        // Dimensiones de la ventana
        int mWidth;
        int mHeight;

        // Window focus
        bool mMouseFocus;
        bool mKeyboardFocus;
        bool mFullScreen;
        bool mMinimized;
        bool mShown;
};

// Inicia SDL y crea la ventana
bool init();

// carga los archivos
bool loadMedia();

// Libera la memoria y termina SDL
void close();

// Ventana custom
LWindow gWindow;

// Display data
int gTotalDisplays = 0;
SDL_Rect* gDisplayBounds = NULL;

// Ventana del renderizado
SDL_Renderer* gRenderer = NULL;

// Texturas de la escena
LTexture gSceneTexture;

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

LWindow::LWindow()
{
    // Inicializa la ventana no existente
    mWindow = NULL;
    mMouseFocus = false;
    mKeyboardFocus = false;
    mFullScreen = false;
    mMinimized = false;
    mWidth = 0;
    mHeight = 0;
}

bool LWindow::init()
{
    // Crea la ventana
    mWindow = SDL_CreateWindow( "SDL Tutorial 36 - Multiples Ventanas", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );

    if( mWindow != NULL )
    {
        mMouseFocus = true;
        mKeyboardFocus = true;
        mWidth = SCREEN_WIDTH;
        mHeight = SCREEN_HEIGHT;

        // Crea el renderer para la ventana
        mRenderer = SDL_CreateRenderer( mWindow, -1, 
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
        if( mRenderer == NULL )
        {
            printf( "No se pudo iniciar el renderer! SDL Error: %s\n", SDL_GetError() );
            mWindow = NULL;
        }
        else
        {
            // Inicializa el renderer color
            SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            
            // Identificador de la ventana
            mWindowID = SDL_GetWindowID( mWindow );
            mWindowDisplayID = SDL_GetWindowDisplayIndex( mWindow );

            // Flag 
            mShown = true;
        }
    }
    else
    {
        printf( "No se pudo crear la ventana! SDL Error: %s\n", SDL_GetError() );
    }

    return mWindow != NULL && mRenderer != NULL;
}

void LWindow::handleEvent( SDL_Event& e )
{   
    // Actualiza la bandera 
    bool updateCaption = false;

    // Ocurre evento de ventana
    if( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
    {
        switch( e.window.event )
        {
            // La ventana se movió
            case SDL_WINDOWEVENT_MOVED:
                mWindowDisplayID = SDL_GetWindowDisplayIndex( mWindow );
                updateCaption = true;
                break;

            // La Ventana aparece
            case SDL_WINDOWEVENT_SHOWN:
                mShown = true;
                break;

            // La Ventana desaparece
            case SDL_WINDOWEVENT_HIDDEN:
                mShown = false;
                break;

            // Obtiene las nuevas dimensiones de la ventana y redibuja en la ventana el cambio
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                mWidth = e.window.data1;
                mHeight = e.window.data2;
                SDL_RenderPresent( gRenderer );
                updateCaption = true;
                break;

            // Redibuja al exponerse
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent( gRenderer );
                break;

            // Mouse entra en la ventana
            case SDL_WINDOWEVENT_ENTER:
                mMouseFocus = true;
                updateCaption = true;
                break;

            // Mouse sale de la ventana
            case SDL_WINDOWEVENT_LEAVE:
                mMouseFocus = false;
                updateCaption = true;
                break;

            // La ventana tiene el teclado enfocado
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                mKeyboardFocus = true;
                updateCaption = true;
                break;

            // La ventana pierde el enfoque del teclado
            case SDL_WINDOWEVENT_FOCUS_LOST:
    
                mKeyboardFocus = false;
                updateCaption = true;

            // Ventana minimizada
            case SDL_WINDOWEVENT_MINIMIZED:
                mMinimized = true;
                break;

            // Ventana maximizada
            case SDL_WINDOWEVENT_MAXIMIZED:
                mMinimized = false;
                break;

            // Ventana restaurada
            case SDL_WINDOWEVENT_RESTORED:
                mMinimized = false;
                break;

            // Esconde al cerrar
            case SDL_WINDOWEVENT_CLOSE:
                SDL_HideWindow( mWindow );
                break;
        }
    }
    else if( e.type == SDL_KEYDOWN )
    {
        // Display change flag
        bool switchDisplay = false;

        // Cycle though displays on up/down
        switch( e.key.keysym.sym )
        {
            case SDLK_UP:
                ++mWindowDisplayID;
                switchDisplay = true;
                break;

            case SDLK_DOWN:
                --mWindowDisplayID;
                switchDisplay = true;
                break;
        }

        // se necesita actualizar el display
        if( switchDisplay )
        {
            // Bound display index
            if( mWindowDisplayID < 0 )
            {
                mWindowDisplayID = gTotalDisplays - 1;
            }
            else if( mWindowDisplayID >= gTotalDisplays )
            {
                mWindowDisplayID = 0;
            }

            // mueve la ventana al centro del siguiente display
            SDL_SetWindowPosition( mWindow, gDisplayBounds[ mWindowDisplayID ].x 
                    + ( gDisplayBounds[ mWindowDisplayID ].w - mWidth ) / 2,
                    gDisplayBounds[ mWindowDisplayID ].y 
                    + ( gDisplayBounds[ mWindowDisplayID ].h - mHeight ) / 2 );

            updateCaption = true;
        }
    }

    // Acutaliza el texto de la ventana con el nuevo dato
    if( updateCaption )
    {
        std::stringstream caption;
        caption << "SDL Tutorial 37 - ID: " << mWindowID << " Display: " << mWindowDisplayID
            << " MouseFocus: " << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus: "
            << ( ( mKeyboardFocus ) ? "On" : "Off" );
        SDL_SetWindowTitle( mWindow, caption.str().c_str() );
    }


    // Entra/Sale del modo de pantalla completa con la tecla de return
    else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
    {
        if( mFullScreen )
        {
            SDL_SetWindowFullscreen( mWindow, SDL_FALSE );
            mFullScreen = false;
        }
        else 
        {
            SDL_SetWindowFullscreen( mWindow, SDL_TRUE );
            mFullScreen = true;
            mMinimized = false;
        }
    }
}

void LWindow::render()
{
    if( !mMinimized )
    {
        // Limpia la pantalla
        SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( mRenderer );

        // Actualiza la pantalla
        SDL_RenderPresent( mRenderer );
    }
}

void LWindow::focus()
{
    // Restaura la ventaan si es necesario
    if( !mShown )
    {
        SDL_ShowWindow( mWindow );
    }

    // Mueve la ventana al frente
    SDL_RaiseWindow( mWindow );
}

void LWindow::free()
{
    if( mWindow != NULL )
    {
        SDL_DestroyWindow( mWindow );
    }

    mMouseFocus = false;
    mKeyboardFocus = false;
    mWidth = 0;
    mHeight = 0;
}

int LWindow::getWidth()
{
    return mWidth;
}

int LWindow::getHeight()
{
    return mHeight;
}

bool LWindow::hasMouseFocus()
{
    return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
    return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
    return mMinimized;
}

bool LWindow::isShown()
{
    return mShown;
}

bool init() {
    // Bandera
    bool success = true;

    // Inicia SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
    {
        printf( "SDL no pudo inicializarse! SDL Error: %s\n", SDL_GetError() );
        success = false; 
    } 
    else 
    {
        // Filtrado
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY , "1") ) {
            printf( "Warning: Filtrado lineal de texturas no disponible!" );
        }

        // Get number of displays
        gTotalDisplays = SDL_GetNumVideoDisplays();
        if( gTotalDisplays < 2 )
        {
            printf( "Warning: Solo hay 1 display conectado!\n" );
        }

        // Get bounds of each display
        gDisplayBounds = new SDL_Rect[ gTotalDisplays ];
        for( int i = 0; i < gTotalDisplays; ++i )
        {
            SDL_GetDisplayBounds( i, &gDisplayBounds[ i ] );
        }

        // Crea la ventana
        if( !gWindow.init() ) 
        {
            printf( "No se pudo iniciar la ventana 0! SDL Error: %s\n", SDL_GetError() );
            success = false;
        } 
    }

    return success;
}

bool loadMedia() {
    bool success = true;
    
    return success;
}

void close() {
    // Destruye las ventanas
    gWindow.free();

    // Termina SDL
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
            }
            
            // Maneja eventos de la ventana
            gWindow.handleEvent( e );
        }

        // Actualiza la ventana
        gWindow.render();
    }
    
    close();
    return 0;
}
