buildandrun: bothoven.c piece.h utils.h
	gcc player.c ll.c libportaudio.a -lrt -lm -lasound -pthread && ./a.out