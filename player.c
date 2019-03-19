#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "piece.h"
#include "utils.h"
#include "ll.h"

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (128)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (4096)

typedef struct pn
{
    double frequency;
    unsigned long time;
    unsigned long duration;
} player_note;

typedef struct 
{
    list * notes;
    unsigned long frames_per_unit;
} player_buffer;

typedef struct
{
    float sine[TABLE_SIZE];
    player_buffer * buffer;
}
paTestData;

double getADSRMultiplier(float time01)
{
    #define ATTACK .1f
    #define DECAY .3f
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
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        double result = 0;

        FOREACH_X(data->buffer->notes, player_note)
        {
            if(x->time <= x->duration)
            {
                static const double deltatime = 1. / (double)SAMPLE_RATE;
                double deltaspace = (double)TABLE_SIZE * (double)x->frequency * deltatime;
                double sample = data->sine[(long)(x->time * deltaspace) % TABLE_SIZE];
                result += sample * getADSRMultiplier((float)x->time/(float)x->duration);

                //double delta = (double)(FRAMES_PER_BUFFER * FRAMES_PER_BUFFER * x->frequency) / (double)SAMPLE_RATE;
                //printf("%f\n", result);
                x->time ++;
            }
        }

        *out++ = result; //left   
        *out++ = result; //right      
    }

    // for( i=0; i<framesPerBuffer; i++ )
    // {
    //     *out++ = data->sine[data->sampleIndex];  /* left */
    //     *out++ = data->sine[data->sampleIndex];  /* right */
    //     data->sampleIndex += 5;
    //     if( data->sampleIndex >= TABLE_SIZE ) data->sampleIndex -= TABLE_SIZE;
    // }

    return paContinue;
}

/*
 * This routine is called by portaudio when playback is done.
 */
static void StreamFinished( void* userData )
{
   paTestData *data = (paTestData *) userData;
   printf( "Piece Completed:\n");
}

/*
* Adds the given notes to the buffer
*/
void fillBuffer(player_buffer * pb, note * notes, int num)
{
    for(int i = 0; i < num; i++)
    {
        note n = notes[i];
        printf("Playing %d-%c%d-%2fHz\n", (int)TONE(n), NAME(n), NOTE_OCTAVE(n), get_note_frequency(n));

        player_note * newNote = NEW(player_note);
        newNote->frequency = get_note_frequency(n);
        newNote->time = 0;
        newNote->duration = pb->frames_per_unit * (1 << DURATION(n)); //TODO PICK DURATION
        ll_cons(pb->notes, newNote);
    }
}

/*
* Removes finished notes and add new notes
*/
void updateBuffer(player_buffer * pb, piece * p, unsigned long time)
{
    REMOVE_ALL_X(pb->notes, player_note, (x->time >= x->duration))

    note * notes = p->units[time].accompaniement;
    fillBuffer(pb, notes, p->units[time].accompaniementCount);

    notes = p->units[time].melody;
    fillBuffer(pb, notes, p->units[time].melodyCount);
}

/*******************************************************************/
int main(void);
int main(void)
{
    srand(12);
    paTestData data;
    for(int i=0; i<TABLE_SIZE; i++ )
    {
        data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
    piece * p = get_musical_scale(6);
   // piece * p = get_random_piece(7.f, 1, 1, 2, .2f);
   // piece * p = get_random_piece(2.3f, 3, 1, 2, .2f);
    print_piano_roll(p);
    data.buffer = NEW(player_buffer);
    data.buffer->notes = ll_new();
    data.buffer->frames_per_unit = SAMPLE_RATE / p->unitsPerSecond;

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
              &data );
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
        updateBuffer(data.buffer, p, i);
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