#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <conio.h>

#define MAX_ACCOUNTS 100
#define MAX_REQUESTS 100
#define MAX_TITLE_LENGTH 100
#define MAX_ADMINS 10
#define FILENAME "accounts.txt"
#define FILENAME_1 "accountRequests.txt"
#define FILENAME_CARDS "cards.txt"
#define FILENAME_REQUESTS "cardRequests.txt"

// Define color codes
#define RED "\x1b[1;31m"
#define GREEN "\x1b[1;32m"
#define YELLOW "\x1b[1;33m"
#define BLUE "\x1b[1;34m"
#define MAGENTA "\x1b[1;35m"
#define CYAN "\x1b[1;36m"
#define RESET "\x1b[0m"

// Structure for account creation requests
struct AccountRequest
{
    int requestID;
    char firstName[100];
    char middleName[100];
    char lastName[100];
    char mobileNo[100];
    char aadharNo[100];
    char panNo[100];
    char password[50];
    char transactionPin[6];
    char status[10]; // "Pending", "Approved", "Rejected"
    int securityQuestion;
    char securityAnswer[100];
};

// Structure for active bank accounts
struct Account
{
    int accountNumber;
    char firstName[100];
    char middleName[100];
    char lastName[100];
    char mobileNo[100];
    char aadharNo[100];
    char panNo[100];
    char password[50];
    char transactionPin[6];
    float balance;
    char status[10]; // "Active", "Frozen"
    int securityQuestion;
    char securityAnswer[100];
};

// Structure for transactions
struct Transaction
{
    int senderAccountNumber;
    int receiverAccountNumber;
    float amount;
};

// Structure for card requests
struct CardRequest
{
    int requestID;
    int accountNumber; // The account number the card is linked to
    char cardType[10]; // "Debit" or "Credit"
    char status[10];   // "Pending", "Approved", "Rejected"
};

// Structure for bank cards (debit/credit)
struct Card
{
    int cardNumber;
    int cvv;
    int accountNumber;   // Linked to a bank account
    char cardType[10];   // "Debit" or "Credit"
    char cardPin[6];     // 4-digit PIN
    char cardStatus[10]; // "Active", "Blocked"
};

typedef struct
{
    char username[MAX_TITLE_LENGTH];
    char password[MAX_TITLE_LENGTH];
} Admin;

// Global lists for requests, approved accounts, transactions, card requests and approved cards
struct AccountRequest accountRequests[MAX_REQUESTS];
struct Account accounts[MAX_ACCOUNTS];
struct Transaction transactions[MAX_REQUESTS];
struct CardRequest cardRequests[MAX_REQUESTS];
struct Card cards[MAX_ACCOUNTS];
int requestCount = 0;
int accountCount = 0;
int transactionCount = 0;
int baseAccountNumber = 4690; // Base value for generating account numbers
int baseCardNumber = 5247;
int baseCVV = 694;
int cardRequestCount = 0;
int cardCount = 0;

// Function prototypes
void customerMenu();
void adminMenu();
void customerPostLoginMenu(struct Account *loggedInCustomer);
void saveAccountsToFile();
void saveRequestedAccountsToFile();
void loadAccountsFromFile();
void loadRequestedAccountsFromFile();
void loadTransactionsFromFile();
void saveTransactionsToFile();
int viewPendingRequests();
int displayAllAccounts();
int findAccountByPassword(char *password);
void transferMoney(struct Account *loggedInCustomer);
void requestCard(struct Account *loggedInCustomer);
void viewPendingCardRequests();
void approveOrRejectCardRequest();
void generateCardDetails(struct Card *newCard);
void customerCardManagement(struct Account *loggedInCustomer);
void changeCardPin(struct Card *card);
void blockOrUnblockCard(struct Card *card);
void loadCardRequests();
void saveCardRequests();
void loadCards();
void saveCards();
void deleteAccount();
void freezeOrUnfreezeAccount();
void viewPendingUnfreezeRequests();
void removeUnfreezeRequest(int accountNumber);
int findAccountByMobileOnly(const char *mobileNo);
int displaySecurityQuestion(int);
void updateProfile(struct Account *loggedInCustomer);
int readCredentials(Admin admins[], int maxAdmins);
int isValidPANFormat(char *pan);
void toUpperCase(char *str);
void clearScreen();
void readPassword(char *password, int maxLength);

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
        fprintf(file, "%d %s %s %s %s %s %s %s %s %s %.2f %d %s\n", accounts[i].accountNumber, accounts[i].firstName, (strlen(accounts[i].middleName) == 0) ? "NULL" : accounts[i].middleName, accounts[i].lastName, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo, accounts[i].password, accounts[i].transactionPin, accounts[i].status, accounts[i].balance, accounts[i].securityQuestion, accounts[i].securityAnswer);
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
        fprintf(file, "%d %s %s %s %s %s %s %s %s %s %d %s\n", accountRequests[i].requestID, accountRequests[i].firstName, (strlen(accountRequests[i].middleName) == 0) ? "NULL" : accountRequests[i].middleName, accountRequests[i].lastName, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo, accountRequests[i].password, accountRequests[i].transactionPin, accountRequests[i].status, accountRequests[i].securityQuestion, accountRequests[i].securityAnswer);
    }

    fclose(file);
}

// Function to save transactions to a text file
void saveTransactionsToFile()
{
    FILE *file = fopen("transactions.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file to save transactions!\n");
        return;
    }

    for (int i = 0; i < transactionCount; i++)
    {
        fprintf(file, "%d %d %.2f\n", transactions[i].senderAccountNumber, transactions[i].receiverAccountNumber, transactions[i].amount);
    }

    fclose(file);
}

