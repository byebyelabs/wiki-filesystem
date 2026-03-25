// C Wrapper for the Python Wrapper that wraps Wikipedia API

#include "wikipedia.h"
#include "wikilocation.c"
// ^ this file is generated after running meta.py ^

void run_python_script(char **args) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork failed");
    exit(2);
  }
  if (pid == 0) {
    if (execvp("python3", args)) {
      perror("execvp failed: get_root");
      exit(2);
    }
  } else {
    // parent
    pid_t ret = waitpid(pid, NULL, 0);
    if (ret == -1) {
      perror("wait pid failed");
      exit(2);
    }
  }
}

// Returns the contents of the output file
// Must be freed by caller
char *read_output_file() {
  FILE *op = fopen(OUTPUT_FILE, "r");
  if (op == NULL) {
    // file does not exist (request was unsuccessful)
    return NULL;
  }

  // this means the file is empty
  if (fseek(op, 0, SEEK_END) != 0) {
    fclose(op);
    return NULL;
  }

  // get content size (file size)
  long content_sz = ftell(op);

  if (content_sz < 0) {
    fclose(op);
    return NULL;
  }

  // reset the file pointer to beginning
  if (fseek(op, 0, SEEK_SET) != 0) {
    fclose(op);
    return NULL;
  }

  // allocate memory for content
  char *content = malloc((size_t)content_sz + 1);
  if (content == NULL) {
    fclose(op);
    return NULL;
  }

  // read and save the content
  size_t read_sz = fread(content, 1, (size_t)content_sz, op);
  fclose(op);
  content[read_sz] = '\0';

  return content;
}

char *get_root() {
  // root is always the featured title (ft) from Wikipedia
  char *args[] = {"python3", PYTHON_SCRIPT_PATH, "-ft", OUTPUT_FILE, NULL};

  run_python_script(args);

  return read_output_file();
}

char *get_dirs(const char *curr_dir) {
  // get links contained in the page for this dir name
  char *args[] = {"python3",        PYTHON_SCRIPT_PATH, "-gl",
                  (char *)curr_dir, OUTPUT_FILE,        NULL};

  run_python_script(args);

  return read_output_file();
}

char *get_content(const char *curr_dir) {
  // get wikipedia summary of the page for this dir name
  char *args[] = {"python3",        PYTHON_SCRIPT_PATH, "-gs",
                  (char *)curr_dir, OUTPUT_FILE,        NULL};

  run_python_script(args);

  return read_output_file();
}
