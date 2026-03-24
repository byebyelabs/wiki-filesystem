#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>

char* get_root();

char** get_dirs(char* );

char* get_content();
