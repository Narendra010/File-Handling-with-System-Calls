#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

// Function to Create File
void createFile(char* filename, mode_t permissions) {
    int fd = creat(filename, permissions);
    if (fd == -1) {
        printf("Error creating file");
        exit(1);
    }
    close(fd);
    printf("File created successfully.\n");
}

// Function to read file
void readFile(char* filename, size_t size, off_t offset, int whence) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("File '%s' does not exist. Do you want to create it? (y/n): ", filename);
        char choice;
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            mode_t permissions;
            printf("Enter the file permissions (in octal): ");
            scanf("%o", &permissions);
            createFile(filename, permissions);
        } else {
            printf("Exiting...\n");
            exit(0);
        }
        fd = open(filename, O_RDONLY);
    }

    char* buffer = (char*)malloc(size+1); // dynamically allocating memory for buffer in runtime
    if (lseek(fd, offset, whence) == -1) {
            printf("Error seeking file");
            free(buffer);
            close(fd);
        }
    ssize_t bytesRead = read(fd, buffer, size);
    if (bytesRead == -1) {
        printf("Error reading file");
        free(buffer);
        close(fd);
        exit(1);
    }
    buffer[bytesRead] = '\0'; // adds null character at the end of buffer array

    printf("Data read from file: %s\n", buffer);
    free(buffer);
    close(fd);
}

// Function to write data to file
void writeFile(char* filename, char* data, size_t size, off_t offset, int whence) {
    int fd = open(filename, O_WRONLY);
    if (fd == -1) {
        printf("File '%s' does not exist. Do you want to create it? (y/n): ", filename);
        char choice;
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            mode_t permissions;
            printf("Enter the file permissions (in octal): ");
            scanf("%o", &permissions);
            createFile(filename, permissions);
        } else {
            printf("Exiting...\n");
            exit(0);
        }
        fd = open(filename, O_WRONLY);
    }
    if (lseek(fd, offset, whence) == -1) {
            printf("Error seeking file");
            close(fd);
        }
    ssize_t bytesWritten = write(fd, data, size);
    if (bytesWritten == -1) {
        printf("Error writing to file");
        close(fd);
        exit(1);
    }
    printf("Data written to file successfully.\n");
    close(fd);
}

// Function to display file stats
void fileStats(char* filename) {
    struct stat fileInfo;
    if (stat(filename, &fileInfo) == -1) {
        printf("Error getting file information");
        exit(0);
    }
    printf("File Information:\n");
    printf("  Owner: %d\n", fileInfo.st_uid);
    printf("  Permissions: %o\n", fileInfo.st_mode & 0777);
    printf("  Inode: %ld\n", fileInfo.st_ino);
    printf("  Access Time: %s", ctime(&fileInfo.st_atime));
    printf("  Modification Time: %s", ctime(&fileInfo.st_mtime));
    printf("  Change Time: %s", ctime(&fileInfo.st_ctime));
}

// Function to create named pipe
void createNamedPipe(char* pipeName, char* purpose) {
    mode_t mode = 0777;
    int result,pfd;

    // For writing into the pipe

    if(strcmp(purpose,"write")==0){
        if (mkfifo(pipeName, mode) == -1) {
            printf("Error creating named pipe");
            exit(1);
        }
        char data[2056];
        printf("Enter your data: ");
        scanf(" %[^\n]s",data);
        data[2055] = '\0';        
        int size = strlen(data);
        pfd = open(pipeName, O_WRONLY);
        if (pfd == -1) {            
            printf("open error");
            exit(1);
        } 
        int s = write(pfd,data,size);
        if(s<=0){            
            printf("write  error");
            exit(1);
        }
        printf("Data written in pipe %s successfully \n", pipeName);              
        close(pfd);        
    }

    //For reading from pipe

    else if(strcmp(purpose,"read")==0) {
        pfd = open(pipeName, O_RDONLY); 
        if (pfd == -1) {         
            perror("open");
            exit(1);
        }
        char buff[2056];
        int s = read(pfd,buff,2055);
        buff[2055] = '\0';
        if(s<=0){
            printf("Erorr in reading from the pipe!");
            exit(1);
        }
        printf("Received message: %s\n", buff);             
        close(pfd);
    }
    else{
        printf("Check the parameters\n");
        close(pfd);
    }

}

// Function to copy data using pipe
void copyFileUsingPipe(char* source, char* dest) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error creating pipe");
        exit(1);
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error forking process");
        exit(1);
    }
    
    if (pid == 0) {  // Child process (reads from pipe and writes to destination file)
        close(pipefd[1]);  // Close unused write end of the pipe
        
        int destFd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (destFd == -1) {
            perror("Error opening destination file");
            exit(1);
        }
        
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            ssize_t bytesWritten = write(destFd, buffer, bytesRead);
            if (bytesWritten == -1) {
                perror("Error writing to destination file");
                exit(1);
            }
        }
        
        close(pipefd[0]); // Close read end of pipe
        close(destFd);
        exit(0);
    } else {  // Parent process (reads from source file and writes to pipe)
        close(pipefd[0]);  // Close unused read end of the pipe
        
        int sourceFd = open(source, O_RDONLY);
        if (sourceFd == -1) {
            perror("Error opening source file");
            exit(1);
        }
        
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(sourceFd, buffer, sizeof(buffer))) > 0) {
            ssize_t bytesWritten = write(pipefd[1], buffer, bytesRead);
            if (bytesWritten == -1) {
                perror("Error writing to pipe");
                exit(1);
            }
        }
        
        close(pipefd[1]); // Close write end of the pipe
        close(sourceFd);
    }
}