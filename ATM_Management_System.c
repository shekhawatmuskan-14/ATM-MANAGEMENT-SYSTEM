#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100
#define MAX_TRANSACTIONS 500
#define NAME_SIZE 50
#define PIN_SIZE 5
#define DAILY_LIMIT 20000.0
#define MIN_BALANCE 500.0

struct Account
{
    int accountNumber;
    char name[NAME_SIZE];
    char pin[PIN_SIZE]; // 4-digit PIN stored as text
    double balance;
    double withdrawnToday;
};

struct Transaction
{
    int transactionID;
    int accountNumber;
    char type[15]; // Deposit or Withdraw
    double amount;
    char date[20]; // Date entered by user (DD-MM-YYYY)
};

struct Account accounts[MAX_ACCOUNTS];
struct Transaction transactions[MAX_TRANSACTIONS];
int accountCount = 0;
int transactionCount = 0;
int nextTransactionID = 1001;

// Find account using account number (linear search).
int findAccount(int accountNumber)
{
    int i;
    for (i = 0; i < accountCount; i++)
    {
        if (accounts[i].accountNumber == accountNumber)
            return i;
    }
    return -1;
}

// Verify PIN before allowing account operations.
int verifyPIN(int index)
{
    char enteredPIN[PIN_SIZE];
    printf("Enter 4-digit PIN: ");
    scanf("%4s", enteredPIN);

    if (strcmp(accounts[index].pin, enteredPIN) == 0)
        return 1;

    printf("Incorrect PIN.\n");
    return 0;
}

// Store transaction details in an array.
void addTransaction(int accountNumber, char type[], double amount)
{
    if (transactionCount >= MAX_TRANSACTIONS)
    {
        printf("Transaction history is full.\n");
        return;
    }

    transactions[transactionCount].transactionID = nextTransactionID++;
    transactions[transactionCount].accountNumber = accountNumber;
    strcpy(transactions[transactionCount].type, type);
    transactions[transactionCount].amount = amount;

    printf("Enter date (DD-MM-YYYY): ");
    scanf("%19s", transactions[transactionCount].date);

    transactionCount++;
}

// MODULE 1: Create account and view/update account details.
void createAccount(void)
{
    struct Account newAccount;
    int i;

    if (accountCount >= MAX_ACCOUNTS)
    {
        printf("Account limit reached.\n");
        return;
    }

    printf("Enter account number: ");
    scanf("%d", &newAccount.accountNumber);

    if (findAccount(newAccount.accountNumber) != -1)
    {
        printf("Account number already exists.\n");
        return;
    }

    printf("Enter account holder name: ");
    scanf(" %49[^\n]", newAccount.name);

    printf("Set 4-digit PIN: ");
    scanf("%4s", newAccount.pin);
    if (strlen(newAccount.pin) != 4)
    {
        printf("PIN must contain exactly 4 digits. Account not created.\n");
        return;
    }
    for (i = 0; i < 4; i++)
    {
        if (newAccount.pin[i] < '0' || newAccount.pin[i] > '9')
        {
            printf("PIN must contain only digits. Account not created.n");
            return;
        }
    }

    printf("Enter opening balance (minimum Rs. %.2f): ", MIN_BALANCE);
    scanf("%lf", &newAccount.balance);

    if (newAccount.balance < MIN_BALANCE)
    {
        printf("Opening balance must be at least Rs. %.2f.\n", MIN_BALANCE);
        return;
    }

    newAccount.withdrawnToday = 0;
    accounts[accountCount++] = newAccount;
    printf("Account created successfully!\n");
}

void accountDetails(void)
{
    int accountNumber, index, choice;
    char newName[NAME_SIZE], newPIN[PIN_SIZE];
    int i;

    printf("Enter account number: ");
    scanf("%d", &accountNumber);

    index = findAccount(accountNumber);
    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }
    if (!verifyPIN(index))
        return;

    printf("\nAccount Number: %d\n", accounts[index].accountNumber);
    printf("Name          : %s\n", accounts[index].name);
    printf("Balance       : Rs. %.2f\n", accounts[index].balance);

    printf("1. Update name\n2. Change PIN\n0. Back\nChoose: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Enter new name: ");
        scanf(" %49[^\n]", newName);
        strcpy(accounts[index].name, newName);
        printf("Name updated successfully.\n");
    }
    else if (choice == 2)
    {
        printf("Enter new 4-digit PIN: ");
        scanf("%4s", newPIN);
        if (strlen(newPIN) != 4)
        {
            printf("PIN must contain exactly 4 digits.\n");
            return;
        }
        for (i = 0; i < 4; i++)
        {
            if (newPIN[i] < '0' || newPIN[i] > '9')
            {
                printf("PIN must contain only digits.\n");
                return;
            }
        }
        strcpy(accounts[index].pin, newPIN);
        printf("PIN changed successfully.\n");
    }
}

// MODULE 2: Balance inquiry and deposit.
void balanceAndDeposit(void)
{
    int accountNumber, index, choice;
    double amount;

    printf("Enter account number: ");
    scanf("%d", &accountNumber);

    index = findAccount(accountNumber);
    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }
    if (!verifyPIN(index))
        return;

    printf("Current balance: Rs. %.2f\n", accounts[index].balance);
    printf("1. Deposit cash\n0. Back\nChoose: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Enter deposit amount: Rs. ");
        scanf("%lf", &amount);

        if (amount <= 0)
        {
            printf("Amount must be greater than zero.\n");
            return;
        }

        accounts[index].balance += amount;
        printf("Deposit successful. New balance: Rs. %.2f\n",
               accounts[index].balance);
        addTransaction(accountNumber, "Deposit", amount);
    }
}

