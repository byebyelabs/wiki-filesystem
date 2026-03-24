#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <sys/stat.h>
#include "wikipedia.h"

const char* content_filename = "00_content";

// initialize the filesystem
static void* wiki_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
	return NULL;
}

// get file attributes
static int wiki_getattr(
    char *path, struct stat *stbuf, struct fuse_file_info *_
) {
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        // root directory of the filesystem
        // is readable and executable by everyone, but writable
        // only by the owner
		stbuf->st_mode = S_IFDIR | 0755;

        // st_nlisk is set to 2 for directories, because of the
        // "." and ".." entries
		stbuf->st_nlink = 2;
        return 0;
	}

    size_t path_len = strlen(path);
    size_t suffix_len = strlen(content_filename) + 1; // + 1 for the prefixed slash
    if (path_len - suffix_len > 0 && strcmp(path + (path_len - suffix_len) + 1, content_filename) == 0){
        // read only
        stbuf->st_mode = S_IFREG | 0444;
        // get size
        const char* content = get_content(path);
        stbuf->st_size = strlen(content);
        free(content);
        return 0;
    }

    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
}

// read the contents of a directory
static int wiki_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
    struct fuse_file_info *fi, enum fuse_readdir_flags flags
) {
    bool is_root = strcmp(path, "/") == 0;
    const char* dirs = is_root ? get_root() : get_dirs(path);
    if (dirs == NULL) {
        return -ENOENT;
    }

    if (!is_root) {
        filler(buf, content_filename, NULL, 0, 0);
    }

    // go through each dir
    int len = strlen(dirs);
    char* dirs_ptr = dirs;
    while (dirs_ptr != NULL) {
        const char* dir = strsep(&dirs_ptr, "\n");
        filler(buf, dir, NULL, 0, 0);
    }

    free(dirs);

	return 0;
}

// open a file
static int wiki_open(const char *path, struct fuse_file_info *fi) {
    if ((fi->flags & O_ACCMODE) != O_RDONLY) {
        return -EACCES;
    }

    char* content = get_content(path);
    if (content == NULL) {
        return -ENOENT;
    }

    fi->fh = content;

	return 0;
}

// read data from a file
static int wiki_read(
    const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi
) {
    // assuming path ends with /content
    if (wiki_open(path, fi) != 0) {
        return -ENOENT;
    }

    const char* content = (char*) fi->fh;
    size_t len = strlen(content);
    if (offset < len) {
        if (offset + size > len) {
            size = len - offset;
        }
        memcpy(buf, content, size);
    } else {
        size = 0;
    }

    return size;
}

// define operations that the filesystem supports
static const struct fuse_operations wiki_oper = {
	.init       = wiki_init,
	.getattr	= wiki_getattr,
	.readdir	= wiki_readdir,
	.open		= wiki_open,
	.read		= wiki_read,
};

// Citation: Developed largely based on the hello.c example from libfuse:
//  https://github.com/libfuse/libfuse/blob/fuse-3.14.0/example/hello.c
int main(int argc, char *argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    
    int ret = fuse_main(args.argc, args.argv, &wiki_oper, NULL);;
	fuse_opt_free_args(&args);
	
    return ret;
}