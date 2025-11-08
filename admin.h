#ifndef ADMIN_H
#define ADMIN_H
#include "common.h"


// ---------- Function Declarations ----------

int admin_login();
void admin_menu(int admin_id);

// Employee Management
void admin_add_employee();
void admin_modify_employee();

// Customer Management
void admin_modify_customer();

// Role Management
void admin_promote_employee_to_manager();

// Password Change
void admin_change_password(int admin_id);

// Logout
void admin_logout(int admin_id);

#endif
