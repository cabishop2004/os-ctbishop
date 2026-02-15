CFLAGS = -Wall -g -std=c11
hi : hi.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

fork-exec : fork-exec.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
clean :
	rm -f hi

.PHONY : clean


