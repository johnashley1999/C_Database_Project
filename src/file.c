#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"

int Create_Db_File(char *file_name) {

    // Attempt File Open Read Error
    // If File opens print error as it should not exists
    int fd = open(file_name, O_RDWR);
    if (fd != -1) {
        close(fd);
        printf("File Already exists\n");
        return STATUS_ERROR;
    }

    // Attempt create file
    // print error if open operation fails 
    fd = open(file_name, O_RDWR | O_CREAT, 0644);
    if (fd == -1){
        perror("open");
        return STATUS_ERROR;
    }

    return fd;

}

int Open_Db_File(char *file_name) {

    // Open previous file
    // print error if the file fails to open
    int fd = open(file_name, O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;

}