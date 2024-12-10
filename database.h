#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Database initialization
int init_database(void);

// Account operations
int create_account(const char *name, int *account_id, double initial_balance);
int deposit(int account_id, double amount);
int withdraw(int account_id, double amount);
double get_balance(int account_id);
int close_account(int account_id);
void list_accounts(void);
int get_next_account_id(void);

#endif