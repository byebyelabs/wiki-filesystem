.PHONY: clean

clean:
	fusermount -u ./mnt

wiki-browser: wikipedia.c wikipedia.h main.c
	clang `pkg-config fuse3 --cflags --libs` -o wiki-browser wikipedia.c main.c

format:
	clang-format -i *.c *.h
