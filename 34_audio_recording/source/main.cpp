#include <stdio.h>
#include <string>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Constantes de la pantalla
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Numero de dispositivos de grabación soportados
const int MAX_RECORDING_DEVICES = 10;

// Tiempo máximo de grabado
const int MAX_RECORDING_SECONDS = 5;

// Tiempo máximo de grabado más padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

// Varias acciones de grabado que se pueden manejar
enum RecordingState
{
    SELECTING_DEVICE,
    STOPPED,
    RECORDING,
    RECORDED,
    PLAYBACK,
    ERROR
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

// Inicia SDL y crea la ventana
bool init();

// carga los archivos
bool loadMedia();

// Libera la memoria y termina SDL
void close();

// Recording/Playback callbacks
void audioRecordingCallback( void* userdata, Uint8* stream, int len );
void audioPlaybackCallback( void* userdaa, Uint8* stream, int len );

// Ventana donde se renderizará
SDL_Window* gWindow = NULL;

// Ventana del renderizado
SDL_Renderer* gRenderer = NULL;

// Fuente de texto global
TTF_Font *gFont = NULL;
SDL_Color gTextColor = { 0x00, 0x00, 0x00, 0xFF };

// Prompt Texture
LTexture gPromptTexture;

// El texto de la textura que especifica los nombres de los dispositivos de grabación
LTexture gDeviceTextures[ MAX_RECORDING_DEVICES ];

// Numero de dispositivos disponibles
int gRecordingDeviceCount = 0;

// Recieved audio spec
SDL_AudioSpec gReceivedRecordingSpec;
SDL_AudioSpec gReceivedPlaybackSpec;

// Recording data buffer
Uint8* gRecordingBuffer = NULL;

// Tamaño del buffer de datos
Uint32 gBufferByteSize = 0;

// Posición del buffer de datos
Uint32 gBufferBytePosition = 0;

// Posición máxima del buffer de datos para la grabación
Uint32 gBufferByteMaxPosition = 0;

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

bool init() {
    // Bandera
    bool success = true;

    // Inicia SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) 
    {
        printf( "SDL no pudo inicializarse! SDL Error: %s\n", SDL_GetError() );
        success = false; 
    } 
    else 
    {
        // Filtrado
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY , "1") ) 
        {
            printf( "Warning: Filtrado lineal de texturas no disponible!" );
        }

        // Crea la ventana
        gWindow = SDL_CreateWindow( "SDL Tutorial 33 - Lectura y escritura de archivos", 
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL ) 
        {
            printf( "No se pudo iniciar la ventana! SDL Error: %s\n", SDL_GetError() );
            success = false;
        } 
        else 
        {
            // Crea el renderizador
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED 
                    | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL ) 
            {
                printf( "No se pudo crear el renderizador! SDL Error: %s\n", SDL_GetError() );
                success = false;
            } 
            else 
            {
                // Inica el renderizador de color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                // Inicializa la carga de PNG
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) ) 
                {
                    printf( "SDL_Image no pudo inicializarse! SDL_image Error: %s\n", 
                            IMG_GetError() );
                    success = false;
                }

            }
            if( TTF_Init() == -1 )
            {
                printf( "SDL_ttf no pudo inicializarse! SDL_ttf Error: %s\n", TTF_GetError() );
                success = false;
            }
        }
    }

    return success;
}

