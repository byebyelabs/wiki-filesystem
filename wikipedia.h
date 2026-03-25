#include <asm/unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

char *get_root();

char *get_dirs(const char *curr_dir);

char *get_content(const char *curr_dir);
