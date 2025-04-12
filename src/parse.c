#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void List_Employees(struct db_header_t *header, struct employee_t *employees) {

    for(int i = 0; i < header->count; i++){
        printf("Employee %d\n", i);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %d\n\n", employees[i].hours);
    }
}

int Remove_Employee_By_Name(struct db_header_t *header, struct employee_t *employees, struct employee_t *temp_employees, char *remove_name_str) {
    /*
    This function populates the temp_employee pointer

    note: The header and employee pointers should not be modified during this function
    */

    // Ensure valid inputs 
    if (header == NULL || employees == NULL || temp_employees == NULL || remove_name_str == NULL) {
        printf("Remove_Employee_By_Name: Invalid arguments\n");
        return STATUS_ERROR;
    }

    printf("Removing Name: %s\n", remove_name_str);

    /*****************************************************************************
    Populate Temp Employees
    *****************************************************************************/
    //Populate temp_employees until employee name is found
    int rem_emp_ind = -1;
    for(int i = 0; i < header->count; i++){
        
        if (strcmp(employees[i].name, remove_name_str) == 0) {
            rem_emp_ind = i;
            break;
        } else {
            temp_employees[i] = employees[i];

            printf("Temp Employee %d name: %s\n", i, temp_employees->name);

        }
    }

    // Return error if employee name not found
    if (rem_emp_ind == -1) {
        printf("Remove_Employee_By_Name: Name %s not found\n", remove_name_str);
        return STATUS_ERROR;
    } else {
        printf("Employee Index Removed: %d\n", rem_emp_ind);
    }

    printf("deleted employee index: %d\n", rem_emp_ind);

    
    printf("Employee %d name: %s\n", 1, employees[1].name);

    // Populate remaining employees after match
    for (int k = rem_emp_ind; k < header->count -1; ++k){
        temp_employees[k] = employees[k+1];

        printf("Temp Employee %d name: %s\n", k, temp_employees[k].name);
    }

    printf("Successfully removed %s\n", remove_name_str);

    return STATUS_SUCCESS;
}

int Add_Employees(struct db_header_t *header, struct employee_t *employees, char *addstring){
    printf("Add employee: %s\n", addstring);

    char *name = strtok(addstring, ",");

    char *addr = strtok(NULL, ",");

    char *hours = strtok(NULL, ",");

    strncpy(employees[header->count-1].name, name, sizeof(employees[header->count-1].name)); 
    strncpy(employees[header->count-1].address, addr, sizeof(employees[header->count-1].address)); 

    employees[header->count-1].hours = atoi(hours);

    return STATUS_SUCCESS;
}

int Update_Employee(struct db_header_t *header, struct employee_t *employees, char *update_hours_str) {

    printf("Update_Employee: Update Employee String %s", update_hours_str);

    char *name = strtok(update_hours_str, ",");
    char *addr = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");
    
    for(int i=0; i<header->count; ++i) {

        if (strcmp(employees[i].name, name)) {

            strncpy(employees[i].address, addr, sizeof(employees[i].address)); 
            employees[i].hours = atoi(hours);
            
            printf("Update_Employee: Successfully updated employee %s to %s", name, update_hours_str);

            return STATUS_SUCCESS;
        }
    }
    
    printf("Update_Employee: Employee %s not found", name);

    return STATUS_ERROR;
}

int Read_Employees(int fd, struct db_header_t *header, struct employee_t **employeesOut) {

    if (fd < 0) {
        printf("Read_Employees: Invalid fd received\n");
        return STATUS_ERROR;
    }

    int count = header->count;

    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL) {
        printf("Read_Employees: Calloc Failed\n");
        return STATUS_ERROR;
    }

    read(fd, employees, count*sizeof(struct employee_t));

    for(int i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;

}

int Create_Db_Header(int fd, struct db_header_t **headerOut) {

    if (fd < 0){
        printf("Create_Db_Header: Invalid fd received\n");
        return STATUS_ERROR;
    }

    struct db_header_t * new_header = calloc(1, sizeof(struct db_header_t));
    if (new_header == NULL) {
        printf("Create_Db_Header: Calloc failed to create db header \n");
        return STATUS_ERROR;
    }

    *new_header = new_db_header_default;

    *headerOut = new_header;

    return STATUS_SUCCESS;
}

int Validate_Db_Header(int fd, struct db_header_t **headerOut) {
    // Ensure valid input for fd is received
    if (fd < 0) {
        printf("Validate_Db_Header:invalid fd\n");
        return STATUS_ERROR;
    }

    if (headerOut == NULL){
        printf("Validate_Db_Header: Valid Header Pointer is invalid\n");
    }

    // Allocate memory for the 
    struct db_header_t *prev_header = calloc(1, sizeof(struct db_header_t));
    if (prev_header == NULL){
        printf("Validate_Db_Header: Calloc failed to create db header\n");
        return STATUS_ERROR;
    }

    //Unpack the Database Header
    if (read(fd, prev_header, sizeof(struct db_header_t)) != sizeof(struct db_header_t)) {
        perror("read");
        free(prev_header);
        return STATUS_ERROR;
    }

    prev_header-> version = ntohs(prev_header->version);
    prev_header-> count = ntohs(prev_header->count);
    prev_header-> magic = ntohl(prev_header->magic);
    prev_header-> filesize = ntohl(prev_header->filesize);

    if (prev_header->version != new_db_header_default.version){
        printf("Validate_Db_Header: Improper header version: %d\n", prev_header->version);
        free(prev_header);
        return STATUS_ERROR;
    }

    if (prev_header->magic != HEADER_MAGIC){
        printf("Validate_Db_Header: Improper header magic\n");
        free(prev_header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (prev_header->filesize != dbstat.st_size){
        printf("Validate_Db_Header: Improper header filesize\n");
        free(prev_header);
        return STATUS_ERROR;
    }

    *headerOut = prev_header;

    return STATUS_SUCCESS;

}

int Output_File(int fd, struct db_header_t *headerOut, struct employee_t *employees) {
    // Ensure Inputs are as expected
    if (fd < 0){
        printf("Output_File: Got a bad FD from the user\n");
        return STATUS_ERROR;
    }
    if (headerOut == NULL) {
        printf("Output_File: Got a NULL header\n");
        return STATUS_ERROR;
    }

    int real_count = headerOut->count;
    printf("Output_File: Real count: %d\n", real_count);

    headerOut->magic = htonl(headerOut->magic);
    headerOut->filesize = htonl(sizeof(struct db_header_t) + real_count*sizeof(struct employee_t));
    headerOut->count = htons(headerOut->count);
    headerOut->version = htons(headerOut->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, headerOut, sizeof(struct db_header_t));
    
    for (int i = 0; i < real_count; i++) {
        printf("Employee %d Name: %s\n", i, employees[i].name);
        printf("Employee %d Address: %s\n", i, employees[i].address);
        printf("Employee %d Hours: %d\n", i, employees[i].hours);
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));

    }

    if (ftruncate(fd, sizeof(struct db_header_t) + real_count*sizeof(struct employee_t)) == -1) {
        printf("Output_File: failed to truncate file\n");
    }

    printf("\n\n");

    return STATUS_SUCCESS;
}
