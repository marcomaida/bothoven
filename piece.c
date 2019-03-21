#include "piece.h"

note get_random_note()
{
	return rand() % TOTAL_TONES + ((rand()%4) << 6);
}

float get_note_frequency(note note)
{
	//https://en.wikipedia.org/wiki/Piano_key_frequencies
	return 440.f * pow(2,(TONE(note) +(START_OCTAVE-1)*12-49+4)/ 12.f);
}

void make_random_piece(piece * p, unsigned int duration, unsigned int unitsPerSecond, unsigned int minNotesPerUnit, unsigned int maxNotesPerUnit, float melodyToAccNotesRatio01)
{
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

void piece_free(piece * p)
{
	unit * u = p->units;
	for(unsigned int i = 0; i < p->unitsCount; i++)
	{
		unit_free(u);
		u++;
	}

	FREE(piece, p);
}

void unit_free(unit * u)
{
	FREE_ARRAY(unit, u->melody, u->melodyCount);
	FREE_ARRAY(unit, u->accompaniement, u->accompaniementCount);
}