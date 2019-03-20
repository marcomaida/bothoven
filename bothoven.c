#include "utils.h"
#include "piece.h"
#include "player.h"
#include <time.h>

int main()
{
	srand((unsigned int)time(0));
    //piece * p = get_random_piece(10.f, 5, 1, 3, .2f);
    piece * p = get_musical_scale(2);
	if(!fork())
	{
		play(p);
     	exit(0);
	}

	print_piano_roll(p);
}