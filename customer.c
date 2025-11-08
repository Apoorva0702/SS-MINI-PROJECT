#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "common.h"
#include "fileutils.h"
#include "customer.h"

#define CUSTOMER_FILE "/home/apoorva-kharya/data/customers.dat"
#define TRANSACTION_FILE "/home/apoorva-kharya/data/transactions.dat"
#define LOAN_FILE "/home/apoorva-kharya/data/loans.dat"
#define FEEDBACK_FILE "/home/apoorva-kharya/data/feedback.dat"


// -------------------- LOGIN --------------------
int customer_login() {
    char username[50], password[50];
    printf("\nEnter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    int fd = open(CUSTOMER_FILE, O_RDONLY);
    if(fd < 0) { printf("Customer DB Missing!\n"); return -1; }

    Customer c;
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(strcmp(c.username, username) == 0 && strcmp(c.password, password) == 0) {
            if(c.is_active == 0) {
                printf("\n Your account is currently DEACTIVATED.\n");
                printf("   Please contact the Manager for assistance.\n");
                close(fd);
                return -1;
            } 
             
            close(fd);
            return c.id;
        }
    }

    close(fd);
    printf("\nInvalid Credentials!\n");
    return -1;
}

// -------------------- MENU --------------------
void customer_menu(int customer_id) {
    int choice;
    while(1) {
        printf("\n-----------------------------\n");
        printf("1. View Balance\n2. Deposit\n3. Withdraw\n4. Transfer\n5. Apply Loan\n6. Change Password\n7. Add Feedback\n8. View Transaction History\n9. Logout\n");
        printf("Enter Choice: ");
        scanf("%d",&choice);

        switch(choice){
            case 1: view_balance(customer_id); break;
            case 2: deposit_money(customer_id); break;
            case 3: withdraw_money(customer_id); break;
            case 4: transfer_money(customer_id); break;
            case 5: apply_loan(customer_id); break;
            case 6: change_password(customer_id); break;
            case 7: give_feedback(customer_id); break;
            case 8: view_transaction_history(customer_id); break;
            case 9: customer_logout(customer_id); return;
            default: printf("Invalid Option\n");
        }
    }
}

// -------------------- VIEW BALANCE --------------------
void view_balance(int customer_id) {
    int fd = open(CUSTOMER_FILE, O_RDWR);
    lock_file(fd);

    Customer c;
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(c.id == customer_id) {
            printf("\n Current Balance: %.2f\n", c.balance);
            break;
        }
    }

    unlock_file(fd);
    close(fd);
}

// -------------------- DEPOSIT --------------------
void deposit_money(int customer_id) {
    float amount;
    printf("\nEnter amount to deposit: ");
    scanf("%f", &amount);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    lock_file(fd);

    Customer c;
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(c.id == customer_id) {
            c.balance += amount;
            lseek(fd, -sizeof(c), SEEK_CUR);
            write(fd, &c, sizeof(c));
            break;
        }
    }

    unlock_file(fd);
    close(fd);

    // Record Transaction
    int tx = open(TRANSACTION_FILE, O_WRONLY|O_APPEND);
    Transaction t = {customer_id, "Deposit", amount, c.balance};
    write(tx, &t, sizeof(t));
    close(tx);

    printf("\n Deposit Successful!\n");
}

// -------------------- WITHDRAW --------------------
void withdraw_money(int customer_id) {
    float amount;
    printf("\nEnter amount to withdraw: ");
    scanf("%f", &amount);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    lock_file(fd);

    Customer c;
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(c.id == customer_id) {
            if(c.balance < amount) { printf("❌ Insufficient Balance!\n"); unlock_file(fd); close(fd); return; }
            c.balance -= amount;
            lseek(fd, -sizeof(c), SEEK_CUR);
            write(fd, &c, sizeof(c));
            break;
        }
    }

    unlock_file(fd);
    close(fd);

    // Record Transaction
    int tx = open(TRANSACTION_FILE, O_WRONLY|O_APPEND);
    Transaction t = {customer_id, "Withdraw", amount, c.balance};
    write(tx, &t, sizeof(t));
    close(tx);

    printf("\n Withdrawal Successful!\n");
}

