// C Wrapper for the Python Wrapper that wraps Wikipedia API

#include "wikipedia.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char script_path[PATH_MAX];
static char output_path[PATH_MAX];

/* dir + name without "//" when dir is "/" or ends with '/'. */
static void join_dir_file(char *out, size_t outsz, const char *dir,
                          const char *name) {
  const char *sep =
      (dir[0] != '\0' && dir[strlen(dir) - 1] != '/') ? "/" : "";
  snprintf(out, outsz, "%s%s%s", dir, sep, name);
}

static void wiki_paths_ensure(void) {
  static int ready;
  if (ready) {
    return;
  }
  ready = 1;

  char dir[PATH_MAX];
  const char *env = getenv("WIKI_FS_HOME");
  if (env != NULL && env[0] != '\0') {
    if (realpath(env, dir) == NULL) {
      strncpy(dir, env, sizeof(dir) - 1);
      dir[sizeof(dir) - 1] = '\0';
    }
  } else if (getcwd(dir, sizeof(dir)) == NULL) {
    perror("wiki_paths_ensure: getcwd");
    exit(1);
  }

  join_dir_file(script_path, sizeof(script_path), dir, "wikipedia.py");
  join_dir_file(output_path, sizeof(output_path), dir, ".local.output.txt");
}

// Returns the contents of the output file
// Must be freed by caller
char *read_output_file() {
  wiki_paths_ensure();
  FILE *op = fopen(output_path, "r+");
  if (op == NULL) {
    // file does not exist (request was unsuccessful)
    return NULL;
  }

  int content_sz = 0;
  do {
    content_sz += 1;
  } while (fgetc(op) != EOF);

  char *content = malloc(content_sz);
  fseek(op, 0, SEEK_SET);

  int index = 0;
  do {
    content[index++] = fgetc(op);
  } while (index < content_sz);

  content[index] = '\0';
  return content;
}

char *get_root() {
  wiki_paths_ensure();
  char *args[] = {"python3", script_path, "-ft", NULL};

  if (execvp("python3", args)) {
    perror("execvp failed: get_root");
    exit(2);
  }

  return read_output_file();
}

char *get_dirs(char *curr_dir) {
  wiki_paths_ensure();
  char *args[] = {"python3", script_path, "-gl", curr_dir, NULL};

  if (execvp("python3", args)) {
    perror("execvp failed: get_dirs");
    exit(2);
  }

  return read_output_file();
}

char *get_content(char *curr_dir) {
  wiki_paths_ensure();
  char *args[] = {"python3", script_path, "-gs", curr_dir, NULL};

  if (execvp("python3", args)) {
    perror("execvp failed: get_content");
    exit(2);
  }

  return read_output_file();
}
