#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "common.h"


// -------- Function Declarations --------

// Login + Menu
int customer_login();
void customer_menu(int customer_id);

// Core Banking Operations
void view_balance(int customer_id);
void deposit_money(int customer_id);
void withdraw_money(int customer_id);
void transfer_money(int customer_id);

// Loan
void apply_loan(int customer_id);

// Password + Feedback
void change_password(int customer_id);
void give_feedback(int customer_id);

// Transaction History
void view_transaction_history(int customer_id);

// Logout (Session End)
void customer_logout(int customer_id);

#endif
