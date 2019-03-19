#include "utils.h"

#include <unistd.h> 
/**
* The representation of a piece is essentially
* a piano roll.
* A piece is just an array of units
*/

#define OCTAVES 4
#define TOTAL_TONES (OCTAVES * 12)
#define START_OCTAVE 3


#define TONE(note) ((note) & 63)
#define DURATION(note) ((note) & 3<<6)
#define NAME(note) "CcDdEFfGgAaB"[TONE(note)%12]
#define NOTE_OCTAVE(note) (TONE(note)/12+START_OCTAVE)

/**
* A note uses the first 2 bits to store note duration
* and the other 6 bits to store the tone.
* 64 tones can be rapresented, good enough for 4 octaves 
*/
typedef unsigned char note;

/*
* A time unit of the piece.
* Represents an instant of time
*/
typedef struct
{
	note * accompaniement;
	unsigned int accompaniementCount;
	note * melody;
	unsigned int melodyCount;
} unit;

/*
* A time unit of the piece.
* Represents an instant of time
*/
typedef struct
{
	unit * units;
	unsigned int unitsCount;
	unsigned int unitsPerSecond;
} piece;

note get_random_note()
{
	return rand() % TOTAL_TONES + ((rand()%4) << 6);
}

float get_note_frequency(note note)
{
	//https://en.wikipedia.org/wiki/Piano_key_frequencies
	return 440.f * pow(2,(TONE(note) +(START_OCTAVE-1)*12-49+4)/ 12.f);
}

piece * get_random_piece(unsigned int duration, unsigned int unitsPerSecond, unsigned int minNotesPerUnit, unsigned int maxNotesPerUnit, float melodyToAccNotesRatio01)
{
	piece * p = NEW(piece);
	p->unitsCount = duration * unitsPerSecond;
	p->unitsPerSecond = unitsPerSecond;
	p->units = NEW_ARRAY(unit,p->unitsCount);

	for(unsigned int i = 0; i < p->unitsCount; i++)
	{//building every unit
		unit * u = &(p->units[i]);
		unsigned int notesCount = rand() % (maxNotesPerUnit - minNotesPerUnit) + minNotesPerUnit;
		
		u->melodyCount = notesCount * melodyToAccNotesRatio01;
		u->melody = NEW_ARRAY(note, u->melodyCount);
		for(unsigned int ii = 0; ii < u->melodyCount; ii++)
		{//building every melody note of a unit
			u->melody[ii] = get_random_note();
		}

		u->accompaniementCount = notesCount - u->melodyCount;
		u->accompaniement = NEW_ARRAY(note, u->accompaniementCount);
		for(unsigned int ii = 0; ii < u->accompaniementCount; ii++)
		{//building every accompaniement note of a unit
			u->accompaniement[ii] = get_random_note();
		}
	}

	return p;
}

piece * get_musical_scale(unsigned int unitsPerSecond)
{
	piece * p = NEW(piece);
	p->unitsCount = 12 * OCTAVES;
	p->unitsPerSecond = unitsPerSecond;
	p->units = NEW_ARRAY(unit,p->unitsCount);

	for(unsigned int i = 0; i < p->unitsCount; i++)
	{//building every unit
		unit * u = &(p->units[i]);
		
		u->melodyCount = 1;
		u->melody = NEW(note);
		*u->melody = i;

		u->accompaniementCount = 0;
	}

	return p;
}

void print_piano_roll(piece * p)
{
	char notes [TOTAL_TONES +1];
	notes[TOTAL_TONES] = '\0';

	for(unsigned int i = 0; i < p->unitsCount; i++)
	{
		memset(notes, ' ', (TOTAL_TONES));

		unit * u = &(p->units[i]);
		for(unsigned int ii = 0; ii < u->melodyCount; ii++)
		{
			notes[TONE(u->melody[ii])] = 'M';
		}

		for(unsigned int ii = 0; ii < u->accompaniementCount; ii++)
		{//building every accompaniement note of a unit
			notes[TONE(u->accompaniement[ii])] = 'A';
		}

		printf("|%s|\n", notes);
	}
}

/*
* Incredibly stupid implementation of a music player that
* uses aplay
*/
void play_piece(piece * p)
{
	if(!fork())
	{
		float timeToWait = 1000.f / (float)p->unitsPerSecond;

		/*
		for(unsigned int i = 0; i < p->unitsCount; i++)
		{
			unit * u = &(p->units[i]);
			for(unsigned int ii = 0; ii < u->melodyCount; ii++)
			{
				if(!fork())
				{
					char out[50];
					snprintf(out, 50, "aplay piano/%d.wav", u->melody[ii]);
     				printf("%s\n", out);
     				system(out);
     				exit(0);
				}
			}

			for(unsigned int ii = 0; ii < u->accompaniementCount; ii++)
			{//building every accompaniement note of a unit
				if(!fork())
				{
					char out[50];
					snprintf(out, 50, "aplay piano/%d.wav", u->accompaniement[ii]);
     				printf("%s\n", out);
     				system(out);
     				exit(0);
				}
			}

			usleep(timeToWait * 1000);
		}
		*/

		exit(0);
	}
}