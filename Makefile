.PHONY: clean

clean:
	fusermount -u ./mnt

wiki-browser: wikipedia.c wikipedia.h main.c
	@python3 meta.py
	clang `pkg-config fuse3 --cflags --libs` -o wiki-browser wikipedia.c main.c
	@echo "now run 'sudo ./wiki-browser ./mnt' to mount the filesystem"

format:
	clang-format -i *.c *.h