// -------------------- TRANSFER MONEY --------------------
void transfer_money(int customer_id) {
    int target_id;
    float amount;

    printf("\nEnter Customer ID to transfer to: ");
    scanf("%d", &target_id);

    if(target_id == customer_id) {
        printf(" Cannot transfer to your own account!\n");
        return;
    }

    printf("Enter amount to transfer: ");
    scanf("%f", &amount);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    lock_file(fd);

    Customer sender, receiver;
    int sender_pos=-1, receiver_pos=-1;

    Customer temp;
    int pos = 0;
    while(read(fd, &temp, sizeof(Customer)) > 0) {
        if(temp.id == customer_id) {
            sender = temp;
            sender_pos = pos;
        }
        if(temp.id == target_id) {
            receiver = temp;
            receiver_pos = pos;
        }
        pos++;
    }

    if(receiver_pos == -1) {
        printf(" Target account not found!\n");
        unlock_file(fd); close(fd); return;
    }

    if(sender.balance < amount) {
        printf(" Insufficient Balance!\n");
        unlock_file(fd); close(fd); return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    lseek(fd, sender_pos * sizeof(Customer), SEEK_SET);
    write(fd, &sender, sizeof(Customer));

    lseek(fd, receiver_pos * sizeof(Customer), SEEK_SET);
    write(fd, &receiver, sizeof(Customer));

    unlock_file(fd);
    close(fd);

    // Log Transaction
    int tx = open(TRANSACTION_FILE, O_WRONLY|O_APPEND|O_CREAT, 0644);
    Transaction t1 = {sender.id, "Transferred Money", -amount, sender.balance};
    Transaction t2 = {receiver.id, "Received Money", amount, receiver.balance};
    write(tx, &t1, sizeof(t1));
    write(tx, &t2, sizeof(t2));
    close(tx);

    printf("\n Transfer Successful!\n");
}

// --------------------APPLY LOAN -----------------------
void apply_loan(int customer_id) {
    float amount;
    int tenure;

    printf("\nEnter Loan Amount: ");
    scanf("%f", &amount);
    printf("Enter Tenure (months): ");
    scanf("%d", &tenure);

    int fd = open(LOAN_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    lock_file(fd);

    Loan l={0};
    l.customer_id = customer_id;
    l.amount = amount;
    l.tenure = tenure;
    strcpy(l.status, "Pending");
    l.assigned_emp_id = -1;
    write(fd, &l, sizeof(l));

    unlock_file(fd);
    close(fd);

    printf("\n Loan Request Submitted (Status: Pending)\n");
}

//---------------------CHANGE PASSWORD ---------------------
void change_password(int customer_id) {
    char oldpass[50], newpass[50];

    printf("\nEnter Old Password: ");
    scanf("%s", oldpass);
    printf("Enter New Password: ");
    scanf("%s", newpass);

    int fd = open(CUSTOMER_FILE, O_RDWR);
    lock_file(fd);

    Customer c;
    int pos = 0;
    while(read(fd, &c, sizeof(Customer)) > 0) {
        if(c.id == customer_id) {
            if(strcmp(c.password, oldpass) == 0) {
                strcpy(c.password, newpass);
                lseek(fd, pos * sizeof(Customer), SEEK_SET);
                write(fd, &c, sizeof(c));
                printf("\n Password Changed Successfully!\n");
            } else {
                printf(" Incorrect Old Password!\n");
            }
            unlock_file(fd); close(fd); return;
        }
        pos++;
    }

    unlock_file(fd);
    close(fd);
}

//-----------------------------------------ADD FEEDBACK-----------------------------------
void give_feedback(int customer_id) {
    Feedback fb;
    fb.customer_id = customer_id;

    printf("\nEnter Feedback: ");
    scanf(" %[^\n]", fb.message); // read full sentence including spaces

    int fd = open(FEEDBACK_FILE , O_WRONLY | O_APPEND | O_CREAT, 0644);
    if(fd < 0) {
        printf(" Unable to record feedback.\n");
        return;
    }

    lock_file(fd);
    write(fd, &fb, sizeof(Feedback));
    unlock_file(fd);
    close(fd);

    printf("\n Thank you! Your feedback has been submitted.\n");
}

// -----------------------------VIEW TRANSACTION HISTORY----------------------------
void view_transaction_history(int customer_id) {
    int fd = open(TRANSACTION_FILE, O_RDONLY);
    if(fd < 0){ printf("No transactions yet.\n"); return; }

    Transaction t;
    printf("\n--- Transaction History ---\n");
    while(read(fd, &t, sizeof(t)) > 0) {
        if(t.customer_id == customer_id) {
            printf("%s | Amount: %.2f | Balance After: %.2f\n",
                   t.description, t.amount, t.balance_after);
        }
    }
    close(fd);
}
//-------------------------------LOGOUT---------------------------------------
void customer_logout(int customer_id) {
   printf("\n Logged out successfully.\n");

}


