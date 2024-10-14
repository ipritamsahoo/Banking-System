#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100
#define MAX_REQUESTS 100
#define FILENAME "accounts.txt"
#define FILENAME_1 "requests.txt"
#define ADMIN_PASSWORD "admin123" // Admin password

// Structure for account creation requests
struct AccountRequest
{
    int requestID;
    char name[100];
    char mobileNo[100];
    char aadharNo[100];
    char panNo[100];
    char password[50];
    char transactionPin[6]; 
    char status[10];   // "Pending", "Approved", "Rejected"
};

// Structure for active bank accounts
struct Account
{
    int accountNumber;
    char name[100];
    char mobileNo[100];
    char aadharNo[100];
    char panNo[100];
    char password[50]; 
    char transactionPin[6];
    float balance;
};


// Global lists for requests, approved accounts, and transactions
struct AccountRequest accountRequests[MAX_REQUESTS];
struct Account accounts[MAX_ACCOUNTS];
int requestCount = 0;
int accountCount = 0;
int baseAccountNumber = 4690; // Base value for generating account numbers

// Function prototypes
void customerMenu();
void customerPostLoginMenu(struct Account *loggedInCustomer);
void saveAccountsToFile();
void saveRequestedAccountsToFile();
void loadAccountsFromFile();
void loadRequestedAccountsFromFile();
int findAccountByMobile(char *mobileNo, char *password);

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
        fprintf(file, "%d %s %s %s %s %s %s %.2f\n", accounts[i].accountNumber, accounts[i].name, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo, accounts[i].password, accounts[i].transactionPin, accounts[i].balance);
    }

    fclose(file);
}

// Function to save requested accounts to a text file
void saveRequestedAccountsToFile()
{
    FILE *file = fopen(FILENAME_1, "w");
    if (file == NULL)
    {
        printf("Error opening file to save account requests!\n");
        return;
    }

    for (int i = 0; i < requestCount; i++)
    {
        fprintf(file, "%d %s %s %s %s %s %s %s\n", accountRequests[i].requestID, accountRequests[i].name, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo, accountRequests[i].password, accountRequests[i].transactionPin, accountRequests[i].status);
    }

    fclose(file);
}

// Function to load accounts from the text file
void loadAccountsFromFile()
{
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL)
    {
        printf("No existing account file found, starting fresh.\n");
        return;
    }

    while (fscanf(file, "%d %s %s %s %s %s %s %f", &accounts[accountCount].accountNumber, accounts[accountCount].name, accounts[accountCount].mobileNo, accounts[accountCount].aadharNo, accounts[accountCount].panNo, accounts[accountCount].password, accounts[accountCount].transactionPin, &accounts[accountCount].balance) != EOF)
    {
        if (accounts[accountCount].accountNumber >= baseAccountNumber)
        {
            baseAccountNumber = accounts[accountCount].accountNumber + 1;
        }
        accountCount++;
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

    while (fscanf(file, "%d %s %s %s %s %s %s %s", &accountRequests[requestCount].requestID, accountRequests[requestCount].name, accountRequests[requestCount].mobileNo, accountRequests[requestCount].aadharNo, accountRequests[requestCount].panNo, accountRequests[requestCount].password, accountRequests[requestCount].transactionPin, accountRequests[requestCount].status) != EOF)
    {
        requestCount++;
    }

    fclose(file);
}

