#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

static sqlite3 *db;

int init_database(void)
{
    int rc = sqlite3_open("bank.db", &db);
    if (rc)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS accounts ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "balance REAL NOT NULL,"
                      "active INTEGER DEFAULT 1);";

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    return SQLITE_OK;
}

int create_account(const char *name, int *account_id, double initial_balance)
{
    char sql[256];
    sprintf(sql, "INSERT INTO accounts (name, balance) VALUES ('%s', %.2f);",
            name, initial_balance);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }
    *account_id = (int)sqlite3_last_insert_rowid(db);
    return SQLITE_OK;
}

int deposit(int account_id, double amount)
{
    char sql[256];
    sprintf(sql, "UPDATE accounts SET balance = balance + %.2f WHERE id = %d;", amount, account_id);
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

int close_account(int account_id)
{
    char sql[256];
    sprintf(sql, "DELETE FROM accounts WHERE id = %d;", account_id);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

double get_balance(int account_id)
{
    char sql[256];
    sprintf(sql, "SELECT balance FROM accounts WHERE id = %d;", account_id);
    sqlite3_stmt *stmt;
    double balance;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            balance = sqlite3_column_double(stmt, 0);
        }
    }
    else
    {
        fprintf(stderr, "Failed to fetch balance: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return balance; // Return the actual balance
}

void list_accounts()
{
    const char *sql = "SELECT id, name, balance, active FROM accounts;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch accounts: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\n=== List of Accounts ===\n");
    printf("ID\tName\t\tBalance\t\tActive\n");
    printf("-----------------------------------------\n");

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        double balance = sqlite3_column_double(stmt, 2);
        int active = sqlite3_column_int(stmt, 3);

        printf("%d\t%s\t\t%.2f\t\t%s\n", id, name, balance, active ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
}

int withdraw(int account_id, double amount)
{
    char sql[256];
    sqlite3_stmt *stmt;
    double current_balance = 0.0;

    // Step 1: Retrieve the current balance
    sprintf(sql, "SELECT balance FROM accounts WHERE id = %d;", account_id);
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch balance: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        current_balance = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (current_balance < amount)
    {
        fprintf(stderr, "Insufficient funds\n");
        return SQLITE_ERROR;
    }

    sprintf(sql, "UPDATE accounts SET balance = balance - %.2f WHERE id = %d;", amount, account_id);
    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}