// MODULE 3: Cash withdrawal with minimum balance and daily limit.
void withdrawCash(void)
{
    int accountNumber, index;
    double amount;

    printf("Enter account number: ");
    scanf("%d", &accountNumber);

    index = findAccount(accountNumber);
    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }
    if (!verifyPIN(index))
        return;

    printf("Available balance: Rs. %.2f\n", accounts[index].balance);
    printf("Enter withdrawal amount: Rs. ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Amount must be greater than zero.\n");
    }
    else if (amount > accounts[index].balance - MIN_BALANCE)
    {
        printf("Insufficient funds! Minimum balance of Rs. %.2f must remain.\n",
               MIN_BALANCE);
    }
    else if (accounts[index].withdrawnToday + amount > DAILY_LIMIT)
    {
        printf("Daily withdrawal limit of Rs. %.2f exceeded.\n", DAILY_LIMIT);
    }
    else
    {
        accounts[index].balance -= amount;
        accounts[index].withdrawnToday += amount;
        printf("Withdrawal successful. Remaining balance: Rs. %.2f\n",
               accounts[index].balance);
        printf("Today's withdrawal total: Rs. %.2f\n",
               accounts[index].withdrawnToday);
        addTransaction(accountNumber, "Withdraw", amount);
    }
}

// MODULE 4: Display transaction history for an account.
void transactionHistory(void)
{
    int accountNumber, index, i, found = 0;

    printf("Enter account number: ");
    scanf("%d", &accountNumber);

    index = findAccount(accountNumber);
    if (index == -1)
    {
        printf("Account not found.\n");
        return;
    }
    if (!verifyPIN(index))
        return;

    printf("\n--- Transaction History ---\n");
    printf("ID\tType\t\tAmount\t\tDate\n");

    for (i = 0; i < transactionCount; i++)
    {
        if (transactions[i].accountNumber == accountNumber)
        {
            printf("%d\t%-10s\tRs. %.2f\t%s\n",
                   transactions[i].transactionID,
                   transactions[i].type,
                   transactions[i].amount,
                   transactions[i].date);
            found = 1;
        }
    }

    if (!found)
        printf("No transactions found.\n");
}

// MODULE 4: Sort transactions by amount (ascending).
void sortTransactionsByAmount(void)
{
    int i, j;
    struct Transaction temp;

    if (transactionCount == 0)
    {
        printf("No transactions to sort.\n");
        return;
    }

    for (i = 0; i < transactionCount - 1; i++)
    {
        for (j = 0; j < transactionCount - i - 1; j++)
        {
            if (transactions[j].amount > transactions[j + 1].amount)
            {
                temp = transactions[j];
                transactions[j] = transactions[j + 1];
                transactions[j + 1] = temp;
            }
        }
    }

    printf("\nTransactions sorted by amount (ascending):\n");
    for (i = 0; i < transactionCount; i++)
    {
        printf("ID: %d | Account: %d | %-10s | Rs. %.2f | %s\n",
               transactions[i].transactionID,
               transactions[i].accountNumber,
               transactions[i].type,
               transactions[i].amount,
               transactions[i].date);
    }
}

// MODULE 4: Search transaction by ID or type.
void searchTransaction(void)
{
    int choice, id, i, found = 0;
    char type[15];

    printf("Search by: 1. Transaction ID  2. Type\nChoose: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        printf("Enter transaction ID: ");
        scanf("%d", &id);
        for (i = 0; i < transactionCount; i++)
        {
            if (transactions[i].transactionID == id)
            {
                printf("ID: %d | Account: %d | %s | Rs. %.2f | %s\n",
                       transactions[i].transactionID,
                       transactions[i].accountNumber,
                       transactions[i].type,
                       transactions[i].amount,
                       transactions[i].date);
                found = 1;
            }
        }
    }
    else if (choice == 2)
    {
        printf("Enter type (Deposit/Withdraw): ");
        scanf("%14s", type);
        for (i = 0; i < transactionCount; i++)
        {
            if (strcmp(transactions[i].type, type) == 0)
            {
                printf("ID: %d | Account: %d | %s | Rs. %.2f | %s\n",
                       transactions[i].transactionID,
                       transactions[i].accountNumber,
                       transactions[i].type,
                       transactions[i].amount,
                       transactions[i].date);
                found = 1;
            }
        }
    }
    else
    {
        printf("Invalid choice.\n");
        return;
    }

    if (!found)
        printf("Transaction not found.\n");
}

int main(void)
{
    int choice;

    do
    {
        printf("\n========== ATM MANAGEMENT SYSTEM ==========");
        printf("\n1. Create Account");
        printf("\n2. View / Update Account Details");
        printf("\n3. Balance Inquiry / Deposit");
        printf("\n4. Cash Withdrawal");
        printf("\n5. Transaction History");
        printf("\n6. Sort Transactions by Amount");
        printf("\n7. Search Transaction");
        printf("\n0. Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createAccount();
            break;
        case 2:
            accountDetails();
            break;
        case 3:
            balanceAndDeposit();
            break;
        case 4:
            withdrawCash();
            break;
        case 5:
            transactionHistory();
            break;
        case 6:
            sortTransactionsByAmount();
            break;
        case 7:
            searchTransaction();
            break;
        case 0:
            printf("Thank you for using ATM Management System!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);

    return 0;
}
