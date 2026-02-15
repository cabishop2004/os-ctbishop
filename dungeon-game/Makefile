CFLAGS = -Wall -Wextra -g -std=c11

all : dungeon-game

run : all
	nice -n 10 ./dungeon-game --steps=6 --prob-trap=0.30 --prob-treasure=0.35 --prob-monster=0.35 --prob-min=1e-5

dungeon-game : dungeon-game.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean :
	rm -f dungeon-game

.PHONY : all run clean