// Function to load transactions from a text file
void loadTransactionsFromFile()
{
    FILE *file = fopen("transactions.txt", "r");
    if (file == NULL)
    {
        printf("No existing transaction file found, starting fresh.\n");
        return;
    }

    while (fscanf(file, "%d %d %f", &transactions[transactionCount].senderAccountNumber, &transactions[transactionCount].receiverAccountNumber, &transactions[transactionCount].amount) != EOF)
    {
        transactionCount++;
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

    while (fscanf(file, "%d %s %s %s %s %s %s %s %s %s %f %d %s", &accounts[accountCount].accountNumber, accounts[accountCount].firstName, accounts[accountCount].middleName, accounts[accountCount].lastName, accounts[accountCount].mobileNo, accounts[accountCount].aadharNo, accounts[accountCount].panNo, accounts[accountCount].password, accounts[accountCount].transactionPin, &accounts[accountCount].status, &accounts[accountCount].balance, &accounts[accountCount].securityQuestion, accounts[accountCount].securityAnswer) != EOF)
    {
        // Check if the middle name is the placeholder "NULL"
        if (strcmp(accounts[accountCount].middleName, "NULL") == 0)
        {
            accounts[accountCount].middleName[0] = '\0'; // Set to empty string
        }

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

    while (fscanf(file, "%d %s %s %s %s %s %s %s %s %s %d %s", &accountRequests[requestCount].requestID, accountRequests[requestCount].firstName, accountRequests[requestCount].middleName, accountRequests[requestCount].lastName, accountRequests[requestCount].mobileNo, accountRequests[requestCount].aadharNo, accountRequests[requestCount].panNo, accountRequests[requestCount].password, accountRequests[requestCount].transactionPin, accountRequests[requestCount].status, &accountRequests[requestCount].securityQuestion, accountRequests[requestCount].securityAnswer) != EOF)
    {
        // Check if the middle name is the placeholder "NULL"
        if (strcmp(accountRequests[requestCount].middleName, "NULL") == 0)
        {
            accountRequests[requestCount].middleName[0] = '\0'; // Set to empty string
        }
        requestCount++;
    }

    fclose(file);
}

// Function to load card requests from the file
void loadCardRequests()
{
    FILE *file = fopen(FILENAME_REQUESTS, "r");
    if (file == NULL)
    {
        printf("No card requests found.\n");
        return;
    }

    cardRequestCount = 0;
    while (fscanf(file, "%d %d %s %s", &cardRequests[cardRequestCount].requestID, &cardRequests[cardRequestCount].accountNumber, cardRequests[cardRequestCount].cardType, cardRequests[cardRequestCount].status) != EOF)
    {
        cardRequestCount++;
    }

    fclose(file);
}

// Function to save card requests to the file
void saveCardRequests()
{
    FILE *file = fopen(FILENAME_REQUESTS, "w");
    if (file == NULL)
    {
        printf("Error saving card requests.\n");
        return;
    }

    for (int i = 0; i < cardRequestCount; i++)
    {
        fprintf(file, "%d %d %s %s\n", cardRequests[i].requestID, cardRequests[i].accountNumber, cardRequests[i].cardType, cardRequests[i].status);
    }

    fclose(file);
}

// Function to load cards (approved) from the file
void loadCards()
{
    FILE *file = fopen(FILENAME_CARDS, "r");
    if (file == NULL)
    {
        printf("No cards found.\n");
        return;
    }

    cardCount = 0;
    while (fscanf(file, "%d %d %d %s %s %s", &cards[cardCount].cardNumber, &cards[cardCount].cvv, &cards[cardCount].accountNumber, cards[cardCount].cardType, cards[cardCount].cardPin, cards[cardCount].cardStatus) != EOF)
    {
        cardCount++;
    }

    fclose(file);
}

// Function to save cards (approved) to the file
void saveCards()
{
    FILE *file = fopen(FILENAME_CARDS, "w");
    if (file == NULL)
    {
        printf("Error saving cards.\n");
        return;
    }

    for (int i = 0; i < cardCount; i++)
    {
        fprintf(file, "%d %d %d %s %s %s\n", cards[i].cardNumber, cards[i].cvv, cards[i].accountNumber, cards[i].cardType, cards[i].cardPin, cards[i].cardStatus);
    }

    fclose(file);
}

// Function to read admin credentials from a file
int readCredentials(Admin admins[], int maxAdmins)
{
    FILE *file = fopen("config.txt", "r");
    if (file == NULL)
    {
        printf("Error opening config file.\n");
        return 0;
    }

    int count = 0;
    while (count < maxAdmins && fscanf(file, "ABCDE=%s\nEDCBA=%s\n", admins[count].username, admins[count].password) == 2)
    {
        count++;
    }

    fclose(file);
    return count; // Return the number of admins read
}

// Function to request account creation (customer side)
void requestAccountCreation()
{
    struct AccountRequest newRequest;
    char confirmPassword[50];      // For confirming the password
    char transactionPin[6];        // For storing the transaction PIN
    char confirmTransactionPin[6]; // For confirming the transaction PIN
    int securityQuestionChoice;    // Choice of security question
    int choice;
    system("cls");

    printf("\n\033[1;34m===== Application For Account Opening =====\033[0m\n\n");

    newRequest.requestID = requestCount + 1; // Assigning unique request ID
    printf("\033[1;36mEnter Your First Name: \033[0m");
    scanf("%s", newRequest.firstName);

    printf("\033[1;36mHave you any middle name? \033[0m\n");
    printf("\033[1;36m1. Yes \033[0m\n");
    printf("\033[1;36m2. No \033[0m\n");
    printf("\033[1;36mEnter your choice: \033[0m");
    scanf("%d", &choice);
    if (choice == 1)
    {
        printf("\033[1;36mEnter Your Middle Name: \033[0m");
        scanf("%s", newRequest.middleName);
    }
    else
    {
        newRequest.middleName[0] = '\0';
    }

    printf("\033[1;36mEnter Your Last Name: \033[0m");
    scanf("%s", newRequest.lastName);

    while (1)
    {
        printf("\033[1;36mEnter Your Mobile No : \033[0m");
        scanf("%s", newRequest.mobileNo);

        if (strlen(newRequest.mobileNo) == 10 && strspn(newRequest.mobileNo, "0123456789") == 10)
        {
            int matchMobile = 0;
            for (int i = 0; i < accountCount; i++)
            {
                if (strcmp(accounts[i].mobileNo, newRequest.mobileNo) == 0)
                {
                    matchMobile = 1; // Return the index if a match is found
                }
            }

            for (int i = 0; i < requestCount; i++)
            {
                if (strcmp(accountRequests[i].mobileNo, newRequest.mobileNo) == 0)
                {
                    matchMobile = 1; // Return the index if a match is found
                }
            }

            if (matchMobile)
            {
                printf("\033[1;31mInvalid Mobile Number! Please Enter a Valid Mobile Number.\033[0m\n");
            }
            else
            {
                break;
            }
        }
        else
        {
            printf("\033[1;31mInvalid Mobile Number! Please enter exactly 10 digits.\033[0m\n");
        }
    }

    while (1)
    {
        printf("\033[1;36mEnter Your Aadhar No : \033[0m");
        scanf("%s", newRequest.aadharNo);

        if (strlen(newRequest.aadharNo) == 12 && strspn(newRequest.aadharNo, "0123456789") == 12)
        {
            printf("\033[1;32mAadhar Number is valid and confirmed!\033[0m\n");
            break;
        }
        else
        {
            printf("\033[1;31mInvalid Aadhar Number! Please enter exactly 12 digits.\033[0m\n");
        }
    }

    while (1)
    {
        printf("\033[1;36mEnter Your PAN No : \033[0m");
        scanf("%s", newRequest.panNo);

        // Convert input to uppercase
        toUpperCase(newRequest.panNo);

        // Check if PAN format is valid
        if (isValidPANFormat(newRequest.panNo))
        {
            printf("\033[1;32mPAN Number is valid and confirmed!\033[0m\n");
            break;
        }
        else
        {
            printf("\033[1;31mInvalid PAN Number! Please follow the format: 5 letters, 4 digits, 1 letter.\033[0m\n");
        }
    }

    // // Password confirmation loop
    // while (1)
    // {
    //     printf("\033[1;36mSet a Password for Your Account: \033[0m");
    //     scanf("%s", newRequest.password);

    //     printf("\033[1;36mConfirm Your Password: \033[0m");
    //     scanf("%s", confirmPassword);

    //     if (strcmp(newRequest.password, confirmPassword) == 0)
    //     {
    //         printf("\033[1;32mPassword confirmed!\033[0m\n");
    //         break; // Passwords match, proceed
    //     }
    //     else
    //     {
    //         printf("\033[1;31mPasswords do not match! Please retype your password.\033[0m\n");
    //     }
    // }

    while (1)
    {
        printf("\033[1;36mSet a Password for Your Account: \033[0m");
        readPassword(newRequest.password, sizeof(newRequest.password));

        printf("\033[1;36mConfirm Your Password: \033[0m");
        readPassword(confirmPassword, sizeof(confirmPassword));

        if (strcmp(newRequest.password, confirmPassword) == 0)
        {
            printf("\033[1;32mPassword confirmed!\033[0m\n");
            break; // Passwords match, proceed
        }
        else
        {
            printf("\033[1;31mPasswords do not match! Please retype your password.\033[0m\n");
        }
    }

    // // Transaction PIN confirmation loop
    // while (1)
    // {
    //     printf("\033[1;36mSet a 4-digit Transaction PIN for Your Account: \033[0m");
    //     scanf("%s", transactionPin);

    //     if (strlen(transactionPin) != 4)
    //     {
    //         printf("\033[1;31mInvalid PIN! Please enter a 4-digit PIN.\033[0m\n");
    //         continue;
    //     }

    //     printf("\033[1;36mConfirm Your Transaction PIN: \033[0m");
    //     scanf("%s", confirmTransactionPin);

    //     if (strcmp(transactionPin, confirmTransactionPin) == 0)
    //     {
    //         strcpy(newRequest.transactionPin, transactionPin); // Store the confirmed PIN
    //         printf("\033[1;32mTransaction PIN confirmed!\033[0m\n");
    //         break; // PINs match, proceed
    //     }
    //     else
    //     {
    //         printf("\033[1;31mTransaction PINs do not match! Please retype your PIN.\033[0m\n");
    //     }
    // }

    // Transaction PIN confirmation loop
    while (1)
    {
        printf("\033[1;36mSet a 4-digit Transaction PIN for Your Account: \033[0m");
        readPassword(transactionPin, sizeof(transactionPin));

        if (strlen(transactionPin) != 4)
        {
            printf("\033[1;31mInvalid PIN! Please enter a 4-digit PIN.\033[0m\n");
            continue;
        }

        printf("\033[1;36mConfirm Your Transaction PIN: \033[0m");
        readPassword(confirmTransactionPin, sizeof(confirmTransactionPin));

        if (strcmp(transactionPin, confirmTransactionPin) == 0)
        {
            strcpy(newRequest.transactionPin, transactionPin); // Store the confirmed PIN
            printf("\033[1;32mTransaction PIN confirmed!\033[0m\n");
            break; // PINs match, proceed
        }
        else
        {
            printf("\033[1;31mTransaction PINs do not match! Please retype your PIN.\033[0m\n");
        }
    }
    // Security question section
    printf("\n\033[1;36m--------------------------------------------\033[0m\n");
    printf("\033[1;36m|        Account Recovery Setup            |\033[0m\n");
    printf("\033[1;36m--------------------------------------------\033[0m\n");

    printf("\n\033[1mPlease select a security question to help recover your account.\033[0m\n");
    printf("\033[1;32mMake sure to choose something memorable.\033[0m\n\n");
    printf("\033[1;36m===========================================================\033[0m\n");
    printf("\033[1;33m1. What was the name of your first pet?\033[0m\n");
    printf("\033[1;33m2. What is the name of the city where you were born?\033[0m\n");
    printf("\033[1;33m3. Who is your favorite singer?\033[0m\n");
    printf("\033[1;33m4. What color do you like the most?\033[0m\n");
    printf("\033[1;33m5. What is your favorite book?\033[0m\n");
    printf("\033[1;33m6. Which teacher did you like the most at school?\033[0m\n");
    printf("\033[1;33m7. What is your favorite sport?\033[0m\n");
    printf("\033[1;33m8. Who is your favorite actor?\033[0m\n");
    printf("\033[1;36m===========================================================\033[0m\n");
    printf("\n\033[1;34mEnter the number of your choice: \033[0m");
    scanf("%d", &securityQuestionChoice);

    switch (securityQuestionChoice)
    {
    case 1:
        newRequest.securityQuestion = 1;
        break;
    case 2:
        newRequest.securityQuestion = 2;
        break;
    case 3:
        newRequest.securityQuestion = 3;
        break;
    case 4:
        newRequest.securityQuestion = 4;
        break;
    case 5:
        newRequest.securityQuestion = 5;
        break;
    case 6:
        newRequest.securityQuestion = 6;
        break;
    case 7:
        newRequest.securityQuestion = 7;
        break;
    case 8:
        newRequest.securityQuestion = 8;
        break;
    default:
        printf("\n\033[1;31mInvalid choice!\033[0m Please select a valid option from the list.\n");
        return; // Exit if invalid option is selected
    }

    // Prompt for the answer to the selected security question
    printf("\n\033[1;32mGreat choice!\033[0m ");
    printf("Now, please enter your answer:\033[1m");
    scanf("%s", newRequest.securityAnswer);
    printf("\033[0m\nThank you! Your answer has been securely saved.\n");
    printf("\033[1;36m---------------------------------------------\033[0m\n");

    strcpy(newRequest.status, "Pending"); // Initial status is pending

    // Save the request
    accountRequests[requestCount++] = newRequest;
    // Success message
    system("cls");
    printf("\n\033[1;32mAccount Creation Request Submitted Successfully!\033[0m\n");
}

// Function to convert a string to uppercase
void toUpperCase(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = str[i] - 'a' + 'A';
        }
    }
}

