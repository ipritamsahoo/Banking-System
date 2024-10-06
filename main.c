#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100
#define MAX_REQUESTS 100
#define FILENAME "accounts.txt"
#define FILENAME_1 "requests.txt"

// Structure for account creation requests
struct AccountRequest
{
    int requestID;
    char name[100];
    char mobileNo[100];
    char aadharNo[100];
    char panNo[100];
    char status[10]; // "Pending", "Approved", "Rejected"
};

// Structure for active bank accounts
struct Account
{
    int accountNumber;
    char name[100];
    char mobileNo[100];
    char aadharNo[100];
    char panNo[100];
    float balance;
};

// Global lists for requests and approved accounts
struct AccountRequest accountRequests[MAX_REQUESTS];
struct Account accounts[MAX_ACCOUNTS];
int requestCount = 0;
int accountCount = 0;
int baseAccountNumber = 4690; // Base value for generating account numbers

// Function to save approved accounts to a text file
void saveAccountsToFile()
{
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL)
    {
        printf("Error opening file to save accounts!\n");
        return;
    }

    for (int i = 0; i < accountCount; i++)
    {
        fprintf(file, "%d %s %s %s %s %.2f\n", accounts[i].accountNumber, accounts[i].name, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo, accounts[i].balance);
    }

    fclose(file);
}

// Function to save requested accounts to a text file
void saveRequestedAccountsToFile()
{
    FILE *file = fopen(FILENAME_1, "w");
    if (file == NULL)
    {
        printf("Error opening file to save accounts!\n");
        return;
    }

    for (int i = 0; i < requestCount; i++)
    {
        fprintf(file, "%d %s %s %s %s %s\n", accountRequests[i].requestID, accountRequests[i].name, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo, accountRequests[i].status);
    }

    fclose(file);
}

void loadAccountsFromFile()
{
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL)
    {
        printf("No existing account file found, starting fresh.\n");
        return;
    }

    while (fscanf(file, "%d %s %s %s %s %f",
                  &accounts[accountCount].accountNumber,
                  accounts[accountCount].name,
                  accounts[accountCount].mobileNo,
                  accounts[accountCount].aadharNo,
                  accounts[accountCount].panNo,
                  &accounts[accountCount].balance) != EOF)
    {

        if (accounts[accountCount].accountNumber >= baseAccountNumber)
        {
            baseAccountNumber = accounts[accountCount].accountNumber + 1;
        }

        accountCount++;

        // Ensure we don't exceed the account limit
        if (accountCount >= MAX_ACCOUNTS)
        {
            printf("Account limit exceeded!\n");
            break;
        }
    }

    fclose(file);
}

// Function to load requested accounts from the text file
void loadRequestedAccountsFromFile()
{
    FILE *file = fopen(FILENAME_1, "r");
    if (file == NULL)
    {
        printf("No existing request file found, starting fresh.\n");
        return;
    }

    while (fscanf(file, "%d %s %s %s %s %s",
                  &accountRequests[requestCount].requestID,
                  accountRequests[requestCount].name,
                  accountRequests[requestCount].mobileNo,
                  accountRequests[requestCount].aadharNo,
                  accountRequests[requestCount].panNo,
                  accountRequests[requestCount].status) != EOF)
    {

        requestCount++;

        // Ensure we don't exceed the request limit
        if (requestCount >= MAX_REQUESTS)
        {
            printf("Request limit exceeded!\n");
            break;
        }
    }

    fclose(file);
}

// Function to request account creation (customer side)
void requestAccountCreation()
{
    if (requestCount >= MAX_REQUESTS)
    {
        printf("Maximum request limit reached!\n");
        return;
    }
    struct AccountRequest newRequest;

    newRequest.requestID = requestCount + 1; // Assigning unique request ID
    printf("Enter Your Name: ");
    scanf("%s", newRequest.name);

    printf("Enter Your Mobile No: ");
    scanf("%s", newRequest.mobileNo);

    printf("Enter Your Aadhar No: ");
    scanf("%s", newRequest.aadharNo);

    printf("Enter Your PAN No: ");
    scanf("%s", newRequest.panNo);

    strcpy(newRequest.status, "Pending"); // Initial status is pending

    // Save the request
    accountRequests[requestCount++] = newRequest;
    printf("Account Creation Request Submitted!\n\n");
}

