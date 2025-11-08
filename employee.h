#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include "common.h"


// ------------ Function Declarations ------------

// Login + Menu
int employee_login();
void employee_menu(int employee_id);

// Customer Management

void add_customer();
void modify_customer();

// Transactions
void view_customer_transaction_history();

// Loan Processing
void view_assigned_loans(int employee_id);
void approve_reject_loan(int employee_id);

// Security
void employee_change_password(int employee_id);

// Logout
void employee_logout(int employee_id);
void debug_list_employees() ;
void debug_list_loans();
#endif