int isValidPANFormat(char *pan)
{
    // Check the length of PAN number
    if (strlen(pan) != 10)
    {
        return 0;
    }

    // Check first 5 characters are letters
    for (int i = 0; i < 5; i++)
    {
        if (pan[i] < 'A' || pan[i] > 'Z')
        {
            return 0;
        }
    }

    // Check next 4 characters are digits
    for (int i = 5; i < 9; i++)
    {
        if (pan[i] < '0' || pan[i] > '9')
        {
            return 0;
        }
    }

    // Check last character is a letter
    if (pan[9] < 'A' || pan[9] > 'Z')
    {
        return 0;
    }

    return 1;
}

// Function to dispaly all Account Requests
int viewPendingRequests()
{
    int foundPending = 0; // Flag to check if any pending requests are found

    for (int i = 0; i < requestCount; i++)
    {
        if (strcmp(accountRequests[i].status, "Pending") == 0)
        {
            foundPending = 1; // Set flag if a pending request is found
        }
    }

    if (!foundPending)
    {
        printf("\033[1;31mNo pending account opening requests found!\033[0m\n"); // Message if no pending requests are found
        return 0;
    }

    printf("\n\033[1;34m========================= Pending Account Opening Requests =========================\033[0m\n\n");

    // Table headers in cyan
    printf("\033[1;36m%-20s %-20s %-15s %-15s %-15s\033[0m\n", "Request ID", "Name", "Mobile No", "Aadhar No", "PAN No");

    printf("--------------------------------------------------------------------------------------\n");

    for (int i = 0; i < requestCount; i++)
    {
        if (strcmp(accountRequests[i].status, "Pending") == 0)
        {
            printf(" %-15d%-2s %-18s%-15s%-15s%-15s\n", accountRequests[i].requestID, accountRequests[i].firstName, accountRequests[i].lastName, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo);
            printf("--------------------------------------------------------------------------------------\n");
        }
    }
    return 0;
}

// Function for admin to approve or reject requests
void approveOrRejectRequest()
{
    int requestID, choice;
    printf("\n\033[1;34m=== Approve/Reject Account Request ===\033[0m\n");
    printf("Enter Request ID to Approve/Reject: ");
    scanf("%d", &requestID);

    for (int i = 0; i < requestCount; i++)
    {
        if (accountRequests[i].requestID == requestID && strcmp(accountRequests[i].status, "Pending") == 0)
        {
            printf("\033[1;36mName:\033[0m %s %s %s\n\033[1;36mMobile Number:\033[0m %s\n\033[1;36mAadhar Number:\033[0m %s\n\033[1;36mPAN Number:\033[0m %s\n", accountRequests[i].firstName, accountRequests[i].middleName, accountRequests[i].lastName, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo);
            printf("\n1. Approve\n2. Reject\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Approve the request
                if (accountCount >= MAX_ACCOUNTS)
                {
                    printf("\033[1;31mError: Maximum account limit reached!\033[0m\n");
                    return;
                }

                struct Account newAccount;
                newAccount.accountNumber = baseAccountNumber++; // Auto-generated account number
                strcpy(newAccount.firstName, accountRequests[i].firstName);
                strcpy(newAccount.middleName, accountRequests[i].middleName);
                strcpy(newAccount.lastName, accountRequests[i].lastName);
                strcpy(newAccount.mobileNo, accountRequests[i].mobileNo);
                strcpy(newAccount.aadharNo, accountRequests[i].aadharNo);
                strcpy(newAccount.panNo, accountRequests[i].panNo);
                strcpy(newAccount.password, accountRequests[i].password);
                strcpy(newAccount.transactionPin, accountRequests[i].transactionPin);
                strcpy(newAccount.securityAnswer, accountRequests[i].securityAnswer);
                newAccount.balance = 1000.0; // Initial balance
                strcpy(newAccount.status, "Active");

                newAccount.securityQuestion = accountRequests[i].securityQuestion;

                // Add new account to the system
                accounts[accountCount++] = newAccount;
                strcpy(accountRequests[i].status, "Approved"); // Update request status

                // Save accounts to file
                saveAccountsToFile();
                saveRequestedAccountsToFile();

                printf("\033[1;32mSuccess: Account Request Approved!\033[0m\n\n");
            }
            else if (choice == 2)
            {
                strcpy(accountRequests[i].status, "Rejected"); // Update request status
                printf("\033[1;31mAccount Request Rejected!\033[0m\n\n");
            }
            else
            {
                printf("\033[1;31mInvalid choice! Please enter 1 or 2.\033[0m\n\n");
            }
            return;
        }
    }
    // If request ID was not found or already processed
    printf("\033[1;31mError: Request ID not found or already processed!\033[0m\n\n");
}

