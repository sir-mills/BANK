#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include <cJSON.h>

void display_menu(void)
{
    printf("\n=== Banking System ===\n");
    printf("1. Create Account\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Check Balance\n");
    printf("5. Close Account\n");
    printf("6. List All Accounts\n");
    printf("7. Exit\n");
    printf("Enter your choice: \n");
}

cJSON *parse_json_input(const char *input)
{
    cJSON *json = cJSON_Parse(input);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error parsing JSON object: %s\n", error_ptr);
        }
        return NULL;
    }
    return json;
}

int main(void)
{
    if (init_database() != SQLITE_OK)
    {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }

    int choice;
    char name[100];
    int account_id;
    double amount;

    while (1)
    {
        display_menu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter name: ");
            scanf("%s", name);
            printf("Enter initial deposit: ");
            scanf("%lf", &amount);
            if (create_account(name, &account_id, amount) == SQLITE_OK)
            {
                printf("Account created successfully! Account ID: %d\n", account_id);
            }
            break;

        case 2:
            printf("Enter account ID: ");
            scanf("%d", &account_id);
            printf("Enter deposit amount: ");
            scanf("%lf", &amount);
            if (deposit(account_id, amount) == SQLITE_OK)
            {
                printf("Deposit successful!\n");
            }
            break;
        case 3:
            printf("Enter account ID: ");
            scanf("%d", &account_id);
            printf("Enter Withdrawal Amount:");
            scanf("%lf", &amount);
            if (withdraw(account_id, amount) == SQLITE_OK)
            {
                printf("Withdrawal successful !\n");
            }
            break;
        case 4:
            printf("Enter Account ID: ");
            scanf("%d", &account_id);
            printf("Balance: %lf\n", get_balance(account_id));
            break;

        case 5:
            printf("Enter Account ID: ");
            scanf("%d", &account_id);
            if (close_account(account_id) == SQLITE_OK)
            {
                printf("Account closed successfully!\n");
            }
            break;

        case 6:
            list_accounts();

            break;

        case 7:
            printf("Thank you for using our banking system!\n");
            return 0;

        default:
            printf("Invalid choice!\n");
        }
    }

    return 0;
}