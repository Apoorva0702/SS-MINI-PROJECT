#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "common.h"
#include "fileutils.h"
#include "admin.h"

#define ADMIN_FILE "/home/apoorva-kharya/data/admins.dat"
#define EMPLOYEE_FILE "/home/apoorva-kharya/data/employees.dat"
#define MANAGER_FILE "/home/apoorva-kharya/data/managers.dat"
#define CUSTOMER_FILE "/home/apoorva-kharya/data/customers.dat"





int admin_login() {
    char username[50], password[50];
    int fd = open(ADMIN_FILE, O_RDONLY);

    if(fd < 0) {
        printf("Admin database missing!\n");
        return -1;
    }

    printf("\nEnter Admin Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    Admin a;
    while(read(fd, &a, sizeof(Admin)) > 0) {
        if(strcmp(a.username, username) == 0 && strcmp(a.password, password) == 0) {
            close(fd);
            printf("\nLogin Successful! Welcome, %s\n", a.name);
            return a.id;
        }
    }

    close(fd);
    printf("\n Invalid Credentials.\n");
    return -1;
}



//------------------------------ADMIN MENU------------------------------

void admin_menu(int admin_id) {
    int choice;

    while(1) {
        printf("\n=========== ADMIN MENU ===========\n");
        printf("1. Add Employee\n");
        printf("2. Modify Employee\n");
        printf("3. Modify Customer\n");
        printf("4. Promote Employee to Manager\n");
        printf("5. Change Password\n");
        printf("6. Logout\n");
        printf("=================================\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: admin_add_employee(); break;
            case 2: admin_modify_employee(); break;
            case 3: admin_modify_customer(); break;
            case 4: admin_promote_employee_to_manager(); break;
            case 5: admin_change_password(admin_id); break;
            case 6: admin_logout(admin_id); return;
            default: printf("Invalid choice. Try again.\n");
        }
    }
}


//--------------------------add new employeee---------------
void admin_add_employee() {
    Employee e;

    printf("\n--- Add New Employee ---\n");

    printf("Enter Employee ID: ");
    scanf("%d", &e.id);

    printf("Enter Name: ");
    scanf("%s", e.name);

    printf("Enter Username: ");
    scanf("%s", e.username);
    
    printf("Enter Password: ");
    scanf("%s", e.password);

    int fd = open(EMPLOYEE_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    lock_file(fd);
    write(fd, &e, sizeof(e));
    unlock_file(fd);
    close(fd);

    printf("\nEmployee Added Successfully!\n");
}

//-------------------------------modify employee details-------------------
void admin_modify_employee() {
    int id;
    printf("\nEnter Employee ID to modify: ");
    scanf("%d", &id);

    int fd = open(EMPLOYEE_FILE, O_RDWR);
    if(fd < 0){ printf("Employee DB missing\n"); return; }

    lock_file(fd);
    Employee e;
    int pos = 0, found = 0;

    while(read(fd, &e, sizeof(e)) > 0) {
        if(e.id == id) {
            found = 1;
            printf("Enter New Name: ");
            scanf("%s", e.name);
            printf("Enter New Username: ");
            scanf("%s", e.username);

            lseek(fd, pos * sizeof(Employee), SEEK_SET);
            write(fd, &e, sizeof(e));
            printf("\nEmployee Updated!\n");
            break;
        }
        pos++;
    }

    if(!found) printf("Employee not found!\n");
    unlock_file(fd);
    close(fd);
}

//----------------------modify customer details--------------------
void admin_modify_customer() {
    int id;
    printf("\nEnter Customer ID to modify: ");
    scanf("%d", &id);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    if(fd < 0){ printf("Customer DB missing\n"); return; }

    lock_file(fd);
    Customer c;
    int pos = 0, found = 0;

    while(read(fd, &c, sizeof(c)) > 0) {
        if(c.id == id) {
            found = 1;
            printf("Enter New Name: ");
            scanf("%s", c.name);
            printf("Enter New Username: ");
            scanf("%s", c.username);
            lseek(fd, pos * sizeof(Customer), SEEK_SET);
            write(fd, &c, sizeof(c));
            printf("\n Customer Updated!\n");
            break;
        }
        pos++;
    }

    if(!found) printf("Customer not found!\n");
    unlock_file(fd);
    close(fd);
}

//---------------------------CHANGE ROLES ---------------------
void admin_promote_employee_to_manager() {
    int id;
    printf("\nEnter Employee ID to Promote to Manager: ");
    scanf("%d", &id);

    int fd = open(EMPLOYEE_FILE, O_RDONLY);
    if(fd < 0){ printf("Employee DB missing\n"); return; }

    Employee e;
    int found = 0;
    while(read(fd, &e, sizeof(e)) > 0) {
        if(e.id == id) { found = 1; break; }
    }
    close(fd);

    if(!found){
        printf("Employee Not Found!\n");
        return;
    }

    int fm = open(MANAGER_FILE, O_APPEND | O_WRONLY | O_CREAT, 0644);
    lock_file(fm);
    write(fm, &e, sizeof(e)); // same credentials become manager
    unlock_file(fm);
    close(fm);

    printf("\nEmployee Promoted to Manager!\n");
}

//--------------------------CHANGE PASSWORD----------------------
void admin_change_password(int admin_id) {
    char oldp[50], newp[50];

    printf("\nEnter Old Password: ");
    scanf("%s", oldp);
    printf("Enter New Password: ");
    scanf("%s", newp);

    int fd = open(ADMIN_FILE, O_RDWR);
    lock_file(fd);

    Admin a;
    int pos = 0, found = 0;

    while(read(fd, &a, sizeof(a)) > 0) {
        if(a.id == admin_id) {
            found = 1;
            if(strcmp(a.password, oldp) == 0) {
                strcpy(a.password, newp);
                lseek(fd, pos * sizeof(Admin), SEEK_SET);
                write(fd, &a, sizeof(a));
                printf("\n Password Updated Successfully!\n");
            } else printf(" Incorrect Old Password!\n");
            break;
        }
        pos++;
    }

    unlock_file(fd);
    close(fd);
}

//--------------------------LOGOUT-----------------
void admin_logout(int admin_id) {
    printf("\n Admin Logged Out Successfully.\n");
}