// Function to display all approved accounts
int displayAllAccounts()
{
    int foundAccount = 0;

    for (int i = 0; i < accountCount; i++)
    {
        foundAccount = 1;
    }

    if (!foundAccount)
    {
        printf("No Accounts found in your system!");
        return 0;
    }
    clearScreen();
    printf("\n=======================================================================================\n");
    printf("                                List of Active Accounts          \n");
    printf("=======================================================================================\n");

    // Table header with some formatting
    printf("\033[1;34m%-20s %-20s %-15s %-15s %-15s\033[0m\n", "Account No", "Name", "Mobile No", "Aadhar No", "PAN No");
    printf("----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < accountCount; i++)
    {
        printf("XXXXXXXX%d\t%-2s %-18s %-15s %-15s %-15s\n", accounts[i].accountNumber, accounts[i].firstName, accounts[i].lastName, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo);
        printf("----------------------------------------------------------------------------------------\n");
    }
    return 0;
}

// Function for admin menu
void adminMenu()
{
    int choice;
    while (1)
    {
        system("cls");
        // Admin menu interface
        printf("\n\033[1;36m===========================================\033[0m\n");
        printf("               \033[1;34mAdmin Panel\033[0m                 \n");
        printf("\033[1;36m===========================================\033[0m\n");
        printf("  \033[1;32m1.\033[0m View Pending Account Opening Requests\n");
        printf("  \033[1;32m2.\033[0m Approve/Reject Account Opening Requests\n");
        printf("  \033[1;32m3.\033[0m View All Accounts\n");
        printf("  \033[1;32m4.\033[0m View Pending Card Requests\n");
        printf("  \033[1;32m5.\033[0m Approve/Reject Card Requests\n");
        printf("  \033[1;32m6.\033[0m View Pending Accounts to Unfreeze\n");
        printf("  \033[1;32m7.\033[0m Freeze/Unfreeze an Account\n");
        printf("  \033[1;32m8.\033[0m Delete an Account\n");
        printf("  \033[1;32m9.\033[0m Log Out\n");
        printf("\033[1;36m===========================================\033[0m\n");
        printf("Enter your choice (1-9): ");

        // Use a scanf to read choice
        scanf("%d", &choice);

        // Execute based on choice
        switch (choice)
        {
        case 1:
            printf("\n\033[1;34mLoading pending account requests...\033[0m\n");
            viewPendingRequests();
            break;
        case 2:
            printf("\n\033[1;34mOpening approval/rejection interface...\033[0m\n");
            approveOrRejectRequest();
            break;
        case 3:
            printf("\n\033[1;34mDisplaying all accounts...\033[0m\n");
            displayAllAccounts();
            break;
        case 4:
            printf("\n\033[1;34mLoading pending card requests...\033[0m\n");
            viewPendingCardRequests();
            break;
        case 5:
            printf("\n\033[1;34mOpening card approval/rejection interface...\033[0m\n");
            approveOrRejectCardRequest();
            break;
        case 6:
            printf("\n\033[1;34mFetching accounts to unfreeze...\033[0m\n");
            viewPendingUnfreezeRequests();
            break;
        case 7:
            printf("\n\033[1;34mOpening account freeze/unfreeze options...\033[0m\n");
            freezeOrUnfreezeAccount();
            break;
        case 8:
            printf("\n\033[1;34mOpening account deletion interface...\033[0m\n");
            deleteAccount();
            break;
        case 9:
            printf("\n\033[1;32mLogging out...\033[0m\n"); // Green color for successful logout
            return;                                        // Log out and return to main menu
        default:
            printf("\n\033[1;31m Invalid choice! Please select a valid option (1-9).\033[0m\n"); // Red color for invalid input
            break;
        }

        // Wait for the user to press enter before going back to the menu
        printf("\n\033[1;34mPress Enter to return to the Admin Menu...\033[0m");
        getchar(); // Capture the newline left by scanf
        getchar(); // Wait for user input
    }
}

int customerLogin()
{
    char mobileNo[100], password[50];
    system("cls");

    printf("\n\033[1;34m========== Customer Login ==========\033[0m\n");
    printf("\033[1;36mEnter Your Mobile Number: \033[0m");
    scanf("%s", mobileNo);

    int index = findAccountByMobileOnly(mobileNo);
    if (index == -1)
    {
        printf("\033[1;31mAccount with this mobile number not found!\033[0m\n");
        return 0;
    }

    // Check if the account is already frozen
    if (strcmp(accounts[index].status, "Frozen") == 0)
    {
        printf("\033[1;33mYour account is currently frozen. Please contact the admin.\033[0m\n");
        int choice;
        // Ask if they want to request to unfreeze the account
        printf("\n\033[1;36mDo you want to request to unfreeze your account?\033[0m\n");
        printf("1. Yes\n2. No\n\033[1;36mEnter your choice: \033[0m");
        scanf("%d", &choice);

        if (choice == 1)
        {
            // Send request to unfreeze
            FILE *file = fopen("unfreezeRequests.txt", "a");
            if (file == NULL)
            {
                printf("\033[1;31mError opening unfreeze request file! Please try again later.\033[0m\n");
                return 0;
            }
            fprintf(file, "%d %s %s %s\n", accounts[index].accountNumber, accounts[index].firstName, accounts[index].middleName, accounts[index].lastName); // Store account number and name
            fclose(file);
            printf("\033[1;32mYour request to unfreeze your account has been submitted successfully.\033[0m\n");
            return 0;
        }
        else
        {
            printf("\033[1;33mNo request submitted. Returning to the main menu.\033[0m\n");
            return 0;
        }
    }

    int passwordAttempts = 0;

    while (passwordAttempts < 3)
    {
        printf("\033[1;36mEnter Your Password: \033[0m");
        readPassword(password, sizeof(password));

        if (strcmp(accounts[index].password, password) == 0)
        {
            // Successful login
            system("cls");
            printf("\033[1;32mLogin successful! Welcome, %s %s %s.\033[0m\n",
                   accounts[index].firstName, accounts[index].middleName, accounts[index].lastName);
            clearScreen();
            customerPostLoginMenu(&accounts[index]);
            return 1;
        }
        else
        {
            passwordAttempts++;
            printf("\033[1;31mInvalid password!\033[0m\n");

            if (passwordAttempts >= 3)
            {
                printf("\033[1;31mYou have exceeded the maximum number of password attempts.\033[0m\n");
                printf("\033[1;31mYour account has been frozen.\033[0m\n");
                strcpy(accounts[index].status, "Frozen");
                saveAccountsToFile();
                return 0;
            }

            // Offer the recovery option
            int choice;
            printf("\033[1;36m\n========================================\033[0m\n");
            printf("\033[1;31m            LOGIN FAILED                \033[0m\n");
            printf("\033[1;36m========================================\033[0m\n\n");
            printf("Please choose an option below:\n");
            printf("\033[1;33m  1. Forgot Password\n");
            printf("  2. Try Again\033[0m\n\n");
            printf("Enter your choice (1-2): ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Forgot Password Flow with Security Question Attempts
                printf("\033[1;34m\n--- FORGOT PASSWORD ---\033[0m\n\n");
                printf("To reset your password, please answer the following security question:\n\n");
                displaySecurityQuestion(accounts[index].securityQuestion);
                int secAttempts = 0;
                int securitySuccess = 0;
                char answer[100];
                while (secAttempts < 3)
                {
                    printf("\n\033[1;33mEnter Your Answer: \033[0m");
                    scanf("%s", answer);

                    if (strcmp(accounts[index].securityAnswer, answer) == 0)
                    {
                        securitySuccess = 1;
                        break;
                    }
                    else
                    {
                        secAttempts++;
                        printf("\033[1;31mIncorrect answer!\033[0m\n");
                        if (secAttempts >= 3)
                        {
                            printf("\033[1;31mYou have exceeded the maximum number of security question attempts.\033[0m\n");
                            printf("\033[1;31mYour account has been frozen.\033[0m\n");
                            strcpy(accounts[index].status, "Frozen");
                            saveAccountsToFile();
                            return 0;
                        }
                        else
                        {
                            printf("\033[1;33mPlease try again.\033[0m\n");
                        }
                    }
                }

                if (securitySuccess)
                {
                    char newPassword[50], confirmPassword[50];
                    printf("\n\033[1;32mAnswer is correct!\033[0m\n");
                    printf("Proceeding to password reset...\n\n");

                    while (1)
                    {
                        printf("\033[1;33mEnter a New Password: \033[0m");
                        readPassword(newPassword, sizeof(newPassword));
                        printf("\033[1;33mConfirm Your New Password: \033[0m");
                        readPassword(confirmPassword, sizeof(confirmPassword));

                        if (strcmp(newPassword, confirmPassword) == 0)
                        {
                            strcpy(accounts[index].password, newPassword);
                            // Optionally update the request record if needed:
                            strcpy(accountRequests[index].password, newPassword);
                            printf("\n\033[1;32m========================================\033[0m\n");
                            printf("\033[1;32m       PASSWORD RESET SUCCESSFUL        \033[0m\n");
                            printf("\033[1;32m========================================\033[0m\n\n");
                            printf("You can now log in with your new password.\n");
                            break;
                        }
                        else
                        {
                            printf("\033[1;31mPasswords do not match! Please try again.\033[0m\n");
                        }
                    }
                    // After successful password reset, allow them to try logging in again.
                    continue;
                }
            }
            else if (choice == 2)
            {
                // Simply continue the loop to try entering the password again.
                continue;
            }
            else
            {
                printf("\033[1;31mInvalid choice! Please try again.\033[0m\n");
            }
        }
    }
    return 0;
}

int displaySecurityQuestion(int choice)
{
    int ch = choice;
    printf("Security Question:\n");
    switch (ch)
    {
    case 1:
        printf("What was the name of your first pet?\n");
        break;
    case 2:
        printf("What is the name of the city where you were born?\n");
        break;
    case 3:
        printf("Who is your favorit singer?\n");
        break;
    case 4:
        printf("What color do you like the most?\n");
        break;
    case 5:
        printf("What is your favorite book?\n");
        break;
    case 6:
        printf("Which teacher did you like the most at school?\n");
        break;
    case 7:
        printf("What is your favorite sport?\n");
        break;
    case 8:
        printf("Who is your favorite actor?\n");
        break;
    default:
        break;
    }
    return 0;
}

// Function to find an account by mobile number and password
int findAccountByPassword(char *password)
{
    for (int i = 0; i < accountCount; i++)
    {
        if (strcmp(accounts[i].password, password) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Function to find an account by mobile number only
int findAccountByMobileOnly(const char *mobileNo)
{
    for (int i = 0; i < accountCount; i++)
    {
        if (strcmp(accounts[i].mobileNo, mobileNo) == 0)
        {
            return i; // Return the index if a match is found
        }
    }
    return -1; // Return -1 if no match is found
}

void transferMoney(struct Account *loggedInCustomer)
{
    int receiverAccountNumber;
    float amount;
    char enteredPin[10];

    printf("\n\033[1;34m==================== Money Transfer ====================\033[0m\n\n");

    int receiverIndex = -1;
    int choice;
    while (1)
    {
        // Input receiver account number
        printf("\033[1;36mEnter The Last 4 Digits of The Receiver's Account Number: \033[0m");
        scanf("%d", &receiverAccountNumber);

        // Check if sender is trying to transfer to their own account
        if (loggedInCustomer->accountNumber == receiverAccountNumber)
        {
            printf("\033[1;31mError: You cannot transfer money to your own account!\033[0m\n");
            printf("\033[1;31mPlease enter a different account number.\033[0m\n");
            printf("\033[1;33mDo you want to try again?\033[0m\n");
            printf("\033[1;33m1. Yes\n2. No\033[0m\n");
            printf("\033[1;33mEnter your choice: \033[0m");
            scanf("%d", &choice);
            if (choice == 1)
            {
                continue;
            }
            else
            {
                return;
            }
        }

        // Find receiver's account

        for (int i = 0; i < accountCount; i++)
        {
            if (accounts[i].accountNumber == receiverAccountNumber)
            {
                receiverIndex = i;
                break;
            }
        }

        // Check if the receiver's account exists
        if (receiverIndex == -1)
        {
            printf("\033[1;31mError: Receiver's account not found! Please verify the account number.\033[0m\n");
            printf("\033[1;31mPlease enter a different account number.\033[0m\n");
            printf("\033[1;33mDo you want to try again?\033[0m\n");
            printf("\033[1;33m1. Yes\n2. No\033[0m\n");
            printf("\033[1;33mEnter your choice: \033[0m");
            scanf("%d", &choice);
            if (choice == 1)
            {
                continue;
            }
            else
            {
                return;
            }
        }
        else
        {
            break;
        }
    }

    // Input transfer amount
    printf("\033[1;36mEnter Amount to Transfer: \033[0m");
    scanf("%f", &amount);

    // Check if the sender has enough balance
    if (loggedInCustomer->balance < amount)
    {
        printf("\033[1;31mError: Insufficient balance! Your current balance is: Rs. %.2f/-\033[0m\n", loggedInCustomer->balance);
        return;
    }

    // Verify transaction PIN with security feature
    int pinAttempts = 0;
    while (1)
    {
        printf("\033[1;36mEnter your 4-digit transaction PIN: \033[0m");
        readPassword(enteredPin, sizeof(enteredPin));

        if (strcmp(loggedInCustomer->transactionPin, enteredPin) != 0)
        {
            pinAttempts++;
            printf("\033[1;31mError: Invalid transaction PIN!\033[0m\n");

            // If three incorrect attempts, freeze account and exit
            if (pinAttempts >= 3)
            {
                printf("\033[1;31mYou have exceeded the maximum number of transaction PIN attempts.\033[0m\n");
                printf("\033[1;31mYour account has been frozen.\033[0m\n");
                strcpy(loggedInCustomer->status, "Frozen");
                saveAccountsToFile();
                return;
            }

            // Provide forgot PIN option after a failed attempt
            int choice;
            printf("\033[1;34m\n========================================\033[0m\n");
            printf("\033[1;34m       TRANSACTION AUTHENTICATION       \033[0m\n");
            printf("\033[1;34m========================================\033[0m\n\n");
            printf("Please choose an option:\n");
            printf("\033[1;33m  1. Forgot Transaction PIN\n");
            printf("  2. Try Again\033[0m\n\n");
            printf("Enter your choice (1-2): ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Forgot PIN flow with security question attempts
                int index = findAccountByMobileOnly(loggedInCustomer->mobileNo);
                if (index == -1)
                {
                    printf("\033[1;31m\nAccount not found for the provided mobile number.\033[0m\n");
                    return;
                }
                printf("\033[1;34m\n--- FORGOT PIN ---\033[0m\n\n");
                printf("To reset your transaction PIN, please answer the following security question:\n\n");
                displaySecurityQuestion(accounts[index].securityQuestion);

                int secPinAttempts = 0;
                int correctAnswer = 0;
                char answer[100];
                while (secPinAttempts < 3)
                {
                    printf("\n\033[1;33mEnter Your Answer: \033[0m");
                    scanf("%s", answer);
                    if (strcmp(accounts[index].securityAnswer, answer) == 0)
                    {
                        correctAnswer = 1;
                        break;
                    }
                    else
                    {
                        secPinAttempts++;
                        printf("\033[1;31mIncorrect answer!\033[0m\n");
                        if (secPinAttempts >= 3)
                        {
                            printf("\033[1;31mYou have exceeded the maximum number of attempts.\033[0m\n");
                            printf("\033[1;31mYour account has been frozen.\033[0m\n");
                            strcpy(loggedInCustomer->status, "Frozen");
                            saveAccountsToFile();
                            return;
                        }
                        else
                        {
                            printf("\033[1;33mPlease try again.\033[0m\n");
                        }
                    }
                }

                if (correctAnswer)
                {
                    char newPIN[10], confirmPIN[10];
                    printf("\033[1;32m\nAnswer is correct!\033[0m\n");
                    printf("Proceeding to PIN reset...\n\n");

                    while (1)
                    {
                        printf("\033[1;33mEnter a New 4-digit PIN: \033[0m");
                        readPassword(newPIN, sizeof(newPIN));
                        printf("\033[1;33mConfirm Your New PIN: \033[0m");
                        readPassword(confirmPIN, sizeof(confirmPIN));

                        if (strcmp(newPIN, confirmPIN) == 0)
                        {
                            strcpy(accounts[index].transactionPin, newPIN); // Update transaction PIN
                            printf("\n\033[1;32m========================================\033[0m\n");
                            printf("\033[1;32m          PIN RESET SUCCESSFUL          \033[0m\n");
                            printf("\033[1;32m========================================\033[0m\n\n");
                            printf("You can now continue your transaction with the new PIN.\n");
                            break;
                        }
                        else
                        {
                            printf("\033[1;31m\nPINs do not match! Please try again.\033[0m\n");
                        }
                    }
                }
            }
            else if (choice == 2)
            {
                continue; // Allow user to retry PIN entry
            }
            else
            {
                printf("\033[1;31m\nInvalid choice! Please select either 1 or 2.\033[0m\n");
            }
        }
        else
        {
            break; // Correct PIN entered, break out of loop
        }
    }

    // Perform transfer after successful PIN verification
    loggedInCustomer->balance -= amount;
    accounts[receiverIndex].balance += amount;

    // Record the transaction
    transactions[transactionCount].senderAccountNumber = loggedInCustomer->accountNumber;
    transactions[transactionCount].receiverAccountNumber = accounts[receiverIndex].accountNumber;
    transactions[transactionCount].amount = amount;
    transactionCount++;

    printf("\033[1;32mTransfer successful! Rs. %.2f/- has been sent to Account No: XXXXXXXX%d\033[0m\n", amount, receiverAccountNumber);
    printf("\033[1;36mYour new balance is: Rs. %.2f/-\033[0m\n", loggedInCustomer->balance);

    // Save updated accounts and transactions to file
    saveAccountsToFile();
    saveTransactionsToFile();
}

// Function to view transaction history
void viewTransactionHistory(struct Account *loggedInCustomer)
{
    int found = 0; // To track if there are any transactions

    for (int i = 0; i < transactionCount; i++)
    {
        if (transactions[i].senderAccountNumber == loggedInCustomer->accountNumber)
        {
            found = 1;
        }
        else if (transactions[i].receiverAccountNumber == loggedInCustomer->accountNumber)
        {
            found = 1;
        }
    }

    if (!found)
    {
        // If no transactions are found
        system("cls");
        printf("\n\033[1;33mNo transactions found for this account.\033[0m\n");
        return;
    }
    system("cls");
    printf("\n\033[1;34m====================== Transaction History ======================\033[0m\n\n");
    printf("\033[1;36mAccount Number: \033[0mXXXXXXXX%d\n", loggedInCustomer->accountNumber);
    printf("--------------------------------------------------------------------\n");
    printf("\033[1;32m|   Type   |     %-14s    |   Related Account   |\033[0m\n", "Amount");
    printf("--------------------------------------------------------------------\n");

    for (int i = 0; i < transactionCount; i++)
    {
        if (transactions[i].senderAccountNumber == loggedInCustomer->accountNumber)
        {
            // Debit transaction (outgoing)
            printf("\033[1;31m|  Debit   |    %-14.2f     |  To   XXXXXXXX%d   |\033[0m\n", transactions[i].amount, transactions[i].receiverAccountNumber);
            found = 1;
        }
        else if (transactions[i].receiverAccountNumber == loggedInCustomer->accountNumber)
        {
            // Credit transaction (incoming)
            printf("\033[1;32m|  Credit  |    %-14.2f     |  From XXXXXXXX%d   |\033[0m\n", transactions[i].amount, transactions[i].senderAccountNumber);
            found = 1;
        }
    }

    printf("--------------------------------------------------------------------\n");
    printf("\033[1;34m===================================================================\033[0m\n");
}

// Function to update customer profile
void updateProfile(struct Account *loggedInCustomer)
{
    int choice;
    system("cls");
    printf("\033[1;34m\n************ Update Your Profile ************\033[0m\n\n");
    printf("What would you like to update?\n");
    printf("\033[1;33m  1. Name\033[0m\n");
    printf("\033[1;33m  2. Mobile Number\033[0m\n");
    printf("\033[1;33m  3. Aadhar Number\033[0m\n");
    printf("\033[1;33m  4. Cancel\033[0m\n\n");
    printf("Enter your choice (1-4): ");

    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        printf("\033[1;36mEnter New First Name: \033[0m");
        scanf("%s", loggedInCustomer->firstName);
        printf("\033[1;36mEnter New Last Name: \033[0m");
        scanf("%s", loggedInCustomer->lastName);
        printf("\033[1;32mName updated successfully!\033[0m\n");

        break;
    case 2:
        while (1)
        {
            printf("\033[1;36mEnter Your Mobile No : \033[0m");
            scanf("%s", loggedInCustomer->mobileNo);

            if (strlen(loggedInCustomer->mobileNo) == 10 && strspn(loggedInCustomer->mobileNo, "0123456789") == 10)
            {
                printf("\033[1;32mMobile Number updated successfully!\033[0m\n");

                break;
            }
            else
            {
                printf("\033[1;31mInvalid Mobile Number! Please enter exactly 10 digits.\033[0m\n");
            }
        }
        break;

    case 3:
        while (1)
        {
            printf("\033[1;36mEnter Your Aadhar No : \033[0m");
            scanf("%s", loggedInCustomer->aadharNo);

            if (strlen(loggedInCustomer->aadharNo) == 12 && strspn(loggedInCustomer->aadharNo, "0123456789") == 12)
            {
                printf("\033[1;32mAadhar Number is valid, confirmed and updated successfully!\033[0m\n");
                break;
            }
            else
            {
                printf("\033[1;31mInvalid Aadhar Number! Please enter exactly 12 digits.\033[0m\n");
            }
        }
        break;
    case 4:
        printf("\033[1;31mProfile update canceled.\033[0m\n");
        return;
    default:
        printf("\033[1;31mInvalid choice! Please try again.\033[0m\n");
        break;
    }

    // After updating, save the updated account details to file
    saveAccountsToFile();
    saveRequestedAccountsToFile();
}

// Function for post-login customer menu
void customerPostLoginMenu(struct Account *loggedInCustomer)
{
    int choice;
    while (1)
    {
        // Check if the account is frozen; if so, log out and return to the login menu.
        if (strcmp(loggedInCustomer->status, "Frozen") == 0)
        {
            printf("\n\033[1;31mYour account has been frozen due to multiple failed attempts.\033[0m\n");
            printf("\033[1;31mYou are now being logged out.\033[0m\n");
            sleep(4); // Optional: give the user time to read the message.
            return;   // Return to the login menu.
        }
        // clearScreen();
        system("cls");
        printf("\n\033[1;35m========== Welcome to Your Personal Banking Portal ==========\033[0m\n");
        printf("\033[1;36mHello, %s! What would you like to do today?\033[0m\n\n", loggedInCustomer->firstName);
        printf("\033[1;32m1. View Account Details\033[0m\n");
        printf("\033[1;32m2. Check Balance\033[0m\n");
        printf("\033[1;32m3. Transfer Money\033[0m\n");
        printf("\033[1;32m4. View Recent Transactions\033[0m\n");
        printf("\033[1;32m5. Manage My Cards\033[0m\n");
        printf("\033[1;32m6. Update Profile\033[0m\n");
        printf("\033[1;31m7. Logout\033[0m\n");
        printf("===========================================================\n");
        printf("\033[1;36mPlease select an option by entering the corresponding number: \033[0m");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            system("cls");
            printf("\n\033[1;33m Account Details \033[0m\n");
            printf("\033[1;37mAccount Number: \033[0mXXXXXXXX%d\n", loggedInCustomer->accountNumber);
            printf("\033[1;37mName: \033[0m%s %s %s\n", loggedInCustomer->firstName, loggedInCustomer->middleName, loggedInCustomer->lastName);
            printf("\033[1;37mMobile No: \033[0m%s\n", loggedInCustomer->mobileNo);
            break;
        case 2:
            system("cls");
            printf("\n\033[1;33m Current Balance \033[0m\n");
            printf("\033[1;37mYour balance is: \033[0mRs. %.2f/-\n", loggedInCustomer->balance);
            break;
        case 3:
            system("cls");
            printf("\n\033[1;33m Let's transfer some money!\033[0m\n");
            clearScreen();
            transferMoney(loggedInCustomer);
            break;
        case 4:
            viewTransactionHistory(loggedInCustomer);
            break;
        case 5:
            customerCardManagement(loggedInCustomer);
            break;
        case 6:
            updateProfile(loggedInCustomer);
            break;
        case 7:
            printf("\n\033[1;31m Logging out...\033[0m\n");
            printf("\033[1;36mThank you for banking with us, %s! See you again soon!\033[0m\n", loggedInCustomer->firstName);
            return;
        default:
            printf("\n\033[1;31mOops! That doesn't seem right. Please enter a valid option.\033[0m\n");
        }

        printf("\n\033[1;36mPress Enter to go back to the previous menu...\033[0m");
        getchar(); // To capture the newline after input
        getchar(); // To wait for the user to press enter
    }
}

// Function for customer menu (initial options)
void customerMenu()
{
    int choice;
    while (1)
    {
        clearScreen();
        printf("\n\033[1;36m=======================================\033[0m\n");
        printf("\033[1;36m          Welcome to Your Menu         \033[0m\n");
        printf("\033[1;36m=======================================\033[0m\n");

        printf("\n\033[1;33m1.  Login\033[0m\n");
        printf("\033[1;33m2.  Sign Up (Application for Account Opening)\033[0m\n");
        printf("\033[1;33m3.  Go To Home\033[0m\n");

        printf("\n\033[1;32mPlease Enter Your Choice (1-3): \033[0m");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            if (customerLogin())
            {
                // Successful login, proceed to post-login menu
            }
            else
            {
                printf("\n\033[1;31m Login failed! Please check your credentials.\033[0m\n");
            }
            break;
        case 2:
            requestAccountCreation();
            break;
        case 3:
            // Return to the main menu (exit customer menu loop)
            printf("\n\033[1;33mReturning to the Home...\033[0m\n");
            return;
        default:
            printf("\n\033[1;31m Invalid choice! Please try again.\033[0m\n");
        }
    }
}

// Function to request a card (customer side)
void requestCard(struct Account *loggedInCustomer)
{
    struct CardRequest newCardRequest;
    newCardRequest.requestID = cardRequestCount + 1;
    newCardRequest.accountNumber = loggedInCustomer->accountNumber;
    system("cls");
    printf("Redirecting to the Card Request Portal...");
    clearScreen();
    printf("\n\033[1;34m========== Card Request Portal ==========\033[0m\n\n");
    printf("\033[1;36mHello, %s! Let's help you request a new card.\033[0m\n", loggedInCustomer->firstName);
    printf("What type of card would you like to request?\n\n");
    printf("\033[1;32m1. Debit Card\033[0m\n");
    printf("\033[1;32m2. Credit Card\033[0m\n");
    printf("\033[1;36mEnter your choice: \033[0m");
    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        strcpy(newCardRequest.cardType, "Debit");
        system("cls");
        printf("\n\033[1;33mYou've chosen to request a Debit Card.\033[0m\n");
    }
    else if (choice == 2)
    {
        strcpy(newCardRequest.cardType, "Credit");
        system("cls");

        printf("\n\033[1;33mYou've chosen to request a Credit Card.\033[0m\n");
    }
    else
    {
        printf("\n\033[1;31mOops! Invalid choice. Please try again.\033[0m\n");
        return;
    }

    strcpy(newCardRequest.status, "Pending");

    // Save the request
    cardRequests[cardRequestCount++] = newCardRequest;
    saveCardRequests(); // Save the updated requests to file
    printf("\n\033[1;32mYour %s Card request has been successfully submitted! \033[0m\n", newCardRequest.cardType);
    printf("\033[1;36mYour request is now under review, and you'll be notified once processed. \033[0m\n");
    sleep(5);
}