// Function for admin to view all pending requests
void viewPendingRequests()
{
    printf("Pending Account Creation Requests:\nRequest ID\tName\tMobile No\tAadhar No\tPAN No\n");
    for (int i = 0; i < requestCount; i++)
    {
        if (strcmp(accountRequests[i].status, "Pending") == 0)
        {
            printf(" %d\t\t%s\t%s\t%s\t%s\n",
                   accountRequests[i].requestID, accountRequests[i].name, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo);
        }
    }
}

// Function for admin to approve or reject requests
void approveOrRejectRequest()
{
    int requestID, choice;
    printf("Enter Request ID to Approve/Reject: ");
    scanf("%d", &requestID);
    printf("Request Details:\nName\tMobile No\tAadhar No\tPAN No\n");
    for (int i = 0; i < requestCount; i++)
    {
        if (accountRequests[i].requestID == requestID && strcmp(accountRequests[i].status, "Pending") == 0)
        {
            printf("%s\t%s\t%s\t%s\n", accountRequests[i].name, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo);
            printf("1. Approve\n2. Reject\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Approve the request
                if (accountCount >= MAX_ACCOUNTS)
                {
                    printf("Maximum account limit reached!\n");
                    return;
                }

                struct Account newAccount;
                newAccount.accountNumber = baseAccountNumber++; // Auto-generated account number
                strcpy(newAccount.name, accountRequests[i].name);
                strcpy(newAccount.mobileNo, accountRequests[i].mobileNo);
                strcpy(newAccount.aadharNo, accountRequests[i].aadharNo);
                strcpy(newAccount.panNo, accountRequests[i].panNo);
                newAccount.balance = 0.0; // Initial balance

                // Add new account to the system
                accounts[accountCount++] = newAccount;

                // Save accounts to file
                saveAccountsToFile();

                strcpy(accountRequests[i].status, "Approved"); // Update request status
                printf("Account Request Approved\n\n");
            }
            else if (choice == 2)
            {
                strcpy(accountRequests[i].status, "Rejected"); // Update request status
                printf("Account Request Rejected!\n\n");
            }
            else
            {
                printf("Invalid choice!\n");
            }
            return;
        }
    }
    printf("Request ID not found or already processed!\n\n");
}

// Function to display all approved accounts
void displayAllAccounts()
{
    printf("Approved Accounts:\n\nAccount No\tName\tMobile No\tBalance\n");
    for (int i = 0; i < accountCount; i++)
    {
        printf("XXXXXXXX%d\t%s\t%s\t%.2f\n",
               accounts[i].accountNumber, accounts[i].name, accounts[i].mobileNo, accounts[i].balance);
    }
}

int main()
{
    int choice;

    loadAccountsFromFile();          // Load existing accounts from file
    loadRequestedAccountsFromFile(); // Load requested pending accounts from file

    while (1)
    {
        printf("1. Request Account Creation (Customer)\n");
        printf("2. View Pending Requests (Admin)\n");
        printf("3. Approve/Reject Requests (Admin)\n");
        printf("4. View All Accounts (Admin)\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            requestAccountCreation();
            break;
        case 2:
            viewPendingRequests();
            break;
        case 3:
            approveOrRejectRequest();
            break;
        case 4:
            displayAllAccounts();
            break;
        case 5:
            saveAccountsToFile();          // Save accounts before exiting
            saveRequestedAccountsToFile(); // Save requested accounts before exiting
            return 0;
        default:
            printf("Invalid choice! Please try again.\n\n");
        }
    }

    return 0;
}
