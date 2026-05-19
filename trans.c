// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
}; // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listRecords(FILE *fPtr);
void searchByAccount(FILE *fPtr);
void searchByLastName(FILE *fPtr);
void accountSummary(FILE *fPtr);
FILE *openCreditFile(const char *fileName);
void flushInput(void);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    cfPtr = openCreditFile("credit.dat");
    if (cfPtr == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            listRecords(cfPtr);
            break;
        case 6:
            searchByAccount(cfPtr);
            break;
        case 7:
            searchByLastName(cfPtr);
            break;
        case 8:
            accountSummary(cfPtr);
            break;
        default:
            puts("Incorrect choice. Enter a number between 1 and 9.");
            break;
        }
    }

    fclose(cfPtr); // fclose closes the file
    puts("Program ended.");
    return 0;
} // end main

FILE *openCreditFile(const char *fileName)
{
    FILE *fPtr = fopen(fileName, "rb+");
    if (fPtr == NULL)
    {
        fPtr = fopen(fileName, "wb+");
        if (fPtr == NULL)
        {
            return NULL;
        }

        struct clientData blankClient = {0, "", "", 0.0};
        for (unsigned int i = 0; i < 100; ++i)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        }
        fflush(fPtr);
        rewind(fPtr);
    }
    return fPtr;
}

void flushInput(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - list all accounts\n"
           "6 - search account by number\n"
           "7 - search accounts by last name\n"
           "8 - show account summary\n"
           "9 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        puts("Invalid input. Please enter a number.");
        flushInput();
        return 0;
    }
    flushInput();
    return menuChoice;
}

void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("Could not open accounts.txt for writing.");
    }
    else
    {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                        client.acctNum, client.lastName, client.firstName, client.balance);
            }
        }
        fclose(writePtr);
        puts("accounts.txt generated successfully.");
    }
}

void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    if (scanf("%u", &account) != 1 || account < 1 || account > 100)
    {
        puts("Invalid account number.");
        flushInput();
        return;
    }
    flushInput();

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("Current record:\n%-6u%-16s%-11s%10.2f\n",
           client.acctNum, client.lastName, client.firstName, client.balance);
    printf("Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1)
    {
        puts("Invalid transaction amount.");
        flushInput();
        return;
    }
    flushInput();

    client.balance += transaction;
    printf("Updated record:\n%-6u%-16s%-11s%10.2f\n",
           client.acctNum, client.lastName, client.firstName, client.balance);

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    fflush(fPtr);
}

void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        flushInput();
        return;
    }
    flushInput();

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    fflush(fPtr);
    printf("Account %u deleted.\n", accountNum);
}

void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        flushInput();
        return;
    }
    flushInput();

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        puts("Invalid input. Record not created.");
        flushInput();
        return;
    }
    flushInput();

    client.acctNum = accountNum;
    fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    fflush(fPtr);
    printf("Account #%u created successfully.\n", accountNum);
}

void listRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int count = 0;

    rewind(fPtr);
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum, client.lastName, client.firstName, client.balance);
            ++count;
        }
    }
    if (count == 0)
    {
        puts("No active accounts found.");
    }
    else
    {
        printf("\nTotal active accounts: %u\n", count);
    }
}

void searchByAccount(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter account number to search (1 - 100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum < 1 || accountNum > 100)
    {
        puts("Invalid account number.");
        flushInput();
        return;
    }
    flushInput();

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account #%u not found.\n", accountNum);
    }
    else
    {
        printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
        printf("%-6u%-16s%-11s%10.2f\n",
               client.acctNum, client.lastName, client.firstName, client.balance);
    }
}

void searchByLastName(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    char searchName[15];
    char lowerSearch[15];
    unsigned int count = 0;

    printf("Enter last name to search: ");
    if (scanf("%14s", searchName) != 1)
    {
        puts("Invalid input.");
        flushInput();
        return;
    }
    flushInput();

    for (unsigned int i = 0; searchName[i] != '\0' && i < sizeof(searchName) - 1; ++i)
    {
        lowerSearch[i] = (char)tolower((unsigned char)searchName[i]);
        lowerSearch[i + 1] = '\0';
    }

    rewind(fPtr);
    printf("%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        char lowerLast[15] = "";
        for (unsigned int i = 0; client.lastName[i] != '\0' && i < sizeof(client.lastName) - 1; ++i)
        {
            lowerLast[i] = (char)tolower((unsigned char)client.lastName[i]);
            lowerLast[i + 1] = '\0';
        }
        if (client.acctNum != 0 && strcmp(lowerLast, lowerSearch) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum, client.lastName, client.firstName, client.balance);
            ++count;
        }
    }
    if (count == 0)
    {
        printf("No accounts found with last name '%s'.\n", searchName);
    }
    else
    {
        printf("\nFound %u matching account(s).\n", count);
    }
}

void accountSummary(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int activeCount = 0;
    double totalBalance = 0.0;
    double minBalance = 0.0;
    double maxBalance = 0.0;

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            if (activeCount == 0)
            {
                minBalance = maxBalance = client.balance;
            }
            else
            {
                if (client.balance < minBalance)
                {
                    minBalance = client.balance;
                }
                if (client.balance > maxBalance)
                {
                    maxBalance = client.balance;
                }
            }
            totalBalance += client.balance;
            ++activeCount;
        }
    }

    printf("Account summary:\n");
    printf("Active accounts: %u\n", activeCount);
    printf("Total balance: %.2f\n", totalBalance);
    if (activeCount > 0)
    {
        printf("Average balance: %.2f\n", totalBalance / activeCount);
        printf("Lowest balance: %.2f\n", minBalance);
        printf("Highest balance: %.2f\n", maxBalance);
    }
}
