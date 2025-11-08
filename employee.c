#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "common.h"
#include "fileutils.h"
#include "employee.h"

#define CUSTOMER_FILE "/home/apoorva-kharya/data/customers.dat"
#define TRANSACTION_FILE "/home/apoorva-kharya/data/transactions.dat"
#define LOAN_FILE "/home/apoorva-kharya/data/loans.dat"
#define EMPLOYEE_FILE "/home/apoorva-kharya/data/employees.dat"


// -------------------- LOGIN --------------------
int employee_login() {
    char username[50], password[50];
    printf("\nEnter Employee Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    int fd = open(EMPLOYEE_FILE, O_RDONLY);
    if(fd < 0) { printf("Employee DB Missing!\n"); return -1; }

    Employee e;
    while(read(fd, &e, sizeof(Employee)) > 0) {
        if(strcmp(e.username, username) == 0 && strcmp(e.password, password) == 0) {
            close(fd);
            return e.id;
        }
    }

    close(fd);
    printf("\n Invalid Employee Credentials!\n");
    return -1;
}

// -------------------- MENU --------------------
void employee_menu(int employee_id) {
    int choice;

    while(1) {
        printf("\n--------------------------------\n");
        printf("1. Add Customer\n2. Modify Customer\n3. View Customer Transaction History\n4. View Assigned Loan Requests\n5. Approve / Reject Loan\n6. Change Password\n7. Logout\n8. employees\n9. loans\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: add_customer(); break;
            case 2: modify_customer(); break;
            case 3: view_customer_transaction_history(); break;
            case 4: view_assigned_loans(employee_id); break;
            case 5: approve_reject_loan(employee_id); break;
            case 6: employee_change_password(employee_id); break;
            case 7: employee_logout(employee_id); return;
            case 8: debug_list_employees();break;
            case 9: debug_list_loans() ; break; 
            default: printf("Invalid Option!\n");
        }
    }
}
//-------------------------------ADD CUSTOMER--------------------
void add_customer() {
    Customer c;
    printf("\n--- Add New Customer ---\n");

    printf("Enter Customer ID: ");
    scanf("%d", &c.id);

    printf("Enter Customer Name: ");
    scanf("%s", c.name);

    printf("Enter Username: ");
    scanf("%s", c.username);

    printf("Enter Password: ");
    scanf("%s", c.password);

    printf("Enter Initial Balance: ");
    scanf("%f", &c.balance);

    int fd = open(CUSTOMER_FILE, O_RDWR | O_APPEND | O_CREAT, 0644);
    if(fd < 0) {
        printf("Error opening customer file!\n");
        return;
    }

    lock_file(fd);
    write(fd, &c, sizeof(Customer));
    unlock_file(fd);
    close(fd);

    printf("\n Customer Added Successfully!\n");
}
//-----------------------MODIFY CUSTOMER DETAILS-------------------
void modify_customer() {
    int id;
    printf("\nEnter Customer ID to modify: ");
    scanf("%d", &id);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    if(fd < 0) {
        printf(" Customer database missing!\n");
        return;
    }

    lock_file(fd);

    Customer c;
    int pos = 0, found = 0;
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(c.id == id) {
            found = 1;

            printf("Current Name: %s\n", c.name);
            printf("Enter New Name: ");
            scanf("%s", c.name);

            printf("Current Username: %s\n", c.username);
            printf("Enter New Username: ");
            scanf("%s", c.username);

            lseek(fd, pos * sizeof(Customer), SEEK_SET);
            write(fd, &c, sizeof(Customer));

            printf("\n Customer Details Updated Successfully!\n");
            break;
        }
        pos++;
    }

    if(!found)
        printf("\n Customer ID Not Found!\n");

    unlock_file(fd);
    close(fd);
}

//--------------- VIEW CUSTOMER TRANSACTION HISTORY ---------
void view_customer_transaction_history() {
    int cid;
    printf("\nEnter Customer ID: ");
    scanf("%d", &cid);

    int fd = open(TRANSACTION_FILE, O_RDONLY);
    if(fd < 0) {
        printf(" No transaction file found.\n");
        return;
    }

    Transaction t;
    int found = 0;

    printf("\n--- Transaction History for Customer %d ---\n", cid);
    while(read(fd, &t, sizeof(Transaction)) > 0) {
        if(t.customer_id == cid) {
            found = 1;
            printf("%s | Amount: %.2f | Balance After: %.2f\n",
                   t.description, t.amount, t.balance_after);
        }
    }

    if(!found)
        printf("No transactions found for this customer.\n");

    close(fd);
}

//------------------------------VIEW LOAN APPLICATIONS ----------------------

