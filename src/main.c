#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

// -f: filename to database file
// -n: create new database file
// -a: Add employee
// -r: Remove employee
// -u: update employee hours

void Print_Usage(char *argv[]){
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    return;
}

int main(int argc, char *argv[]){

    int c;
    bool new_file = false;
    char *file_path = NULL;
    char *addstring = NULL;
    bool list = false;
    char *remove_name_str = NULL;
    char *update_hours_str = NULL;

    int dbfd = -1;
    struct db_header_t *header = NULL;
    struct employee_t *employees = NULL;

    while((c = getopt(argc, argv, "nf:a:lr:u:")) != -1){
        switch(c){
            case 'n':
                new_file = true;
                break;
            
            case 'f':
                file_path = optarg;
                break;
            
            case 'a':
                addstring = optarg;
                break;
            
            case 'l':
                list = true;
                break;

            case 'r':
                remove_name_str = optarg;
                break;

            case 'u':
                update_hours_str = optarg;
                break;

            case '?':
                printf("main: Unknown option -%c\n", c);
                break;
            default:
                return -1;
        }
    }

    if (file_path == NULL){
        printf("main: Filepath is a required argument\n");
        Print_Usage(argv);
        return 0;
    }

    // -n
    if (new_file){

        dbfd = Create_Db_File(file_path);

        if (dbfd == STATUS_ERROR) {
            printf("main: Unable to create database file\n");
            return -1;
        }

        if (Create_Db_Header(dbfd, &header) == STATUS_ERROR) {
            printf("main: Failed to create database header");
            return -1;

        }

    // no -n
    } else {
        dbfd = Open_Db_File(file_path);
        if (dbfd == STATUS_ERROR) {
            printf("Main: Unable to open database file\n");
            return -1;
        }

        if (Validate_Db_Header(dbfd, &header) == STATUS_ERROR){
            printf("Main: Failed to Validate DB Header\n");
            return -1;
        }
    }

    // Always read employees
    if (Read_Employees(dbfd, header, &employees) != STATUS_SUCCESS) {
        printf("main: Failed to read employees\n");
        return -1;
    }

    //-a
    if (addstring) {

        ++header->count;
        employees = realloc(employees, header->count*sizeof(struct employee_t));
        
        if (Add_Employees(header, employees, addstring) != STATUS_SUCCESS){
            printf("main: Failed to Add Employees\n");
            return -1;
        }
    } 

    //-l
    if (list) {
        List_Employees(header, employees);
    }

    //-r
    if (remove_name_str) {

        //Allocate Temp Employee Structure
        struct employee_t *temp_employees = calloc(header->count - 1, sizeof(struct employee_t));
        if (temp_employees == NULL) {
            printf("main: Calloc Temp Employees Failed\n");
            return STATUS_ERROR;
        }

        //Populate temp employee structure
        if (Remove_Employee_By_Name(header,  employees, temp_employees, remove_name_str) == STATUS_SUCCESS) {

            //Resize header and employee pointers
            --header->count;
            free(employees);

            //Update employee pointer
            employees = temp_employees;

        } else {
            printf("main: Removing employee was unsuccessful");
            //Deallocate temp_employee pointer from the heap
            free(temp_employees);
            return STATUS_ERROR;
        }
    }

    if (update_hours_str) {

        if (Update_Employee(header, employees, update_hours_str) != STATUS_SUCCESS) {
            printf("main: Update Employee hours for %s failed", update_hours_str);

        }
    }

    // Always Output file
    if (Output_File(dbfd, header, employees) == STATUS_ERROR) {
        printf("main: Failed to Output File\n");
        return -1;
    }

    return 0; 
}

