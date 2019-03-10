#include "utils.h"
#include "piece.h"
#include <time.h>

int main()
{
	srand((unsigned int)time(0));
	piece * p = get_random_piece(5, 8, 3, 8, .2f);
	if(fork())
	{
     	system("aplay note.wav");
     	exit(0);
	}

	print_piano_roll(p);
}