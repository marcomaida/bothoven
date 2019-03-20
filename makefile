buildandrun: bothoven.c piece.h utils.h player.h player.c 
	gcc bothoven.c ll.c player.c piece.c libportaudio.a -lrt -lm -lasound -pthread && ./a.out