// Function for admin to view all pending card requests
void viewPendingCardRequests()
{
    int foundCardRequest = 0;

    for (int i = 0; i < cardRequestCount; i++)
    {
        if (strcmp(cardRequests[i].status, "Pending") == 0)
        {
            foundCardRequest = 1;
        }
    }
    if (!foundCardRequest)
    {
        printf("\n\033[1;33mGreat! There are no pending card requests at the moment.\033[0m\n");
        return;
    }
    printf("\n\033[1;34m========== Pending Card Requests ==========\033[0m\n");
    printf("\033[1;36mBelow are the card requests that are currently pending:\033[0m\n\n");

    // Table headers with better formatting
    printf("\033[1;32m%-15s %-20s %-15s\033[0m\n", "Request ID", "Account Number", "Card Type");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < cardRequestCount; i++)
    {
        if (strcmp(cardRequests[i].status, "Pending") == 0)
        {
            printf("\033[1;37m%-15d %-20d %-15s\033[0m\n",
                   cardRequests[i].requestID, cardRequests[i].accountNumber, cardRequests[i].cardType);
        }
    }
    return;
}

// Function for admin to approve or reject card requests
void approveOrRejectCardRequest()
{
    int requestID, choice;
    printf("\n\033[1;34m========== Approve or Reject Card Request ==========\033[0m\n");
    printf("\033[1;36mEnter the Card Request ID you would like to process: \033[0m");
    scanf("%d", &requestID);

    // Loop through the requests to find the matching one
    for (int i = 0; i < cardRequestCount; i++)
    {
        if (cardRequests[i].requestID == requestID && strcmp(cardRequests[i].status, "Pending") == 0)
        {
            printf("\n\033[1;33mRequest Details:\033[0m\n");
            printf("\033[1;37mAccount Number: \033[0m%d\n", cardRequests[i].accountNumber);
            printf("\033[1;37mCard Type: \033[0m%s\n", cardRequests[i].cardType);

            // Present options to approve or reject
            printf("\n\033[1;32m1. Approve\033[0m\n\033[1;31m2. Reject\033[0m\n");
            printf("\033[1;36mEnter your choice: \033[0m");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Approve the card request
                struct Card newCard;
                newCard.accountNumber = cardRequests[i].accountNumber;
                strcpy(newCard.cardType, cardRequests[i].cardType);
                generateCardDetails(&newCard); // Generate card details (e.g., number, CVV)

                strcpy(newCard.cardStatus, "Active");
                cards[cardCount++] = newCard;
                saveCards(); // Save the approved card to the file

                strcpy(cardRequests[i].status, "Approved");
                saveCardRequests(); // Save updated requests to the file

                printf("\n\033[1;32m Card Request Approved! \033[0m\n");
                printf("\033[1;36mThe new card has been generated and is now active.\033[0m\n");
            }
            else if (choice == 2)
            {
                // Reject the card request
                strcpy(cardRequests[i].status, "Rejected");
                saveCardRequests(); // Save updated requests to the file

                printf("\n\033[1;31m Card Request Rejected!\033[0m\n");
                printf("\033[1;36mThis card request has been successfully marked as rejected.\033[0m\n");
            }
            else
            {
                printf("\n\033[1;31mInvalid choice! Please select a valid option.\033[0m\n");
            }
            return;
        }
    }

    // If the request ID was not found or already processed
    printf("\n\033[1;33m Request ID not found or already processed!\033[0m\n");
    printf("\033[1;36mPlease enter a valid request ID to continue.\033[0m\n");
}

