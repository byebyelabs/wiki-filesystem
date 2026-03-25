.PHONY: clean mount

all: wiki-browser

setup:
	mkdir -p mnt
	python3 -m venv venv
	source venv/bin/activate && pip install -r requirements.txt

clean:
	fusermount -u ./mnt

mount:
	source venv/bin/activate && ./wiki-browser -d -s ./mnt

wiki-browser: wikipedia.c wikipedia.h main.c
	python3 meta.py
	clang `pkg-config fuse3 --cflags --libs` -o wiki-browser wikipedia.c main.c

format:
	clang-format -i *.c *.h
