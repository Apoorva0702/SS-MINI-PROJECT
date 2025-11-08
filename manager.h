#ifndef MANAGER_H
#define MANAGER_H
#include "common.h"


// -------- Function Declarations --------

// Login + Menu
int manager_login();
void manager_menu(int manager_id);

// Customer Account Control
void activate_deactivate_customer();

// Loan Assignment
void list_employees_for_assignment();
void assign_loan_to_employee();

// Feedback Review
void view_feedback();

// Password Change
void manager_change_password(int manager_id);

// Logout
void manager_logout(int manager_id);

#endif