// Function to generate card details (card number, CVV, pin)
void generateCardDetails(struct Card *newCard)
{

    // Generate a 4-digit card number
    newCard->cardNumber = baseCardNumber;

    // Generate a random CVV (3 digits)
    newCard->cvv = baseCVV;

    // Initially set card PIN to "0"
    strcpy(newCard->cardPin, "0");

    baseCardNumber++;
    baseCVV++;
}

void customerCardManagement(struct Account *loggedInCustomer)
{
    int accountNumber = loggedInCustomer->accountNumber;
    int choice;

    while (1) // Loop to stay in card management until the user chooses to exit
    {
        system("cls");
        printf("Redirecting to the Card Management...");
        clearScreen();

        printf("\n\033[1;34m=============== Card Management ===============\033[0m\n\n");
        printf("\033[1;36mWhat would you like to do today, %s?\033[0m\n\n", loggedInCustomer->firstName);
        printf("\033[1;32m1. Request a New Card\033[0m\n");
        printf("\033[1;32m2. Manage Your Cards\033[0m\n");
        printf("\033[1;31m3. Go To Previous Menu\033[0m\n");
        printf("\033[1;36mEnter your choice: \033[0m");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // Call function to request a new card
            requestCard(loggedInCustomer);
            break;

        case 2:
        {
            int found = 0;

            // Check if the customer has any cards linked to their account
            for (int i = 0; i < cardCount; i++)
            {
                if (cards[i].accountNumber == accountNumber)
                {
                    found = 1; // Card found
                    break;
                }
            }

            if (found)
            {
                system("cls");
                printf("\n\033[1;33m========= Your Cards =========\033[0m\n\n");

                // Loop through all cards linked to the customer's account
                for (int i = 0; i < cardCount; i++)
                {
                    if (cards[i].accountNumber == accountNumber)
                    {

                        int manageChoice;
                        while (1) // Loop to stay in card management for each card
                        {
                            printf("\033[1;37mCard Details:\033[0m\n");
                            printf("\033[1;37mCard Number: \033[0mXXXX XXXX %d\n", cards[i].cardNumber);
                            printf("\033[1;37mCVV: \033[0m%d\n", cards[i].cvv);
                            printf("\033[1;37mExpiry Date: 12/31\033[0m\n");
                            printf("\033[1;37mCard Type: \033[0m%s\n", cards[i].cardType);
                            printf("\033[1;37mStatus: \033[0m%s\n", cards[i].cardStatus);
                            printf("\n\033[1;36mOptions:\033[0m\n");
                            printf("\033[1;32m1. Generate/Change PIN\033[0m\n");
                            printf("\033[1;32m2. Block/Unblock Card\033[0m\n");
                            printf("\033[1;31m3. Go to Previous Menu\033[0m\n");
                            printf("\033[1;36mEnter your choice: \033[0m");
                            scanf("%d", &manageChoice);

                            switch (manageChoice)
                            {
                            case 1:
                                // Change or generate PIN for the card
                                changeCardPin(&cards[i]);
                                break;

                            case 2:
                                // Block or unblock the card
                                blockOrUnblockCard(&cards[i]);
                                break;

                            case 3:
                                // Exit to card management main menu
                                goto mainMenu;
                                break;

                            default:
                                printf("\n\033[1;31mInvalid choice! Please try again.\033[0m\n");
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                system("cls");
                printf("\n\033[1;31mNo cards linked to your account.\033[0m\n");
                sleep(3);
            }
            break;
        }

        case 3:
            return;

        default:
            printf("\n\033[1;31mInvalid choice! Please enter a valid option.\033[0m\n");
            break;
        }

    mainMenu:; // Label to break out of the inner loop and return to the main menu
    }
}

// Function to change or generate the card PIN
void changeCardPin(struct Card *card)
{
    // If no PIN is set, prompt the user to create a new one
    if (strcmp(card->cardPin, "0") == 0)
    {
        printf("\n\033[1;31mNo PIN is currently set for this card.\033[0m\n");
        char newPin[6], confirmPin[6];

        // Loop to ensure a valid PIN is set
        while (1)
        {
            printf("\033[1;36mEnter a new 4-digit PIN: \033[0m");
            readPassword(newPin, sizeof(newPin));

            // Ensure the entered PIN is exactly 4 digits
            if (strlen(newPin) == 4)
            {
                printf("\033[1;36mConfirm the new PIN: \033[0m");
                readPassword(confirmPin, sizeof(confirmPin));

                // Check if the confirmed PIN matches
                if (strcmp(newPin, confirmPin) == 0)
                {
                    strcpy(card->cardPin, newPin); // Set the new PIN
                    saveCards();                   // Save updated PIN to file
                    system("cls");
                    printf("\n\033[1;32m PIN successfully created!\033[0m\n");
                    clearScreen();
                    break;
                }
                else
                {
                    printf("\n\033[1;31m PINs do not match. Please try again.\033[0m\n");
                }
            }
            else
            {
                printf("\n\033[1;31m Invalid PIN format. Must be exactly 4 digits.\033[0m\n");
            }
        }
    }
    else
    {
        // If a PIN is already set, ask the user to enter the current PIN before changing it
        char currentPin[6];
        printf("\n\033[1;33mA PIN is already set for this card.\033[0m\n");
        printf("\033[1;36mEnter current PIN: \033[0m");
        readPassword(currentPin, sizeof(currentPin));

        // Verify the current PIN
        if (strcmp(card->cardPin, currentPin) == 0)
        {
            char newPin[6], confirmPin[6];

            // Loop to ensure a valid new PIN is set
            while (1)
            {
                printf("\033[1;36mEnter a new 4-digit PIN: \033[0m");
                readPassword(newPin, sizeof(newPin));

                // Ensure the new PIN is exactly 4 digits
                if (strlen(newPin) == 4)
                {
                    printf("\033[1;36mConfirm the new PIN: \033[0m");
                    readPassword(confirmPin, sizeof(confirmPin));

                    // Check if the confirmed new PIN matches
                    if (strcmp(newPin, confirmPin) == 0)
                    {
                        strcpy(card->cardPin, newPin); // Update the PIN
                        saveCards();                   // Save updated PIN to file
                        system("cls");
                        printf("\n\033[1;32m PIN successfully changed!\033[0m\n");
                        clearScreen();
                        break;
                    }
                    else
                    {
                        printf("\n\033[1;31m PINs do not match. Please try again.\033[0m\n");
                    }
                }
                else
                {
                    printf("\n\033[1;31m Invalid PIN format. Must be exactly 4 digits.\033[0m\n");
                }
            }
        }
        else
        {
            printf("\n\033[1;31m Incorrect current PIN!\033[0m\n");
        }
    }
}

// Function to block or unblock a card
void blockOrUnblockCard(struct Card *card)
{
    printf("\n\033[1;36m=== Card Status ===\033[0m\n\n");

    if (strcmp(card->cardStatus, "Active") == 0)
    {
        printf("\033[1;33mYour card is currently \033[1;32mActive.\033[0m\n");
        printf("\033[1;36mDo you want to:\033[0m\n");
        printf("1. \033[1;31mBlock Temporarily\033[0m\n");
        printf("2. \033[1;31mBlock Permanently\033[0m\n");
        printf("3. \033[1;36mCancel\033[0m\n");
        printf("\033[1;36mEnter your choice: \033[0m");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) // Temporarily block the card
        {
            strcpy(card->cardStatus, "Temporarily Blocked");
            saveCards(); // Save updated status to file
            printf("\n\033[1;32m Card has been temporarily blocked.\033[0m\n");
        }
        else if (choice == 2) // Permanently block the card
        {
            strcpy(card->cardStatus, "Permanently Blocked");
            saveCards(); // Save updated status to file
            printf("\n\033[1;31m Card has been permanently blocked. It cannot be unblocked.\033[0m\n");
        }
        else if (choice == 3) // Return to the previous menu
        {
            return;
        }
        else
        {
            printf("\n\033[1;31m Invalid choice! Please try again.\033[0m\n");
        }
    }
    else if (strcmp(card->cardStatus, "Temporarily Blocked") == 0)
    {
        printf("\033[1;33mYour card is currently Temporarily Blocked.\033[0m\n");
        printf("\033[1;36mDo you want to unblock the card?\033[0m\n");
        printf("1. \033[1;32m1. Yes, Unblock it\033[0m\n");
        printf("0. \033[1;31m2. No, Keep it blocked\033[0m\n");
        printf("\033[1;36mEnter your choice: \033[0m");

        int choice;
        scanf("%d", &choice);

        if (choice == 1) // Unblock the card
        {
            strcpy(card->cardStatus, "Active");
            saveCards(); // Save updated status to file
            printf("\n\033[1;32m Card has been unblocked and is now Active.\033[0m\n");
        }
        else if (choice == 2)
        {
            printf("\n\033[1;33mCard remains temporarily blocked.\033[0m\n");
        }
        else
        {
            printf("\n\033[1;31m Invalid choice! Please try again.\033[0m\n");
        }
    }
    else if (strcmp(card->cardStatus, "Permanently Blocked") == 0)
    {
        printf("\n\033[1;31m This card is Permanently Blocked and cannot be unblocked.\033[0m\n");
    }
    else
    {
        printf("\n\033[1;31m Unknown card status! Please contact support.\033[0m\n");
    }
}

// Function to delete an account (admin-only)
void deleteAccount()
{
    int accountNumber;
    char confirmChoice;

    printf("\n\033[1;31m=== Delete Account ===\033[0m\n");
    printf("\033[1;31mEnter Account Number to delete: \033[0m");
    scanf("%d", &accountNumber);

    int found = -1;

    // Search for the account by account number
    for (int i = 0; i < accountCount; i++)
    {
        if (accounts[i].accountNumber == accountNumber)
        {
            found = i;
            break;
        }
    }

    // If account is found
    if (found != -1)
    {
        printf("\n\033[1;33mAccount found:\033[0m %s %s %s \033[1;33m(Account No: XXXXXXXX%d)\033[0m\n", accounts[found].firstName, accounts[found].middleName, accounts[found].lastName, accounts[found].accountNumber);
        printf("\n\033[1;31m Warning: This action is irreversible! Are you sure you want to delete this account?\033[0m\n");
        printf("\033[1;31m1. Yes, delete this account\033[0m\n");
        printf("\033[1;32m2. No, keep the account\033[0m\n");
        printf("\033[1;36mEnter your choice: \033[0m");
        scanf(" %c", &confirmChoice); // Adding space before %c to capture newline character properly

        if (confirmChoice == '1') // User chose "Yes"
        {
            // Shift all accounts after the found index to the left
            for (int i = found; i < accountCount - 1; i++)
            {
                accounts[i] = accounts[i + 1];
            }
            accountCount--; // Reduce the total account count

            for (int i = found; i < requestCount - 1; i++)
            {
                accountRequests[i] = accountRequests[i + 1];
            }
            requestCount--;

            // Save the updated accounts to file
            saveAccountsToFile();
            saveRequestedAccountsToFile();
            printf("\n\033[1;31m Account successfully deleted.\033[0m\n");
        }
        else if (confirmChoice == '2') // User chose "No"
        {
            printf("\n\033[1;33mAccount deletion cancelled. No changes made.\033[0m\n");
        }
        else
        {
            printf("\n\033[1;31m Invalid choice! Account deletion cancelled.\033[0m\n");
        }
    }
    else
    {
        printf("\n\033[1;31m Account not found! Please check the account number and try again.\033[0m\n");
    }
}

// Function to freeze or unfreeze an account (admin-only)
void freezeOrUnfreezeAccount()
{
    int accountNumber, choice;

    printf("\n\033[1;36m=== Freeze/Unfreeze Account ===\033[0m\n");
    printf("\033[1;33mEnter Account Number to freeze/unfreeze: \033[0m");
    scanf("%d", &accountNumber);

    int found = -1;

    // Search for the account by account number
    for (int i = 0; i < accountCount; i++)
    {
        if (accounts[i].accountNumber == accountNumber)
        {
            found = i;
            break;
        }
    }

    if (found != -1)
    {
        // Display account details
        printf("\n\033[1;33mAccount found:\033[0m %s %s %s \033[1;33m(Account No: XXXXXXXX%d)\033[0m\n",
               accounts[found].firstName, accounts[found].middleName, accounts[found].lastName, accounts[found].accountNumber);
        printf("\033[1;33mCurrent Status:\033[0m \033[1;32m%s\033[0m\n", accounts[found].status);

        if (strcmp(accounts[found].status, "Active") == 0)
        {
            // If the account is active, provide the option to freeze it
            printf("\033[1;36m1. Freeze Account\n\033[0m");
            printf("\033[1;33m2. Cancel\033[0m\n");
            printf("\033[1;36mEnter your choice: \033[0m");
            scanf("%d", &choice);

            if (choice == 1)
            {
                strcpy(accounts[found].status, "Frozen"); // Set account status to "Frozen"
                saveAccountsToFile();
                printf("\n\033[1;32m Account has been successfully frozen.\033[0m\n");
            }
            else
            {
                printf("\n\033[1;33mOperation cancelled.\033[0m\n");
            }
        }
        else if (strcmp(accounts[found].status, "Frozen") == 0)
        {
            // If the account is frozen, provide the option to unfreeze it
            printf("\033[1;36m1. Unfreeze Account\n\033[0m");
            printf("\033[1;33m2. Cancel\033[0m\n");
            printf("\033[1;36mEnter your choice: \033[0m");
            scanf("%d", &choice);

            if (choice == 1)
            {
                strcpy(accounts[found].status, "Active"); // Set account status back to "Active"
                saveAccountsToFile();
                printf("\n\033[1;32m Account has been successfully unfrozen.\033[0m\n");
            }
            else
            {
                printf("\n\033[1;33mOperation cancelled.\033[0m\n");
            }
        }
        else
        {
            printf("\033[1;31m Unknown account status!\033[0m\n");
        }
    }
    else
    {
        printf("\033[1;31m Account not found! Please check the account number and try again.\033[0m\n");
    }
}

void viewPendingUnfreezeRequests()
{
    FILE *file = fopen("unfreezeRequests.txt", "r");
    if (file == NULL)
    {
        printf("No pending unfreeze requests found.\n");
        return;
    }

    printf("Pending Unfreeze Requests:\nAccount Number\tName\n");
    int accountNumber;
    char name[100];

    while (fscanf(file, "%d %s", &accountNumber, name) == 2)
    {
        printf("XXXXXXXX%d\t%s\n", accountNumber, name);
    }

    fclose(file);
}

void removeUnfreezeRequest(int accountNumber)
{
    FILE *file = fopen("unfreezeRequests.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (file == NULL || tempFile == NULL)
    {
        printf("Error processing unfreeze request file!\n");
        return;
    }

    int accNum;
    char name[100];

    // Copy all requests except the one for the unfrozen account
    while (fscanf(file, "%d %s", &accNum, name) != EOF)
    {
        if (accNum != accountNumber) // Skip the unfrozen account
        {
            fprintf(tempFile, "%d %s\n", accNum, name);
        }
    }

    fclose(file);
    fclose(tempFile);

    // Replace original file with the updated one
    remove("unfreezeRequests.txt");
    rename("temp.txt", "unfreezeRequests.txt");
}

void clearScreen()
{
    printf("\n\n\t\033[1;31mPlease Do Not Press Any Key...\033[0m\n");
    // Delay for 3 seconds
    sleep(2);
    // ANSI escape code to clear the screen
    printf("\033[H\033[J");
}

// Function to read password and display asterisks
void readPassword(char *password, int maxLength)
{
    int index = 0;
    char ch;
    while (1)
    {
        ch = getch();
        if (ch == '\r')
        { // Enter key pressed
            password[index] = '\0';
            printf("\n");
            break;
        }
        else if (ch == '\b')
        { // Backspace key pressed
            if (index > 0)
            {
                index--;
                printf("\b \b");
            }
        }
        else if (index < maxLength - 1)
        {
            password[index++] = ch;
            printf("*");
        }
    }
}

void printWelcomeMessage()
{
    // Array to store a compact ASCII version of "WELCOME"
    const char *welcome[5][7] = {
        {GREEN "W   W" RESET, GREEN "EEEEE" RESET, GREEN "L    " RESET, GREEN " CCC " RESET, GREEN " OOO " RESET, GREEN "M   M" RESET, GREEN "EEEEE" RESET},
        {GREEN "W   W" RESET, GREEN "E    " RESET, GREEN "L    " RESET, GREEN "C   C" RESET, GREEN "O   O" RESET, GREEN "MM MM" RESET, GREEN "E    " RESET},
        {GREEN "W W W" RESET, GREEN "EEE  " RESET, GREEN "L    " RESET, GREEN "C    " RESET, GREEN "O   O" RESET, GREEN "M M M" RESET, GREEN "EEE  " RESET},
        {GREEN "WW WW" RESET, GREEN "E    " RESET, GREEN "L    " RESET, GREEN "C   C" RESET, GREEN "O   O" RESET, GREEN "M   M" RESET, GREEN "E    " RESET},
        {GREEN "W   W" RESET, GREEN "EEEEE" RESET, GREEN "LLLL " RESET, GREEN " CCC " RESET, GREEN " OOO " RESET, GREEN "M   M" RESET, GREEN "EEEEE" RESET}};

    // Array to store a compact ASCII version of "T"
    const char *t[5][1] = {
        {MAGENTA "          #####" RESET},
        {MAGENTA "            #  " RESET},
        {MAGENTA "            #  " RESET},
        {MAGENTA "            #  " RESET},
        {MAGENTA "            #  " RESET}};

    // Array to store a compact ASCII version of "O"
    const char *o[5][1] = {
        {MAGENTA " OOO " RESET},
        {MAGENTA "O   O" RESET},
        {MAGENTA "O   O" RESET},
        {MAGENTA "O   O" RESET},
        {MAGENTA " OOO " RESET}};

    // Array to store a compact ASCII version of "PSD BANK"
    const char *psd_bank[5][8] = {
        {YELLOW "PPPP " RESET, YELLOW " SSSS" RESET, YELLOW "DDDD " RESET, RED "     " RESET, CYAN "BBBB " RESET, CYAN " A A " RESET, CYAN "NN     N" RESET, CYAN "K  KK" RESET},
        {YELLOW "P   P" RESET, YELLOW "S    " RESET, YELLOW "D   D" RESET, RED "     " RESET, CYAN "B   B" RESET, CYAN "A   A" RESET, CYAN "N N    N" RESET, CYAN "K K " RESET},
        {YELLOW "PPPP " RESET, YELLOW " SSS " RESET, YELLOW "D   D" RESET, RED "     " RESET, CYAN "BBBB " RESET, CYAN "AAAAA" RESET, CYAN "N  N   N" RESET, CYAN "KK  " RESET},
        {YELLOW "P    " RESET, YELLOW "    S" RESET, YELLOW "D   D" RESET, RED "     " RESET, CYAN "B   B" RESET, CYAN "A   A" RESET, CYAN "N    N N" RESET, CYAN "K K " RESET},
        {YELLOW "P    " RESET, YELLOW "SSSS " RESET, YELLOW "DDDD " RESET, RED "     " RESET, CYAN "BBBB " RESET, CYAN "A   A" RESET, CYAN "N     NN" RESET, CYAN "K  KK" RESET}};

    // Print "WELCOME"
    for (int i = 0; i < 5; i++)
    {
        printf("                          ");
        for (int j = 0; j < 7; j++)
        {
            printf("%s  ", welcome[i][j]);
        }
        printf("\n");
    }

    // Print "TO" with indentation
    printf("\n");
    for (int i = 0; i < 5; i++)
    {
        printf("                               "); // Indentation for "TO"
        printf("%s  %s\n", t[i][0], o[i][0]);
    }

    // Print "PSD BANK"
    printf("\n");
    for (int i = 0; i < 5; i++)
    {
        printf("                     ");
        for (int j = 0; j < 8; j++)
        {
            printf("%s  ", psd_bank[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    loadAccountsFromFile();          // Load existing accounts
    loadRequestedAccountsFromFile(); // Load existing account requests
    loadTransactionsFromFile();
    loadCardRequests();
    loadCards();

    Admin admins[MAX_ADMINS];
    int numAdmins = readCredentials(admins, MAX_ADMINS);

    system("cls");
    printWelcomeMessage();
    sleep(3);

    int userType;
    while (1)
    {

        // Clear the screen
        clearScreen();

        // Loop for role selection
        printf("\n\033[1;36m=======================================\033[0m\n");
        printf("      \033[1;34mWelcome to the PSD BANK!\033[0m   \n");
        printf("\033[1;36m=======================================\033[0m\n");
        printf("      \033[1;33mSelect your role:\033[0m\n");
        printf("          \033[1;32m1. Customer\033[0m\n");
        printf("          \033[1;32m2. Admin\033[0m\n");
        printf("          \033[1;32m3. Exit\033[0m\n");
        printf("\033[1;36m=======================================\033[0m\n");
        printf("      \033[1;36mEnter your choice: \033[0m");
        scanf("%d", &userType);

        if (userType == 1)
        {
            // Customer Menu
            printf("\n\033[1;34mRedirecting to Customer Menu...\033[0m\n");
            customerMenu();
        }
        else if (userType == 2)
        {
            // Admin Menu with username and password
            char username[MAX_TITLE_LENGTH];
            char password[MAX_TITLE_LENGTH];
            system("cls");
            printf("Enter Admin Username: ");
            readPassword(username, sizeof(username));
            printf("Enter Admin Password: ");
            readPassword(password, sizeof(password));

            int loginSuccess = 0;
            for (int i = 0; i < numAdmins; i++)
            {
                if (strcmp(username, admins[i].username) == 0 && strcmp(password, admins[i].password) == 0)
                {
                    loginSuccess = 1;
                    break; // Exit loop on successful login
                }
            }

            if (loginSuccess)
            {
                adminMenu();
            }
            else
            {
                printf("\n\033[1;31mIncorrect username or password! Access denied.\033[0m\n");
            }
        }
        else if (userType == 3)
        {
            system("cls");
            printf("\n\033[1;32mExiting the system. Goodbye! Have a great day!\033[0m\n\n"); // Green color for exit message
            break;                                                                           // Exit the program
        }
        else
        {
            printf("\n\033[1;31m Invalid selection! Please try again.\033[0m\n"); // Red color for error
        }
    }

    // Save all changes to files before exiting
    saveAccountsToFile();
    saveRequestedAccountsToFile();
    saveTransactionsToFile();
    saveCardRequests();
    saveCards();

    return 0;
}