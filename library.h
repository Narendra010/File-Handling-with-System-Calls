void createFile(char* filename, mode_t permissions);
void readFile(char* filename, size_t size, off_t offset, int whence);
void writeFile(char* filename, char* data, size_t size, off_t offset, int whence);
void copyFileUsingPipe(char* source, char* dest);
void fileStats(char* filename);
void createNamedPipe(char* pipeName, char* purpose);