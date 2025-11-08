#ifndef COMMON_H
#define COMMON_H

#define MAX_NAME 50
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_FEEDBACK 200

typedef struct {
    int id;
    char name[MAX_NAME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int is_logged_in;
} Admin;

typedef struct {
    int id;
    char name[MAX_NAME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int is_logged_in;
} Manager;

typedef struct {
    int id;
    char name[MAX_NAME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int is_logged_in;
} Employee;

typedef struct {
    int id;
    char name[MAX_NAME];
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    float balance;
    int is_active;// 1 = active, 0 = deactivated
    int is_logged_in;
} Customer;

typedef struct {
    int customer_id;
    char description[100];
    float amount;
    float balance_after;
} Transaction;

typedef struct {
    int customer_id;
    float amount;
    int tenure;
    char status[20];
    int assigned_emp_id;
} Loan;

typedef struct {
    int customer_id;
    char message[MAX_FEEDBACK];
} Feedback;


#endif
