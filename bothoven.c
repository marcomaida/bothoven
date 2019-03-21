#include "utils.h"
#include "piece.h"
#include "player.h"
#include <time.h>

void make_genetic_island(int population, int duration);

typedef struct 
{
	piece * individuals;
	int population;
} genetic_island;

int main()
{
	INIT_MEMORY_CHECK
	srand((unsigned int)time(0));
	make_genetic_island(100, 10);
	CHECK_MEMORY
    //piece * p = get_random_piece(10.f, 5, 1, 3, .2f);
    //piece * p = get_musical_scale(2);
	/*
	if(!fork())
	{
		play(p);
     	exit(0);
	}
	print_piano_roll(p);
	*/
}

void make_genetic_island(int population, int duration)
{
	genetic_island * island = NEW(genetic_island);
	island->individuals = NEW_ARRAY(piece, population);

	piece * p = island->individuals;
	for(int i = 0; i < population; i++)
	{
		make_random_piece(p, duration,5, 1, 3, .5f);
		//print_piano_roll(p);
		p++;
	}




	p = island->individuals;
	for(int i = 0; i < population; i++)
	{
		piece_free(p);
		p++;
	}
}