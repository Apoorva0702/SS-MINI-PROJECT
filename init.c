#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define DATA_DIR "/home/apoorva-kharya/data/"

typedef struct {
    int id;
    char name[50];
    char username[50];
    char password[50];
    int is_logged_in;
} Admin;

typedef struct {
    int id;
    char name[50];
    char username[50];
    char password[50];
    int is_logged_in;
} Employee;

typedef struct {
    int id;
    char name[50];
    char username[50];
    char password[50];
    int is_logged_in;
} Manager;

typedef struct {
    int id;
    char name[50];
    char username[50];
    char password[50];
    float balance;
    int is_active;
    int is_logged_in;
} Customer;

int main() {

    printf("\nInitializing Data Files...\n");

    // Create data directory if missing
    mkdir(DATA_DIR, 0755);

    // Create and seed Admin
    FILE *fa = fopen("/home/apoorva-kharya/data/admins.dat", "wb");
    Admin a = {1, "SystemAdmin", "admin", "admin123" , 0};
    fwrite(&a, sizeof(Admin), 1, fa);
    fclose(fa);

    // Create and seed Employee
    FILE *fe = fopen("/home/apoorva-kharya/data/employees.dat", "wb");
    Employee e = {1, "Ravi", "emp1", "11" , 0};
    fwrite(&e, sizeof(Employee), 1, fe);
    fclose(fe);

    // Create and seed Manager
    FILE *fm = fopen("/home/apoorva-kharya/data/managers.dat", "wb");
    Manager m = {1, "Meena", "mgr1", "mgr123" , 0};
    fwrite(&m, sizeof(Manager), 1, fm);
    fclose(fm);

    // Create and seed Customer
    FILE *fc = fopen("/home/apoorva-kharya/data/customers.dat", "wb");
    Customer c = {1, "Sanjh", "cust1", "11", 5000.0, 1 , 0};
    fwrite(&c, sizeof(Customer), 1, fc);
    fclose(fc);

    // Create empty transaction, loan, feedback files
    fclose(fopen("/home/apoorva-kharya/data/transactions.dat", "wb"));
    fclose(fopen("/home/apoorva-kharya/data/loans.dat", "wb"));
    fclose(fopen("/home/apoorva-kharya/data/feedback.dat", "wb"));

    printf("\n✅ Data Initialization Completed Successfully.\n");
    printf("Default Credentials:\n");
    printf(" Admin:   username = admin, password = admin123\n");
    printf(" Employee: ID = 1, username = emp1, password = 11\n");
    printf(" Manager:  ID = 1, username = mgr1, password = mgr123\n");
    printf(" Customer: ID = 1, username = cust1, password = 11\n\n");

    return 0;
}
