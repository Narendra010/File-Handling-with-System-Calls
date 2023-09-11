#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "library.h"

int main(int argc, char* argv[]) {
    printf("Usage: %s <command> [arguments]\n", argv[0]);
    printf("Commands:\n");
    printf("  create <filename> <permissions>\n");
    printf("  read <filename> <size> <offset> <whence>\n");
    printf("      Whence : 0 -> to read from beginning\n");
    printf("               1 -> to read from current position\n");
    printf("               2 -> to read from end position (for this case, enter offset in negative.)\n");
    printf("  write <filename> <data> <offset> <whence>\n");
    printf("      Whence : 0 -> to write from beginning\n");
    printf("               1 -> to write from current position\n");
    printf("               2 -> to write from end position (for this case, enter offset in negative.)\n");
    printf("  file_info <filename>\n");
    printf("  create_named_pipe <pipe_name> <purpose>\n");
    printf("  copy_with_pipe <source file name> <destination file name>\n");

    if (strcmp(argv[1], "create") == 0 && argc == 4) {
        char* filename = argv[2];
        mode_t permissions = strtol(argv[3], NULL, 8);
        createFile(filename, permissions);
    }
    else if (strcmp(argv[1], "read") == 0 && argc == 6) {
        char* filename = argv[2];
        size_t size = atol(argv[3]);
        off_t offset = atol(argv[4]);
        int whence = atoi(argv[5]);
        readFile(filename, size, offset, whence);
    }
    else if (strcmp(argv[1], "write") == 0 && argc == 6) {
        char* filename = argv[2];
        char* data = argv[3];
        size_t size = strlen(data);
        off_t offset = atol(argv[4]);
        int whence = atoi(argv[5]);
        writeFile(filename, data, size, offset, whence);
    }
    else if (strcmp(argv[1],"file_info")==0 && argc == 3) {
        char* filename = argv[2];
        fileStats(filename);
    }
    else if(strcmp(argv[1],"create_named_pipe")==0 && argc == 4) {
        char* pipeName = argv[2];
        char* purpose = argv[3];
        createNamedPipe(pipeName, purpose);
    }
    else if(strcmp(argv[1], "copy_with_pipe")==0 && argc == 4) {

        char* source = argv[2];
        char* dest = argv[3];
        copyFileUsingPipe(source, dest);
    }
    else {
        printf("Invalid command or incorrect number of arguments.\n");
    }
    return 0;
}