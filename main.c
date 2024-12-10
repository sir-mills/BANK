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

int handle_deposit(const char *json_str)
{
    cJSON *json = parse_json_input(json_str);
    if (json == NULL)
        return -1;

    cJSON *account_id_item = cJSON_GetObjectItem(json, "account_id");
    cJSON *amount_item = cJSON_GetObjectItem(json, "amount");

    if (!cJSON_IsNumber(account_id_item) || !cJSON_IsNumber(amount_item))
    {
        fprintf(stderr, "invalid format enter a number\n");
        cJSON_Delete(json);
        return -1;
    }

    int account_id = account_id_item->valueint;
    double amount = amount_item->valuedouble;

    int result = deposit(account_id, amount);

    cJSON *response = cJSON_CreateObject();

    cJSON_AddNumberToObject(response, "account_id", account_id);
    cJSON_AddNumberToObject(response, "status", result == SQLITE_OK ? 1 : 0);

    char message[256];
    if (result == SQLITE_OK)
    {
        snprintf(message, sizeof(message), "Account ID: %d, Deposit: %d Successful ", account_id, amount);
    }
    else
    {
        snprintf(message, sizeof(message), "Account ID: %d Failed to deposit", account_id);
    }

    cJSON_AddStringToObject(response, "message", message);

    char *response_str = cJSON_Print(response);
    printf("%s\n", response_str);

    free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(json);

    return result;
}

int handle_withdraw(const char *json_str)
{
    cJSON *json = parse_json_input(json_str);
    if (json == NULL)
        return -1;

    cJSON *account_id_item = cJSON_GetObjectItem(json, "account_id");
    cJSON *amount_item = cJSON_GetObjectItem(json, "amount");

    if (!cJSON_IsNumber(account_id_item) || !cJSON_IsNumber(amount_item))
    {
        fprintf(stderr, "invalid format enter a number\n");
        cJSON_Delete(json);
        return -1;
    }

    int account_id = account_id_item->valueint;
    double amount = amount_item->valuedouble;

    int result = withdraw(account_id, amount);

    cJSON *response = cJSON_CreateObject();

    cJSON_AddNumberToObject(response, "account_id", account_id);
    cJSON_AddNumberToObject(response, "status", result == SQLITE_OK ? 1 : 0);

    char message[256];
    if (result == SQLITE_OK)
    {
        snprintf(message, sizeof(message), "Account ID: %d, Withdraw : %d Successful", account_id, amount);
    }
    else
    {
        snprintf(message, sizeof(message), "Account ID: %d Failed to withdraw", account_id);
    }

    cJSON_AddStringToObject(response, "message", message);

    char *response_str = cJSON_Print(response);
    printf("%s\n", response_str);

    free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(json);

    return result;
}

int handle_get_balance(const char *json_str)
{
    cJSON *json = parse_json_input(json_str);
    if (json == NULL)
        return -1;
    cJSON *account_id_item = cJSON_GetObjectItem(json, "account_id");

    if (!cJSON_IsNumber(account_id_item))
    {
        fprintf(stderr, "invalid format enter a number\n");
        cJSON_Delete(json);
        return -1;
    }
    int account_id = account_id_item->valueint;
    double balance = get_balance(account_id);

    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "account_id", account_id);
    cJSON_AddNumberToObject(response, "status", 1);

    char message[256];
    if (1)
    {
        sprintf(message, "Account ID: &d, Balance : %lf", account_id, balance);
    }
    else
    {
        sprintf(message, "Account ID: %d, Balance : %lf", account_id, balance);
    }
    cJSON_AddStringToObject(response, "message", message);

    char *response_str = cJSON_Print(response);
    printf("%s\n", response_str);

    free(response_str);
    cJSON_Delete(response);
    cJSON_Delete(json);

    return 0;
}

int handle_close_account(const char *json_str)
{
    cJSON *json = parse_json_input(json_str);
    if (json == NULL)
        return -1;
    cJSON *account_id_item = cJSON_GetObjectItem(json, "account_id");

    if (!cJSON_IsNumber(account_id_item))
    {
        fprintf(stderr, "invalid format enter a number\n");
        cJSON_Delete(json);
        return -1;
    }
    int account_id = account_id_item->valueint;

    int result = close_account(account_id);

    cJSON *response = cJSON_CreateObject();

    cJSON_AddNumberToObject(response, "account_id", account_id);
    cJSON_AddNumberToObject(response, "status", result == SQLITE_OK ? 1 : 0);

    char message[256];
    if (result == SQLITE_OK)
    {
        sprintf(message, "Account ID: %d, closed successfully", account_id);
    }
    else
    {
        sprintf(message, "Account ID: %d, Failed to close aza", account_id);
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
            fgets(json_input, sizeof(json_input), stdin);
            handle_deposit(json_input);
            break;
        case 3:
            printf("Enter account ID: ");
            fgets(json_input, sizeof(json_input), stdin);
            handle_withdraw(json_input);
            break;
        case 4:
            printf("Enter Account ID: ");
            fgets(json_input, sizeof(json_input), stdin);
            handle_get_balance(json_input);
            break;

        case 5:
            printf("Enter Account ID: ");
            fgets(json_input, sizeof(json_input), stdin);
            handle_close_account(json_input);
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