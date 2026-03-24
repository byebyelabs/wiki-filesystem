// C Wrapper for the Python Wrapper that wraps Wikipedia API

#include "wikipedia.h"

// Output is guaranteed to be written at this location after Python call
static char* OUTPUT_FILE = ".local.output.txt";


// Returns the contents of the output file
// Must be freed by caller
char* read_output_file() {
    FILE* op = fopen(OUTPUT_FILE, "r+");
    if (op == NULL) { 
        // file does not exist (request was unsuccessful)    
        return NULL;
    }

    int content_sz = 0;
    do {
        content_sz += 1;
    } while (fgetc(op) != EOF);

    char* content = malloc(content_sz);
    fseek(op, 0, SEEK_SET);
    
    int index = 0;
    do {
        content[index++] = fgetc(op);
    } while (index < content_sz);

    content[index] = '\0';
    return content;
}

char* get_root() {
    // root is always the featured title (ft) from Wikipedia
    char *args[] = { "python3", "wikipedia.py", "-ft", NULL};
    
    if(execvp("python3", args)) {
        perror("execvp failed: get_root");
        exit(2);
    }
    
    return read_output_file();
}

char* get_dirs(char* curr_dir) {
    // get links contained in the page for this dir name
    char *args[] = { "python3", "wikipedia.py", "-gl", curr_dir, NULL };
    
    if(execvp("python3", args)) {
        perror("execvp failed: get_dirs");
        exit(2);
    }
    
    return read_output_file();
}

char* get_content(char* curr_dir) {
    // get wikipedia summary of the page for this dir name
    char *args[] = { "python3", "wikipedia.py", "-gs", curr_dir, NULL};
    
    if(execvp("python3", args)) {
        perror("execvp failed: get_content");
        exit(2);
    }
    
    return read_output_file();
}
