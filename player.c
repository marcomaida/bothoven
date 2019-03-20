#include "player.h"
#include <stdio.h>
#include "portaudio.h"
#include "piece.h"
#include "utils.h"
#include "ll.h"

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (128)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (8192)
  
const double overtones [] = { 1. };
const int overtonesCount = 1;

typedef struct pn
{
    double frequency;
    unsigned long time;
    unsigned long duration;
} player_note;

typedef struct 
{
    float sine[TABLE_SIZE];
    list * notes;
    unsigned long frames_per_unit;
} player_buffer;

double getADSRMultiplier(float time01)
{
    #define ATTACK .05f
    #define DECAY .1f
    #define SUSTAIN .6f
    #define RELEASE 1.f

    double a,b;
    if(time01 <= ATTACK)
        a = 0, b = 1, time01 /= ATTACK;
    else if (time01 <= DECAY)
        a = 1, b = .7, time01 = (time01-ATTACK) / (DECAY - ATTACK);
    else if (time01 <= SUSTAIN)
        a = .7, b = .6, time01 = (time01-DECAY) / (SUSTAIN - DECAY);
    else if (time01 <= RELEASE)
        a = .6, b = 0, time01 = (time01-SUSTAIN) / (RELEASE - SUSTAIN);

    double res = b * time01 + a * (1. - time01);
    return res;
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int paSampleCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    player_buffer *buffer = (player_buffer*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        double result = 0;

        FOREACH_X(buffer->notes, player_note)
        {
            if(x->time <= x->duration)
            {
                static const double deltatime = 1. / (double)SAMPLE_RATE;

                for(int o = 0; o < overtonesCount; o++)
                {
                    double deltaspace = (double)TABLE_SIZE * (double)x->frequency * overtones[o] * deltatime;
                    double sample = buffer->sine[(long)(x->time * deltaspace) % TABLE_SIZE];
                    result += sample * getADSRMultiplier((float)x->time/(float)x->duration);
                }
                //printf("%f\n", result);
                x->time ++;
            }
        }

        *out++ = result; //left   
        *out++ = result; //right      
    }

    return paContinue;
}

/*
 * This routine is called by portaudio when playback is done.
 */
static void StreamFinished( void* userData )
{
   player_buffer *buffer = (player_buffer *) userData;
   printf( "Piece Completed:\n");

   ll_free(buffer->notes);
   FREE(buffer);
}

/*
* Adds the given notes to the buffer
*/
void fillBuffer(player_buffer * buffer, note * notes, int num)
{
    for(int i = 0; i < num; i++)
    {
        note n = notes[i];
        printf("Playing %d-%c%d-%2fHz\n", (int)TONE(n), NAME(n), NOTE_OCTAVE(n), get_note_frequency(n));
        //printf("count: %d\n", ll_count(buffer->notes));
        player_note * newNote = NEW(player_note);
        newNote->frequency = get_note_frequency(n);
        newNote->time = 0;
        newNote->duration = buffer->frames_per_unit * (1 << DURATION(n)); //TODO PICK DURATION
        ll_cons(buffer->notes, newNote);
    }
}

/*
* Removes finished notes and adds new notes
*/
void updateBuffer(player_buffer * buffer, piece * p, unsigned long time)
{
    REMOVE_ALL_X(buffer->notes, player_note, (x->time >= x->duration))

    note * notes = p->units[time].accompaniement;
    fillBuffer(buffer, notes, p->units[time].accompaniementCount);

    notes = p->units[time].melody;
    fillBuffer(buffer, notes, p->units[time].melodyCount);
}

/*******************************************************************/
int play(piece * p)
{
    player_buffer * buffer = NEW(player_buffer);
    buffer->notes = ll_new();
    buffer->frames_per_unit = SAMPLE_RATE / p->unitsPerSecond;
    for(int i=0; i<TABLE_SIZE; i++ )
        buffer->sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );

#pragma region portaudio init
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      goto error;
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paSampleCallback,
              buffer );
    if( err != paNoError ) goto error;
    err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
    if( err != paNoError ) goto error;
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;
#pragma endregion

    //printf("Play for %d seconds.\n", NUM_SECONDS );
    for(int i = 0; i < p->unitsCount; i++)
    {
        Pa_Sleep( 1.f / (float)p->unitsPerSecond * 1000.f );
        updateBuffer(buffer, p, i);
    }
    Pa_Sleep( 1.f / (float)p->unitsPerSecond * 1000.f );

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();
    printf("Test finished.\n");
    
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}