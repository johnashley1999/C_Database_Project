#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

struct db_header_t{
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

static const struct db_header_t new_db_header_default = {
    .magic = HEADER_MAGIC,
    .version = 0x1,
    .count = 0,
    .filesize = sizeof(struct db_header_t)
};

struct employee_t{
    char name[256];
    char address[256];
    unsigned int hours;
};

int Create_Db_Header(int fd, 
                     struct db_header_t **headerOut);

int Validate_Db_Header(int fd, 
                       struct db_header_t **headerOut);

int Add_Employees(struct db_header_t *header, 
                  struct employee_t *employees, 
                  char *addstring);

int Update_Employee(struct db_header_t *header, 
                    struct employee_t *employees, 
                    char *update_hours_str);

int Remove_Employee_By_Name(struct db_header_t *header, 
                            struct employee_t *employees,
                            struct employee_t *temp_employees, 
                            char *remove_name_str);

void List_Employees(struct db_header_t *header, 
                    struct employee_t *employees);

int Read_Employees(int fd,
                   struct db_header_t *,
                   struct employee_t **employeesOut);

int Output_File(int fd, 
                struct db_header_t *headerOut,
                struct employee_t *employees);
#endif