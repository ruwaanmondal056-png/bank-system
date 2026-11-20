#include <stdio.h>

#define ACCOUNT_FILE "bank_account.dat"
#define TEMP_FILE "bank_account.tmp"

typedef struct {
    int account_number;
    char account_holder[100];
    double balance;
} Account;

static int find_account(int number, Account *found)
{
    FILE *file = fopen(ACCOUNT_FILE, "rb");
    Account account;

    if (file == NULL) return 0;
    while (fread(&account, sizeof account, 1, file) == 1) {
        if (account.account_number == number) {
            if (found != NULL) *found = account;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

static void add_member(void)
{
    Account account;
    FILE *file;

    printf("Enter account number: ");
    if (scanf("%d", &account.account_number) != 1 || account.account_number <= 0) {
        printf("Account number must be a positive integer.\n");
        return;
    }
    if (find_account(account.account_number, NULL)) {
        printf("An account with that number already exists.\n");
        return;
    }
    printf("Enter account holder name: ");
    if (scanf(" %99[^\n]", account.account_holder) != 1) {
        printf("Invalid name.\n");
        return;
    }
    account.balance = 0.0;

    file = fopen(ACCOUNT_FILE, "ab");
    if (file == NULL) {
        perror("Could not open account data file");
        return;
    }
    if (fwrite(&account, sizeof account, 1, file) != 1) {
        perror("Could not save account");
    } else {
        printf("Member added with a starting balance of 0.00.\n");
    }
    fclose(file);
}

static int update_account(const Account *updated)
{
    FILE *input = fopen(ACCOUNT_FILE, "rb");
    FILE *output;
    Account account;
    int found = 0;

    if (input == NULL) return 0;
    output = fopen(TEMP_FILE, "wb");
    if (output == NULL) {
        perror("Could not create temporary account file");
        fclose(input);
        return 0;
    }

    while (fread(&account, sizeof account, 1, input) == 1) {
        if (account.account_number == updated->account_number) {
            account = *updated;
            found = 1;
        }
        if (fwrite(&account, sizeof account, 1, output) != 1) {
            perror("Could not write account data");
            fclose(input);
            fclose(output);
            remove(TEMP_FILE);
            return 0;
        }
    }
    fclose(input);
    if (fclose(output) != 0 || !found) {
        remove(TEMP_FILE);
        return 0;
    }
    if (remove(ACCOUNT_FILE) != 0 || rename(TEMP_FILE, ACCOUNT_FILE) != 0) {
        perror("Could not replace account data file");
        return 0;
    }
    return 1;
}

static int select_account(Account *account)
{
    int number;
    printf("Enter account number: ");
    if (scanf("%d", &number) != 1 || number <= 0) {
        printf("Account number must be a positive integer.\n");
        return 0;
    }
    if (!find_account(number, account)) {
        printf("Account not found. Add the member first.\n");
        return 0;
    }
    return 1;
}

static void deposit(void)
{
    Account account;
    double amount;
    if (!select_account(&account)) return;
    printf("Enter deposit amount: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0.0) {
        printf("Enter a valid amount greater than zero.\n");
        return;
    }
    account.balance += amount;
    if (update_account(&account))
        printf("Deposit successful. New balance: %.2f\n", account.balance);
}

static void withdraw(void)
{
    Account account;
    double amount;
    if (!select_account(&account)) return;
    printf("Enter withdrawal amount: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0.0) {
        printf("Enter a valid amount greater than zero.\n");
        return;
    }
    if (amount > account.balance) {
        printf("Insufficient funds. Current balance: %.2f\n", account.balance);
        return;
    }
    account.balance -= amount;
    if (update_account(&account))
        printf("Withdrawal successful. New balance: %.2f\n", account.balance);
}

static void show_balance(void)
{
    Account account;
    if (!select_account(&account)) return;
    printf("Account number: %d\n", account.account_number);
    printf("Account holder: %s\n", account.account_holder);
    printf("Current balance: %.2f\n", account.balance);
}

int main(void)
{
    int choice;
    do {
        printf("\n=== Bank Account Management System ===\n");
        printf("1. Add member/account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Balance enquiry\n");
        printf("5. Exit\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            return 1;
        }
        switch (choice) {
            case 1: add_member(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: show_balance(); break;
            case 5: printf("Goodbye.\n"); break;
            default: printf("Choose a number from 1 to 5.\n");
        }
    } while (choice != 5);
    return 0;
}