// Function to request account creation (customer side) with password and PIN confirmation
void requestAccountCreation() {
    struct AccountRequest newRequest;
    char confirmPassword[50];   // For confirming the password
    char transactionPin[6];     // For storing the transaction PIN
    char confirmTransactionPin[6];  // For confirming the transaction PIN

    newRequest.requestID = requestCount + 1; // Assigning unique request ID
    printf("Enter Your Name: ");
    scanf("%s", newRequest.name);

    printf("Enter Your Mobile No: ");
    scanf("%s", newRequest.mobileNo);

    printf("Enter Your Aadhar No: ");
    scanf("%s", newRequest.aadharNo);

    printf("Enter Your PAN No: ");
    scanf("%s", newRequest.panNo);

    // Password confirmation loop
    while (1) {
        printf("Set a Password for Your Account: ");
        scanf("%s", newRequest.password);

        printf("Confirm Your Password: ");
        scanf("%s", confirmPassword);

        if (strcmp(newRequest.password, confirmPassword) == 0) {
            break;  // Passwords match, proceed
        } else {
            printf("Passwords do not match! Please retype your password.\n");
        }
    }

    // Transaction PIN confirmation loop
    while (1) {
        printf("Set a 4-digit Transaction PIN for Your Account: ");
        scanf("%s", transactionPin);

        if (strlen(transactionPin) != 4) {
            printf("Invalid PIN! Please enter a 4-digit PIN.\n");
            continue;
        }

        printf("Confirm Your Transaction PIN: ");
        scanf("%s", confirmTransactionPin);

        if (strcmp(transactionPin, confirmTransactionPin) == 0) {
            strcpy(newRequest.transactionPin, transactionPin);  // Store the confirmed PIN
            break;  // PINs match, proceed
        } else {
            printf("Transaction PINs do not match! Please retype your PIN.\n");
        }
    }

    strcpy(newRequest.status, "Pending");  // Initial status is pending
    
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
                strcpy(newAccount.password, accountRequests[i].password);
                strcpy(newAccount.transactionPin, accountRequests[i].transactionPin);
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
    printf("Approved Accounts:\n\nAccount No\tName\tMobile No\tAadhar No\tPAN No\n");
    for (int i = 0; i < accountCount; i++)
    {
        printf("XXXXXXXX%d\t%s\t%s\t%s\t%s\n",
               accounts[i].accountNumber, accounts[i].name, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo);
    }
}

// Function for admin menu
void adminMenu()
{
    int choice;
    while (1)
    {
        printf("Admin Menu:\n");
        printf("1. View Pending Requests\n");
        printf("2. Approve/Reject Requests\n");
        printf("3. View All Accounts\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            viewPendingRequests();
            break;
        case 2:
            approveOrRejectRequest();
            break;
        case 3:
            displayAllAccounts();
            break;
        case 4:
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

// Function for customer login
int customerLogin()
{
    char mobileNo[100], password[50];

    printf("Enter Your Mobile Number: ");
    scanf("%s", mobileNo);
    printf("Enter Your Password: ");
    scanf("%s", password);

    int index = findAccountByMobile(mobileNo, password);
    if (index == -1)
    {
        printf("Invalid mobile number or password! Try again.\n");
        return 0;
    }
    else
    {
        printf("Login successful!\n");
        customerPostLoginMenu(&accounts[index]);
        return 1;
    }
}

// Function to find an account by mobile number and password
int findAccountByMobile(char *mobileNo, char *password)
{
    for (int i = 0; i < accountCount; i++)
    {
        if (strcmp(accounts[i].mobileNo, mobileNo) == 0 && strcmp(accounts[i].password, password) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Function for post-login customer menu
void customerPostLoginMenu(struct Account *loggedInCustomer)
{
    int choice;
    while (1)
    {
        printf("\nCustomer Menu:\n");
        printf("1. View Account Details\n");
        printf("2. Check Balance\n");
        printf("3. Money Transfer\n");
        printf("4. View Transactions\n");
        printf("5. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Account Number: XXXXXXXX%d\nName: %s\nMobile No: %s\n",
                   loggedInCustomer->accountNumber, loggedInCustomer->name, loggedInCustomer->mobileNo);
            break;
        case 2:
            printf("Current Balance: %.2f\n", loggedInCustomer->balance);
            break;
        case 3:
            // Implement money transfer logic 
            break;
        case 4:
            // Implement viewing transactions logic 
            break;
        case 5:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

// Function for customer menu (initial options)
void customerMenu()
{
    int choice;
    while (1)
    {
        printf("\nCustomer Menu:\n");
        printf("1. Request Account Creation\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            requestAccountCreation();
            break;
        case 2:
            if (customerLogin())
            {
                // Successful login, proceed to post-login menu
            }
            break;
        case 3:
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

// Main function with role selection
int main()
{
    loadAccountsFromFile();          // Load existing accounts
    loadRequestedAccountsFromFile(); // Load existing account requests

    int userType;
    printf("Welcome to the Bank Management System!\n");
    printf("Select your role:\n1. Customer\n2. Admin\nEnter your choice: ");
    scanf("%d", &userType);

    if (userType == 1)
    {
        // Customer Menu
        customerMenu();
    }
    else if (userType == 2)
    {
        // Admin Menu with password protection
        char password[20];
        printf("Enter Admin Password: ");
        scanf("%s", password);

        if (strcmp(password, ADMIN_PASSWORD) == 0)
        {
            adminMenu();
        }
        else
        {
            printf("Incorrect password! Access denied.\n");
        }
    }
    else
    {
        printf("Invalid selection!\n");
    }

    // Save all changes to files before exiting
    saveAccountsToFile();
    saveRequestedAccountsToFile();

    return 0;
}
