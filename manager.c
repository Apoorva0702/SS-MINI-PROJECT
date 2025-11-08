#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/file.h>
#include "common.h"
#include "fileutils.h"
#include "manager.h"

#define CUSTOMER_FILE "/home/apoorva-kharya/data/customers.dat"
#define FEEDBACK_FILE "/home/apoorva-kharya/data/feedback.dat"
#define LOAN_FILE "/home/apoorva-kharya/data/loans.dat"
#define EMPLOYEE_FILE "/home/apoorva-kharya/data/employees.dat"
#define MANAGER_FILE "/home/apoorva-kharya/data/managers.dat"




// ------------- LOGIN---------------------
int manager_login() {
    char username[50], password[50];
    printf("\nEnter Manager Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    int fd = open(MANAGER_FILE, O_RDONLY);
    if(fd < 0) { printf("Manager database missing!\n"); return -1; }

    Manager m;
    while(read(fd, &m, sizeof(Manager)) > 0) {
        if(strcmp(m.username, username) == 0 && strcmp(m.password, password) == 0) {
            close(fd);
            return m.id;
        }
    }

    close(fd);
    printf("\n Invalid Manager Credentials!\n");
    return -1;
}

//------------------------------MENU----------------------------------

void manager_menu(int manager_id) {
    int choice;
    while(1) {
        printf("\n---------------- Manager Menu ----------------\n");
        printf("1. Activate / Deactivate Customer Account\n");
        printf("2. Assign Loan to Employee\n");
        printf("3. View Customer Feedback\n");
        printf("4. Change Password\n");
        printf("5. Logout\n");
        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: activate_deactivate_customer(); break;
            case 2: assign_loan_to_employee(); break;
            case 3: view_feedback(); break;
            case 4: manager_change_password(manager_id); break;
            case 5: manager_logout(manager_id); return;
            default: printf("Invalid Option!\n");
        }
    }
}

//---------------------------------ACTIVATE/DEACTIVATE ACCOUNT--------------------------
void activate_deactivate_customer() {
    int cid;
    printf("\nEnter Customer ID to Activate/Deactivate the account: ");
    scanf("%d", &cid);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    if(fd < 0) {
        printf("\n Unable to open customers database.\n");
        return;
    }

    lock_file(fd);

    Customer c;
    int found = 0;
    off_t pos = 0;

    // Read all customers until match found
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(c.id == cid) {
            found = 1;
            c.is_active = !c.is_active; // toggle active state

            // Move write pointer to correct record location
            lseek(fd, pos * sizeof(Customer), SEEK_SET);
            write(fd, &c, sizeof(Customer));

            printf("\n Customer %d is now %s.\n",
                   cid, c.is_active ? "ACTIVE" : "DEACTIVATED");
            break;
        }
        pos++;
    }

    if(!found)
        printf("\n Customer ID %d not found.\n", cid);

    unlock_file(fd);
    close(fd);
}

//---------------------------------- ASSIGN LOAN TO EMPLOYEE-------------------------
void list_employees_for_assignment() {
    int fd = open(EMPLOYEE_FILE, O_RDONLY);
    Employee e;
    printf("\nAvailable Employees:\n");
    while(read(fd, &e, sizeof(Employee)) > 0)
        printf("ID=%d | USER=%s\n", e.id, e.username);
    close(fd);
}



void assign_loan_to_employee() {
    int cid, empid;

    printf("\nEnter Customer ID (Loan Applicant): ");
    scanf("%d", &cid);

    printf("Enter Employee ID to Assign: ");
    scanf("%d", &empid);

    int fd = open(LOAN_FILE, O_RDWR);
    if(fd < 0) {
        printf(" Loan file not found!\n");
        return;
    }

    lock_file(fd);

    Loan l;
    int pos = 0, found = 0;

    while(read(fd, &l, sizeof(Loan)) > 0) {
        if(l.customer_id == cid) {
            found = 1;
            l.assigned_emp_id = empid;

            lseek(fd, pos * sizeof(Loan), SEEK_SET);
            write(fd, &l, sizeof(Loan));

            printf("\n Loan Application Assigned Successfully!\n");
            break;
        }
        pos++;
    }

    if(!found)
        printf(" No Loan Found for this Customer!\n");

    unlock_file(fd);
    close(fd);
}

// -----------------------------VIEW CUSTOMER FEEDBACK--------------------
void view_feedback() {
    int fd = open(FEEDBACK_FILE, O_RDONLY);
    if(fd < 0) {
        printf("\n No Feedback Available.\n");
        return;
    }

    // --- ADD FILE LOCKING ---
    lock_file(fd); 

    Feedback fb;

    printf("\n----- All Customer Feedback -----\n");

    while (read(fd, &fb, sizeof(Feedback)) == sizeof(Feedback)) {
        printf("Customer ID: %d\n", fb.customer_id);
        printf("Feedback: %s\n", fb.message);
        printf("--------------------------------\n");
    }

    // --- ADD UNLOCKING ---
    unlock_file(fd);
    close(fd);
}

//--------------------------CHANGE PASSWORD ---------------------
void manager_change_password(int manager_id) {
    char oldpass[50], newpass[50];

    printf("\nEnter Old Password: ");
    scanf("%s", oldpass);
    printf("Enter New Password: ");
    scanf("%s", newpass);

    int fd = open(MANAGER_FILE, O_RDWR);
    lock_file(fd);

    Manager m;
    int pos = 0, found = 0;

    while(read(fd, &m, sizeof(Manager)) > 0) {
        if(m.id == manager_id) {
            found = 1;
            if(strcmp(m.password, oldpass) == 0) {
                strcpy(m.password, newpass);
                lseek(fd, pos * sizeof(Manager), SEEK_SET);
                write(fd, &m, sizeof(Manager));
                printf("\n Password Updated Successfully!\n");
            } else {
                printf(" Incorrect Old Password!\n");
            }
            break;
        }
        pos++;
    }

    unlock_file(fd);
    close(fd);

    if(!found) printf(" Manager not found.\n");
}

//------------------------------LOGOUT-----------------------------
void manager_logout(int manager_id) {
    int fd = open(MANAGER_FILE, O_RDWR);
    if(fd < 0) return;

    lock_file(fd);

    Manager m;
    off_t pos = 0;
    while(read(fd, &m, sizeof(Manager)) == sizeof(Manager)) {
        if(m.id == manager_id) {
            m.is_logged_in = 0;
            lseek(fd, pos * sizeof(Manager), SEEK_SET);
            write(fd, &m, sizeof(Manager));
            break;
        }
        pos++;
    }

    unlock_file(fd);
    close(fd);

    printf("\nManager logged out successfully.\n");
}