void view_assigned_loans(int employee_id) {
    int fd = open(LOAN_FILE , O_RDONLY);
    if(fd < 0) { printf(" No loans.\n"); return; }

    Loan l;
    printf("\n--- Assigned Loan Applications ---\n");

    while(read(fd, &l, sizeof(Loan)) > 0) {
        if(l.assigned_emp_id == employee_id && strcmp(l.status, "Pending") == 0) {
            printf("Customer ID: %d | Amount: %.2f | Tenure: %d months | Status: %s\n",
                   l.customer_id, l.amount, l.tenure, l.status);
        }
    }

    close(fd);
}

//-------------------------------aprrove/reject loans-----------------------

void approve_reject_loan(int employee_id) {
    int cid;
    printf("\nEnter Customer ID: ");
    scanf("%d", &cid);

    int loan_fd = open(LOAN_FILE, O_RDWR); // LOAN_FILE is defined in employee.c
    if(loan_fd < 0) { printf(" No loans.\n"); return; }

    lock_file(loan_fd);

    Loan l;
    int pos = 0, found = 0;

    // 1. Find and Process Loan Record
    while(read(loan_fd, &l, sizeof(Loan)) > 0) {
        if(l.customer_id == cid && l.assigned_emp_id == employee_id && strcmp(l.status, "Pending") == 0) {
            found = 1;

            int choice;
            printf("\n1. Approve\n2. Reject\nEnter choice: ");
            scanf("%d", &choice);

            if(choice == 1) {
                strcpy(l.status, "Approved");
                
                //  Update Customer Balance on Approval ---
                int cust_fd = open(CUSTOMER_FILE, O_RDWR);
                if (cust_fd >= 0) {
                    lock_file(cust_fd); // Lock the Customer file
                    
                    Customer c;
                    int cust_pos = 0;
                    
                    // Iterate through customers to find the one whose loan was approved
                    while(read(cust_fd, &c, sizeof(Customer)) > 0) {
                        if(c.id == cid) {
                            c.balance += l.amount; // Add loan amount
                            
                            // Go back and write the updated customer record
                            lseek(cust_fd, cust_pos * sizeof(Customer), SEEK_SET);
                            write(cust_fd, &c, sizeof(Customer));
                            break; 
                        }
                        cust_pos++;
                    }
                    
                    unlock_file(cust_fd); // Unlock the Customer file
                    close(cust_fd);
                    
                    printf("\n Customer Balance Updated: %.2f added.\n", l.amount);

                }
                
            } else {
                strcpy(l.status, "Rejected");
            }

            // Go back and write the updated loan status
            lseek(loan_fd, pos * sizeof(Loan), SEEK_SET);
            write(loan_fd, &l, sizeof(Loan));

            printf("\n Loan %s Successfully.\n", l.status);
            break;
        }
        pos++;
    }

    if(!found) printf(" Loan not found or not assigned to you.\n");

    unlock_file(loan_fd);
    close(loan_fd);
}


//-----------------------------------CHANGE PASSWORD -------------------------
void employee_change_password(int employee_id) {
    char oldpass[50], newpass[50];

    printf("\nEnter Old Password: ");
    scanf("%s", oldpass);
    printf("Enter New Password: ");
    scanf("%s", newpass);

    int fd = open(EMPLOYEE_FILE, O_RDWR);
    lock_file(fd);

    Employee e;
    int pos = 0, found = 0;

    while(read(fd, &e, sizeof(Employee)) > 0) {
        if(e.id == employee_id) {
            found = 1;
            if(strcmp(e.password, oldpass) == 0) {
                strcpy(e.password, newpass);
                lseek(fd, pos * sizeof(Employee), SEEK_SET);
                write(fd, &e, sizeof(Employee));
                printf("\n Password Changed Successfully!\n");
            } else {
                printf(" Incorrect Old Password!\n");
            }
            break;
        }
        pos++;
    }

    unlock_file(fd);
    close(fd);

    if(!found)
        printf(" Employee not found.\n");
}

//-------------------------------LOGOUT---------------------------------
void employee_logout(int employee_id) {
    printf("\n Employee Logged Out Successfully.\n");
}   

void debug_list_employees() {
    int fd = open(EMPLOYEE_FILE, O_RDONLY);
    Employee e;
    printf("\n--- EMPLOYEE DATABASE CONTENTS ---\n");
    while(read(fd, &e, sizeof(Employee)) > 0) {
        printf("ID=%d | USER=%s | PASS=%s\n", e.id, e.username, e.password);
    }
    close(fd);
}

void debug_list_loans() {
    int fd = open("/home/apoorva-kharya/data/loans.dat", O_RDONLY);
    Loan l;
    printf("\n--- LOAN DATABASE CONTENTS ---\n");
    while(read(fd, &l, sizeof(Loan)) > 0) {
        printf("CID=%d | AMT=%.2f | TENURE=%d | STATUS=%s | EMP_ID=%d\n",
            l.customer_id, l.amount, l.tenure, l.status, l.assigned_emp_id);
    }
    close(fd);
}
