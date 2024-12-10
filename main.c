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

int handle_create_account(const char *json_str)
{
    cJSON *json = parse_json_input(json_str);
    if (json == NULL)
        return -1;
    cJSON *name_item = cJSON_GetObjectItem(json, "name");
    cJSON *amount_item = cJSON_GetObjectItem(json, "initial_deposit");

    if (!cJSON_IsString(name_item) || !cJSON_IsNumber(amount_item))
    {
        fprintf(stderr, "Invalid JSON Input Format\n");
        cJSON_Delete(json);
        return -1;
    }
    const char *name = name_item->valuestring;
    double amount = amount_item->valuedouble;

    int account_id;

    int result = create_account(name, &account_id, amount);

    cJSON *response = cJSON_CreateObject();

    cJSON_AddNumberToObject(response, "account_id", account_id);
    cJSON_AddNumberToObject(response, "status", result == SQLITE_OK ? 1 : 0);

    char message[256];
    if (result == SQLITE_OK)
    {
        snprintf(message, sizeof(message), "Account created, Account ID: %d", account_id);
    }
    else
    {
        snprintf(message, sizeof(message), "Failed to create account");
    }

    cJSON_AddStringToObject(response, "message", message);

    char *response_str = cJSON_Print(response);
    printf("%s\n", response_str);

    free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(json);

    return result;
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
    char json_input[512];

    while (1)
    {
        display_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            printf("Enter Account Details: ");
            fgets(json_input, sizeof(json_input), stdin);

            handle_create_account(json_input);
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