bool loadMedia() 
{
    bool success = true;
    // Carga la fuente
    gFont = TTF_OpenFont( "romfs/lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Falló la carga de la fuente! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
       
    // Renderiza el texto
    gPromptTexture.loadFromRenderedText( "Select your recording device: ", gTextColor );

    // Get Capture device count
    gRecordingDeviceCount = SDL_GetNumAudioDevices( SDL_TRUE );

    // No recording devices
    if( gRecordingDeviceCount < 1 )
    {
        printf( "No se pudo obtener el dispositivo de captura de audio! SDL Error: %s\n",
                SDL_GetError() );
        printf( "%i\n", gRecordingDeviceCount );
        success = false;
    }
    // Al menos un dispositivo conectado
    else 
    {
        // Cap recording device count
        if( gRecordingDeviceCount > MAX_RECORDING_DEVICES )
            gRecordingDeviceCount = MAX_RECORDING_DEVICES;

        // Renderiza los nombres
        std::stringstream promptText;
        for( int i = 0; i < gRecordingDeviceCount; ++i )
        {
            // Obtiene los nombres
            promptText.str( "" );
            promptText << i << ": " << SDL_GetAudioDeviceName( i, SDL_TRUE );

            // Establece la textura por nombre
            gDeviceTextures[ i ].loadFromRenderedText( promptText.str().c_str(), gTextColor );
        }
    }


    return success;
}

void close() 
{
    // Libera la textura cargada
    gPromptTexture.free();
    for( int i = 0; i < MAX_RECORDING_DEVICES; ++i )
    {
        gDeviceTextures[ i ].free();
    }

    // Libera la fuente global
    TTF_CloseFont( gFont );
    gFont = NULL;

    // Destruye la ventana
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    // free audio playback
    if( gRecordingBuffer != NULL )
    {
        delete[] gRecordingBuffer;
        gRecordingBuffer = NULL;
    }

    // Termina SDL
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void audioRecordingCallback( void* userdata, Uint8* stream, int len )
{
    // Copia el audio del stream
    memcpy( &gRecordingBuffer[ gBufferBytePosition ], stream, len );

    // Se mueve a traves del buffer
    gBufferBytePosition += len;
}

void audioPlaybackCallback( void* userdata, Uint8* stream, int len )
{
    // Copia el audio al stream
    memcpy( stream, &gRecordingBuffer[ gBufferBytePosition ], len );

    // Se mueve a traves del buffer
    gBufferBytePosition += len;
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

    // Set the default recording state
    RecordingState currentState = SELECTING_DEVICE;

    // Audio device ID's
    SDL_AudioDeviceID recordingDeviceId = 0;
    SDL_AudioDeviceID playbackDeviceId = 0;

    while( !quit ) {
        bool renderText = false;
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT )
                quit = true;

            // Do current event state handling
            switch( currentState )
            {
                // User is selecting recording device
                case SELECTING_DEVICE:

                    if( e.type == SDL_KEYDOWN )
                    {
                        // Handle key press from 0 to 9
                        if( e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9 )
                        {
                            // Get selection index
                            int index = e.key.keysym.sym - SDLK_0;
    
                            // Index is valid
                            if( index < gRecordingDeviceCount )
                            {
                                // Default audio spec
                                SDL_AudioSpec desiredRecordingSpec;
                                SDL_zero( desiredRecordingSpec );
                                desiredRecordingSpec.freq = 44100;
                                desiredRecordingSpec.format = AUDIO_F32;
                                desiredRecordingSpec.channels = 2;
                                desiredRecordingSpec.samples = 4096;
                                desiredRecordingSpec.callback = audioRecordingCallback;
    
                                // Open recording device
                                recordingDeviceId = SDL_OpenAudioDevice( SDL_GetAudioDeviceName
                                        ( index, SDL_TRUE ), SDL_TRUE, &desiredRecordingSpec, 
                                        &gReceivedRecordingSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
    
                                // Device failed to open
                                if( recordingDeviceId == 0 )
                                {
                                    // Reporta el error
                                    printf( "Falló en abrir el dispositivo de grabación! SDL Error: %s\n", SDL_GetError() );
                                    gPromptTexture.loadFromRenderedText( "Falló en abrir el dispositivo de grabación!", gTextColor );
                                    currentState = ERROR;
                                }
                                // Device opened successfully
                                else
                                {
                                    // Default audio spec
                                    SDL_AudioSpec desiredPlaybackSpec;
                                    SDL_zero( desiredPlaybackSpec );
                                    desiredPlaybackSpec.freq = 44100;
                                    desiredPlaybackSpec.format = AUDIO_F32;
                                    desiredPlaybackSpec.channels = 2;
                                    desiredPlaybackSpec.samples = 4096;
                                    desiredPlaybackSpec.callback = audioRecordingCallback;
                                    
                                    // Open playback device
                                    playbackDeviceId = SDL_OpenAudioDevice( NULL, SDL_FALSE, &desiredPlaybackSpec, &gReceivedPlaybackSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE );
    
                                    // Device failed to open
                                    if( playbackDeviceId == 0 )
                                    {
                                        // Reporta el error
                                        printf( "Falló en abrir el dispositivo de repoducción! SDL Erorr: %s\n", SDL_GetError() );
                                        gPromptTexture.loadFromRenderedText( "Falló en abrir el dispositivo de reproducción!", gTextColor );
                                        currentState = ERROR;
                                    }
                                    // Device opened successfully
                                    else
                                    {
                                        // Calculate per sample bytes
                                        int bytesPerSample = gReceivedRecordingSpec.channels * ( SDL_AUDIO_BITSIZE( gReceivedRecordingSpec.format ) / 8 );
    
                                        // Calculate bytes per second
                                        int bytesPerSecond = gReceivedRecordingSpec.freq * bytesPerSample;
    
                                        // Calculate max buffer use
                                        gBufferByteMaxPosition = MAX_RECORDING_SECONDS * bytesPerSecond;
    
                                        // Allocate and initialize byte buffer
                                        gRecordingBuffer = new Uint8[ gBufferByteSize ];
                                        memset( gRecordingBuffer, 0, gBufferByteSize );
    
                                        // Go on to next state
                                        gPromptTexture.loadFromRenderedText( "Press 1 to record for 5 seconds", gTextColor );
                                        currentState = STOPPED;
                                    }
                                }
                            }
                        }
                    }
                    break;

                // User getting ready to record
                case STOPPED:
                    // On key press
                    if( e.type == SDL_KEYDOWN )
                    {
                        // Start playback
                        if( e.key.keysym.sym == SDLK_1 )
                        {
                            // Go back to beginning of buffer
                            gBufferBytePosition = 0;
    
                            // Start recording
                            SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );
    
                            // Go on the next state
                            gPromptTexture.loadFromRenderedText( "Recording...", gTextColor );
    
                            currentState = RECORDING;
                        }
                    }
                break;
    
                // User has finished recording
                case RECORDED:
                    // On key press
                    if( e.type == SDL_KEYDOWN )
                    {
                        // Start playback
                        if( e.key.keysym.sym == SDLK_1 )
                        {
                            // Go back to beginning of buffer
                            gBufferBytePosition = 0;
    
                            // Start playback
                            SDL_PauseAudioDevice( playbackDeviceId, SDL_FALSE );
    
                            // Go on to the next state
                            gPromptTexture.loadFromRenderedText( "Playing...", gTextColor );
    
                            currentState = PLAYBACK;
                        }
                        // Record again
                        if( e.key.keysym.sym == SDLK_2 )
                        {
                            // Reset the buffer
                            gBufferBytePosition = 0;
                            memset( gRecordingBuffer, 0, gBufferByteSize );
    
                            // Start recording
                            SDL_PauseAudioDevice( recordingDeviceId, SDL_FALSE );
    
                            // Go on to next state
                            gPromptTexture.loadFromRenderedText( "Recording...", gTextColor );

                            currentState = RECORDING;
                        }
                    }
                    break;
                }
        }

        // Actualizando la grabación
        if( currentState == RECORDING )
        {
            // Lock Callback
            SDL_LockAudioDevice( recordingDeviceId );

            // Finished recording
            if( gBufferBytePosition > gBufferByteMaxPosition )
            {
                // Stop recording audio
                SDL_PauseAudioDevice( recordingDeviceId, SDL_TRUE );

                // go on to next stage
                gPromptTexture.loadFromRenderedText( "Press 1 to play back. Press 2 to record again.", gTextColor );
                currentState = RECORDED;
            }
            
            // Unlock Callback
            SDL_UnlockAudioDevice( recordingDeviceId );
        }
        // Actualizando playback
        else if( currentState == PLAYBACK )
        {
            // Lock playback
            SDL_LockAudioDevice( playbackDeviceId );

            // Finished playback
            if( gBufferBytePosition > gBufferByteMaxPosition )
            {
                // Stop playing audio
                SDL_PauseAudioDevice( playbackDeviceId, SDL_TRUE );

                // Go on to next state
                gPromptTexture.loadFromRenderedText( "Press 1 to playback. Press 2 to record again.", gTextColor );
                currentState = RECORDED;
            }

            // Unlock callback
            SDL_UnlockAudioDevice( playbackDeviceId );
        }

        // Limpia la pantalla
        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        // Rendereiza las texturas del texto
        gPromptTexture.render( ( SCREEN_WIDTH - gPromptTexture.getWidth() ) / 2, 0 );
    
        // User is selecting
        if( currentState == SELECTING_DEVICE )
        {
            // Render device names
            int yOffset = gPromptTexture.getHeight() * 2;
            for( int i = 0; i < gRecordingDeviceCount; ++i )
            {
                gDeviceTextures[ i ].render( 0, yOffset );
                yOffset += gDeviceTextures[ i ].getHeight() + 1;
            }
        }

        SDL_RenderPresent( gRenderer );
    }

    close();
    return 0;
}
