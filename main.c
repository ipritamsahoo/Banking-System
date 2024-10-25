#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100
#define MAX_REQUESTS 100
#define MAX_TITLE_LENGTH 100
#define MAX_ADMINS 10
#define FILENAME "accounts.txt"
#define FILENAME_1 "accountRequests.txt"
#define FILENAME_CARDS "cards.txt"
#define FILENAME_REQUESTS "cardRequests.txt"

// Structure for account creation requests
struct AccountRequest
{
    int requestID;
    char firstName[100];
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
        fprintf(file, "%d %s %s %s %s %s %s %s %s %.2f %d %s\n", accounts[i].accountNumber, accounts[i].firstName, accounts[i].lastName, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo, accounts[i].password, accounts[i].transactionPin, accounts[i].status, accounts[i].balance, accounts[i].securityQuestion, accounts[i].securityAnswer);
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
        fprintf(file, "%d %s %s %s %s %s %s %s %s %d %s\n", accountRequests[i].requestID, accountRequests[i].firstName, accountRequests[i].lastName, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo, accountRequests[i].password, accountRequests[i].transactionPin, accountRequests[i].status, accountRequests[i].securityQuestion, accountRequests[i].securityAnswer);
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

    while (fscanf(file, "%d %s %s %s %s %s %s %s %s %f %d %s", &accounts[accountCount].accountNumber, accounts[accountCount].firstName, accounts[accountCount].lastName, accounts[accountCount].mobileNo, accounts[accountCount].aadharNo, accounts[accountCount].panNo, accounts[accountCount].password, accounts[accountCount].transactionPin, &accounts[accountCount].status, &accounts[accountCount].balance, &accounts[accountCount].securityQuestion, accounts[accountCount].securityAnswer) != EOF)
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

    while (fscanf(file, "%d %s %s %s %s %s %s %s %s %d %s", &accountRequests[requestCount].requestID, accountRequests[requestCount].firstName, accountRequests[requestCount].lastName, accountRequests[requestCount].mobileNo, accountRequests[requestCount].aadharNo, accountRequests[requestCount].panNo, accountRequests[requestCount].password, accountRequests[requestCount].transactionPin, accountRequests[requestCount].status, &accountRequests[requestCount].securityQuestion, accountRequests[requestCount].securityAnswer) != EOF)
    {
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

    newRequest.requestID = requestCount + 1; // Assigning unique request ID
    printf("Enter Your First Name: ");
    scanf("%s", newRequest.firstName);

    printf("Enter Your Last Name: ");
    scanf("%s", newRequest.lastName);

    printf("Enter Your Mobile No: ");
    scanf("%s", newRequest.mobileNo);

    printf("Enter Your Aadhar No: ");
    scanf("%s", newRequest.aadharNo);

    printf("Enter Your PAN No: ");
    scanf("%s", newRequest.panNo);

    // Password confirmation loop
    while (1)
    {
        printf("Set a Password for Your Account: ");
        scanf("%s", newRequest.password);

        printf("Confirm Your Password: ");
        scanf("%s", confirmPassword);

        if (strcmp(newRequest.password, confirmPassword) == 0)
        {
            break; // Passwords match, proceed
        }
        else
        {
            printf("Passwords do not match! Please retype your password.\n");
        }
    }

    // Transaction PIN confirmation loop
    while (1)
    {
        printf("Set a 4-digit Transaction PIN for Your Account: ");
        scanf("%s", transactionPin);

        if (strlen(transactionPin) != 4)
        {
            printf("Invalid PIN! Please enter a 4-digit PIN.\n");
            continue;
        }

        printf("Confirm Your Transaction PIN: ");
        scanf("%s", confirmTransactionPin);

        if (strcmp(transactionPin, confirmTransactionPin) == 0)
        {
            strcpy(newRequest.transactionPin, transactionPin); // Store the confirmed PIN
            break;                                             // PINs match, proceed
        }
        else
        {
            printf("Transaction PINs do not match! Please retype your PIN.\n");
        }
    }

    // Security question section
    printf("\nChoose a Security Question for Account Recovery:\n");
    printf("1. What is your mother's maiden name?\n");
    printf("2. What was the name of your first pet?\n");
    printf("3. What is the name of the city where you were born?\n");
    printf("4. Who is your favorit singer?\n");
    printf("Enter your choice: ");
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
    default:
        printf("Invalid choice! Please select a valid option.\n");
        return; // Exit if invalid option is selected
    }

    // Prompt for the answer to the selected security question
    printf("Enter the answer to your security question: ");
    scanf("%s", newRequest.securityAnswer);

    strcpy(newRequest.status, "Pending"); // Initial status is pending

    // Save the request
    accountRequests[requestCount++] = newRequest;
    printf("Account Creation Request Submitted!\n\n");
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
        printf("No pending requests found.\n"); // Message if no pending requests are found
        return 0;
    }

    printf("Pending Account Creation Requests:\nRequest ID\tName\tMobile No\tAadhar No\tPAN No\n");

    for (int i = 0; i < requestCount; i++)
    {
        if (strcmp(accountRequests[i].status, "Pending") == 0)
        {
            printf(" %d\t\t%s %s\t%s\t%s\t%s\n",
                   accountRequests[i].requestID, accountRequests[i].firstName, accountRequests[i].lastName, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo);
        }
    }
    return 0;
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
            printf("%s\t%s %s\t%s\t%s\n", accountRequests[i].firstName, accountRequests[i].lastName, accountRequests[i].mobileNo, accountRequests[i].aadharNo, accountRequests[i].panNo);
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
                strcpy(newAccount.firstName, accountRequests[i].firstName);
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
    printf("Approved Accounts:\n\nAccount No\tName\tMobile No\tAadhar No\tPAN No\n");
    for (int i = 0; i < accountCount; i++)
    {
        printf("XXXXXXXX%d\t%s %s\t%s\t%s\t%s\n",
               accounts[i].accountNumber, accounts[i].firstName, accounts[i].lastName, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo);
    }
    return 0;
}

// Function for admin menu
void adminMenu()
{
    int choice;
    while (1)
    {
        printf("Admin Menu:\n");
        printf("1. View Pending Account Requests\n");
        printf("2. Approve/Reject Account Requests\n");
        printf("3. View All Accounts\n");
        printf("4. View Pending Card Requests\n");
        printf("5. Approve/Reject Card Requests\n");
        printf("6. View Pending Accounts to Unfreeze\n");
        printf("7. Freeze/Unfreeze an Account\n");
        printf("8. Delete an Account\n");
        printf("9. Log Out\n");
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
            viewPendingCardRequests();
            break;
        case 5:
            approveOrRejectCardRequest();
            break;
        case 6:
            viewPendingUnfreezeRequests();
            break;
        case 7:
            freezeOrUnfreezeAccount();
            break;
        case 8:
            deleteAccount();
            break;
        case 9:
            return; // Log out
        default:
            printf("Invalid choice! Please try again.\n");
        }

        printf("\nPress enter to return to the previous menu...");
        getchar();
        getchar();
    }
}

// Function for customer login
int customerLogin()
{
    char mobileNo[100], password[50];

    printf("Enter Your Mobile Number: ");
    scanf("%s", mobileNo);

    int index = findAccountByMobileOnly(mobileNo); // Find by mobile number only
    if (index == -1)
    {
        printf("Account with this mobile number not found!\n");
        return 0;
    }

    while (1)
    {
        printf("Enter Your Password: ");
        scanf("%s", password);

        index = findAccountByPassword(password);
        if (index == -1)
        {
            printf("Invalid password!\n");

            // Provide forgot password option after failed login
            int choice;
            printf("1. Forgot Password\n2. Try Again\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Forgot password flow
                // Ask the security question
                index = findAccountByMobileOnly(mobileNo);
                displaySecurityQuestion(accounts[index].securityQuestion);
                char answer[100];
                printf("Enter Your Answer: ");
                scanf("%s", answer);

                if (strcmp(accounts[index].securityAnswer, answer) == 0)
                {
                    // Allow resetting the password
                    char newPassword[50], confirmPassword[50];
                    while (1)
                    {
                        printf("Enter a New Password: ");
                        scanf("%s", newPassword);
                        printf("Confirm Your New Password: ");
                        scanf("%s", confirmPassword);

                        if (strcmp(newPassword, confirmPassword) == 0)
                        {
                            strcpy(accounts[index].password, newPassword); // Update password
                            strcpy(accountRequests[index].password, newPassword);
                            printf("Password reset successful! You can now log in with your new password.\n");
                            return 1;
                        }
                        else
                        {
                            printf("Passwords do not match! Please try again.\n");
                        }
                    }
                }
                else
                {
                    printf("Incorrect answer to the security question. Password reset failed.\n");
                    return 0;
                }
            }
            else if (choice == 2)
            {
                // Allow user to try again
                continue;
            }
            else
            {
                printf("Invalid choice! Please try again.\n");
            }
        }
        else if (strcmp(accounts[index].status, "Frozen") == 0)
        {
            printf("Your account is currently frozen. Please contact the admin.\n");
            int choice;
            // Ask if they want to request to unfreeze the account
            printf("Do you want to request to unfreeze your account?\n1. Yes\n2. No\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Send request to unfreeze
                FILE *file = fopen("unfreezeRequests.txt", "a");
                if (file == NULL)
                {
                    printf("Error opening unfreeze request file!\n");
                    return 0;
                }
                fprintf(file, "%d %s %s\n", accounts[index].accountNumber, accounts[index].firstName, accounts[index].lastName); // Store account number and name
                fclose(file);
                printf("Your request to unfreeze your account has been submitted.\n");
                return 0;
            }
            else
            {
                printf("No request submitted.\n");
                return 0;
            }
        }
        else
        {
            printf("Login successful!\n");
            customerPostLoginMenu(&accounts[index]);
            return 1;
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
        printf("What is your mother's maiden name?\n");
        break;
    case 2:
        printf("What was the name of your first pet?\n");
        break;
    case 3:
        printf("What is the name of the city where you were born?\n");
        break;
    case 4:
        printf("Who is your favorit singer?\n");
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

// Function to transfer money from one account to another
void transferMoney(struct Account *loggedInCustomer)
{
    int receiverAccountNumber;
    float amount;
    char enteredPin[10];

    printf("Enter The Last 4 Digits of The Receiver's Account Number: ");
    scanf("%d", &receiverAccountNumber);

    if (loggedInCustomer->accountNumber == receiverAccountNumber)
    {
        printf("Invalid Account !!!");
        return;
    }

    // Find receiver's account
    int receiverIndex = -1;
    for (int i = 0; i < accountCount; i++)
    {
        if (accounts[i].accountNumber == receiverAccountNumber)
        {
            receiverIndex = i;
            break;
        }
    }

    // Check if receiver's account exists
    if (receiverIndex == -1)
    {
        printf("Receiver's account not found!\n");
        return;
    }

    printf("Enter Amount to Transfer: ");
    scanf("%f", &amount);

    // Check if sender has sufficient balance
    if (loggedInCustomer->balance < amount)
    {
        printf("Insufficient balance!\n");
        return;
    }

    // Verify transaction PIN
    while (1)
    {
        printf("Enter your 4-digit transaction PIN: ");
        scanf("%s", enteredPin);

        if (strcmp(loggedInCustomer->transactionPin, enteredPin) != 0)
        {
            printf("Invalid transaction PIN!\n");
            // return;

            // Provide forgot PIN option after failed transaction
            int choice;
            printf("1. Forgot Transaction PIN\n2. Try Again\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Forgot PIN flow
                // Ask the security question
                int index = findAccountByMobileOnly(loggedInCustomer->mobileNo);
                if (index == -1)
                {
                    printf("Account not found for mobile number.\n");
                    return;
                }

                // Display the security question
                displaySecurityQuestion(accounts[index].securityQuestion);
                char answer[100];
                printf("Enter Your Answer: ");
                scanf("%s", answer);

                // Check the answer
                if (strcmp(accounts[index].securityAnswer, answer) == 0)
                {
                    // Allow resetting the PIN
                    char newPIN[10], confirmPIN[10];
                    while (1)
                    {
                        printf("Enter a New 4-digit PIN: ");
                        scanf("%s", newPIN);
                        printf("Confirm Your New PIN: ");
                        scanf("%s", confirmPIN);

                        if (strcmp(newPIN, confirmPIN) == 0)
                        {
                            strcpy(accounts[index].transactionPin, newPIN); // Update transaction PIN
                            printf("PIN reset successful!\n");
                            break;
                        }
                        else
                        {
                            printf("PINs do not match! Please try again.\n");
                        }
                    }
                }
                else
                {
                    printf("Incorrect answer to the security question. PIN reset failed.\n");
                    return;
                }
            }
            else if (choice == 2)
            {
                continue; // Allow user to retry PIN entry
            }
            else
            {
                printf("Invalid choice! Please try again.\n");
            }
        }
        else
        {
            break; // PIN is correct
        }
    }

    // Perform transfer
    loggedInCustomer->balance -= amount;
    accounts[receiverIndex].balance += amount;

    // Record the transaction
    transactions[transactionCount].senderAccountNumber = loggedInCustomer->accountNumber;
    transactions[transactionCount].receiverAccountNumber = accounts[receiverIndex].accountNumber;
    transactions[transactionCount].amount = amount;
    transactionCount++;

    printf("Transfer successful! New balance: %.2f\n", loggedInCustomer->balance);

    // Save updated accounts and transactions to file
    saveAccountsToFile();
    saveTransactionsToFile();
}

// Function to view transaction history
void viewTransactionHistory(struct Account *loggedInCustomer)
{
    printf("\nTransaction History for Account Number XXXXXXXX%d:\n", loggedInCustomer->accountNumber);
    printf("--------------------------------------------------------------------\n");
    printf("|   Type   |     Amount    |   Related Account   |\n");
    printf("--------------------------------------------------------------------\n");

    for (int i = 0; i < transactionCount; i++)
    {
        if (transactions[i].senderAccountNumber == loggedInCustomer->accountNumber)
        {
            // This is a debit transaction
            printf("|  Debit   |    %.2f     |  To   XXXXXXXX%d   |\n", transactions[i].amount, transactions[i].receiverAccountNumber);
        }
        else if (transactions[i].receiverAccountNumber == loggedInCustomer->accountNumber)
        {
            // This is a credit transaction
            printf("|  Credit  |    %.2f     |  From XXXXXXXX%d   |\n", transactions[i].amount, transactions[i].senderAccountNumber);
        }
    }

    printf("--------------------------------------------------------------------\n");
}

// Function to update customer profile
void updateProfile(struct Account *loggedInCustomer)
{
    int choice;
    printf("\nUpdate Your Profile:\n");
    printf("1. Update Name\n");
    printf("2. Update Mobile Number\n");
    printf("3. Update Aadhar Number\n");
    printf("4. Cancel\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        printf("Enter New First Name: ");
        scanf("%s", loggedInCustomer->firstName);
        printf("Enter New Last Name: ");
        scanf("%s", loggedInCustomer->lastName);
        printf("Name updated successfully!\n");
        break;
    case 2:
        printf("Enter New Mobile Number: ");
        scanf("%s", loggedInCustomer->mobileNo);
        printf("Mobile Number updated successfully!\n");
        break;
    case 3:
        printf("Enter New Aadhar Number: ");
        scanf("%s", loggedInCustomer->aadharNo);
        printf("Mobile Number updated successfully!\n");
        break;
    case 4:
        printf("Profile update canceled.\n");
        return;
    default:
        printf("Invalid choice! Please try again.\n");
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
        printf("\nCustomer Menu:\n");
        printf("1. View Account Details\n");
        printf("2. Check Balance\n");
        printf("3. Money Transfer\n");
        printf("4. View Transactions\n");
        printf("5. My Cards\n");
        printf("6. Update Profile\n");
        printf("7. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Account Number: XXXXXXXX%d\nName: %s %s\nMobile No: %s\n",
                   loggedInCustomer->accountNumber, loggedInCustomer->firstName, loggedInCustomer->lastName, loggedInCustomer->mobileNo);
            break;
        case 2:
            printf("Current Balance: %.2f\n", loggedInCustomer->balance);
            break;
        case 3:
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
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }

        printf("\nPress enter to return to the previous menu...");
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
        printf("\nYour Menu:\n");
        printf("1. Login\n");
        printf("2. Sign Up (Request Account Creation)\n");
        printf("3. Go To Home\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            if (customerLogin())
            {
                // Successful login, proceed to post-login menu
            }
            break;
        case 2:
            requestAccountCreation();
            break;
        case 3:
            // Return to the main menu (exit customer menu loop)
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

// Function to request a card (customer side)
void requestCard(struct Account *loggedInCustomer)
{
    struct CardRequest newCardRequest;
    newCardRequest.requestID = cardRequestCount + 1;
    newCardRequest.accountNumber = loggedInCustomer->accountNumber;

    printf("Request a Card:\n1. Debit\n2. Credit\nEnter your choice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1)
    {
        strcpy(newCardRequest.cardType, "Debit");
    }
    else if (choice == 2)
    {
        strcpy(newCardRequest.cardType, "Credit");
    }
    else
    {
        printf("Invalid choice!\n");
        return;
    }

    strcpy(newCardRequest.status, "Pending");

    // Save the request
    cardRequests[cardRequestCount++] = newCardRequest;
    saveCardRequests(); // Save the updated requests to file
    printf("Card Request Submitted!\n\n");
}

// Function for admin to view all pending card requests
void viewPendingCardRequests()
{
    int foundCardRequest = 0;

    for (int i = 0; i < cardRequestCount; i++)
    {
        foundCardRequest = 1;
    }
    if (!foundCardRequest)
    {
        printf("No card request found!");
        return;
    }
    printf("Pending Card Requests:\nRequest ID\tAccount Number\tCard Type\n");
    for (int i = 0; i < cardRequestCount; i++)
    {
        if (strcmp(cardRequests[i].status, "Pending") == 0)
        {
            printf(" %d\t\t%d\t\t%s\n",
                   cardRequests[i].requestID, cardRequests[i].accountNumber, cardRequests[i].cardType);
        }
    }
    return;
}

// Function for admin to approve or reject card requests
void approveOrRejectCardRequest()
{
    int requestID, choice;
    printf("Enter Card Request ID to Approve/Reject: ");
    scanf("%d", &requestID);

    for (int i = 0; i < cardRequestCount; i++)
    {
        if (cardRequests[i].requestID == requestID && strcmp(cardRequests[i].status, "Pending") == 0)
        {
            printf("Request Details:\nAccount Number: %d\nCard Type: %s\n",
                   cardRequests[i].accountNumber, cardRequests[i].cardType);
            printf("1. Approve\n2. Reject\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                // Approve the card request
                struct Card newCard;
                newCard.accountNumber = cardRequests[i].accountNumber;
                strcpy(newCard.cardType, cardRequests[i].cardType);
                generateCardDetails(&newCard);

                strcpy(newCard.cardStatus, "Active");
                cards[cardCount++] = newCard;
                saveCards(); // Save the approved card to file

                strcpy(cardRequests[i].status, "Approved"); // Update request status
                saveCardRequests();                         // Save updated requests to file
                printf("Card Request Approved!");
            }
            else if (choice == 2)
            {
                strcpy(cardRequests[i].status, "Rejected"); // Update request status
                saveCardRequests();                         // Save updated requests to file
                printf("Card Request Rejected!\n\n");
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
        printf("\n==== Card Management ====\n");
        printf("1. Request a New Card\n");
        printf("2. Manage Your Cards\n");
        printf("3. Go To Previous Menu\n");
        printf("Enter your choice: ");
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
                printf("\n==== Manage Your Cards ====\n");

                // Loop through all cards linked to the customer's account
                for (int i = 0; i < cardCount; i++)
                {
                    if (cards[i].accountNumber == accountNumber)
                    {

                        int manageChoice;
                        while (1) // Loop to stay in card management until the user chooses to exit
                        {
                            printf("Card Details:\n");
                            printf("Card Number: XXXX XXXX %d\n", cards[i].cardNumber);
                            printf("Card Expiry Date: 12/31");
                            printf("CVV: %d\n", cards[i].cvv);
                            printf("Card Type: %s\n", cards[i].cardType);
                            printf("Status: %s\n", cards[i].cardStatus);
                            printf("\nOptions:\n");
                            printf("1. Generate/Change PIN\n");
                            printf("2. Block/Unblock Card\n");
                            printf("3. Go to Previous Menu\n");
                            printf("Enter your choice: ");
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
                                printf("Invalid choice! Please try again.\n");
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                printf("No cards linked to your account.\n");
            }
            break;
        }

        case 3:
            // Return to previous menu
            return;

        default:
            printf("Invalid choice! Please enter a valid option.\n");
            break;
        }

    mainMenu:; // Label to break out of the inner loop and return to the main menu
    }
}

// Function to change or generate the card PIN
void changeCardPin(struct Card *card)
{
    // If the card has no PIN set initially, prompt to create a new PIN
    if (strcmp(card->cardPin, "0") == 0)
    {
        printf("No PIN is currently set for this card.\n");
        char newPin[6], confirmPin[6];

        // Loop to ensure the user sets a valid PIN
        while (1)
        {
            printf("Enter a new 4-digit PIN: ");
            scanf("%s", newPin);

            // Check if the entered PIN is 4 digits long
            if (strlen(newPin) == 4)
            {
                printf("Confirm the new PIN: ");
                scanf("%s", confirmPin);

                // Check if the confirmed PIN matches
                if (strcmp(newPin, confirmPin) == 0)
                {
                    strcpy(card->cardPin, newPin); // Set the new PIN
                    saveCards();                   // Save updated PIN to file
                    printf("PIN successfully created!\n");
                    break;
                }
                else
                {
                    printf("PINs do not match. Please try again.\n");
                }
            }
            else
            {
                printf("Invalid PIN format. Must be exactly 4 digits.\n");
            }
        }
    }
    else
    {
        // If the card already has a PIN, ask the customer to enter the current PIN to change it
        char currentPin[6];
        printf("Enter current PIN: ");
        scanf("%s", currentPin);

        // Check if the current PIN matches
        if (strcmp(card->cardPin, currentPin) == 0)
        {
            char newPin[6], confirmPin[6];

            // Loop to ensure the user sets a valid new PIN
            while (1)
            {
                printf("Enter a new 4-digit PIN: ");
                scanf("%s", newPin);

                // Check if the entered new PIN is 4 digits long
                if (strlen(newPin) == 4)
                {
                    printf("Confirm the new PIN: ");
                    scanf("%s", confirmPin);

                    // Check if the confirmed new PIN matches
                    if (strcmp(newPin, confirmPin) == 0)
                    {
                        strcpy(card->cardPin, newPin); // Update the PIN
                        saveCards();                   // Save updated PIN to file
                        printf("PIN successfully changed!\n");
                        break;
                    }
                    else
                    {
                        printf("PINs do not match. Please try again.\n");
                    }
                }
                else
                {
                    printf("Invalid PIN format. Must be exactly 4 digits.\n");
                }
            }
        }
        else
        {
            printf("Incorrect current PIN!\n");
        }
    }
}

// Function to block or unblock a card
void blockOrUnblockCard(struct Card *card)
{
    if (strcmp(card->cardStatus, "Active") == 0)
    {
        printf("Card is currently Active.\n");
        printf("Do you want to:\n1. Block Temporarily\n2. Block Permanently\nEnter your choice: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) // Temporarily block the card
        {
            strcpy(card->cardStatus, "Temporarily Blocked");
            saveCards(); // Save updated status to file
            printf("Card has been temporarily blocked.\n");
        }
        else if (choice == 2) // Permanently block the card
        {
            strcpy(card->cardStatus, "Permanently Blocked");
            saveCards(); // Save updated status to file
            printf("Card has been permanently blocked. It cannot be unblocked.\n");
        }
        else
        {
            printf("Invalid choice!\n");
        }
    }
    else if (strcmp(card->cardStatus, "Temporarily Blocked") == 0)
    {
        printf("Card is currently Temporarily Blocked.\n");
        printf("Do you want to unblock the card? (1 for Yes, 0 for No): ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) // Unblock the card
        {
            strcpy(card->cardStatus, "Active");
            saveCards(); // Save updated status to file
            printf("Card has been unblocked and is now Active.\n");
        }
    }
    else if (strcmp(card->cardStatus, "Permanently Blocked") == 0)
    {
        printf("Card is Permanently Blocked. It cannot be unblocked.\n");
    }
    else
    {
        printf("Card status is unknown.\n");
    }
}

// Function to delete an account (admin-only)
void deleteAccount()
{
    int accountNumber;
    char confirmChoice;

    printf("Enter Account Number to delete: ");
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
        printf("Account found: %s %s (Account No: XXXXXXXX%d)\n", accounts[found].firstName, accounts[found].lastName, accounts[found].accountNumber);
        printf("Are you sure you want to delete this account?\n");
        printf("1. Yes\n");
        printf("2. No\n");
        printf("Enter your choice: ");
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
            printf("Account successfully deleted.\n");
        }
        else if (confirmChoice == '2') // User choose "No"
        {
            printf("Account deletion cancelled.\n");
        }
        else
        {
            printf("Invalid choice! Account deletion cancelled.\n");
        }
    }
    else
    {
        printf("Account not found!\n");
    }
}

// Function to freeze or unfreeze an account (admin-only)
void freezeOrUnfreezeAccount()
{
    int accountNumber, choice;
    printf("Enter Account Number to freeze/unfreeze: ");
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
        // Check the current status of the account
        printf("Account found: %s %s (Account No: XXXXXXXX%d)\n", accounts[found].firstName, accounts[found].lastName, accounts[found].accountNumber);
        printf("Current Status: %s\n", accounts[found].status);
        if (strcmp(accounts[found].status, "Active") == 0)
        {
            // If account is active, allow freezing
            printf("1. Freeze Account\n2. Cancel\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                strcpy(accounts[found].status, "Frozen"); // Set account status to Frozen
                saveAccountsToFile();
                printf("Account has been frozen.\n");
            }
            else
            {
                printf("Operation cancelled.\n");
            }
        }
        else if (strcmp(accounts[found].status, "Frozen") == 0)
        {
            // If account is frozen, allow unfreezing
            printf("1. Unfreeze Account\n2. Cancel\nEnter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                strcpy(accounts[found].status, "Active"); // Set account status back to Active
                saveAccountsToFile();
                printf("Account has been unfrozen.\n");
            }
            else
            {
                printf("Operation cancelled.\n");
            }
        }
        else
        {
            printf("Unknown account status!\n");
        }
    }
    else
    {
        printf("Account not found!\n");
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

    while (fscanf(file, "%d %s", &accountNumber, name) != EOF)
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

int main()
{
    loadAccountsFromFile();          // Load existing accounts
    loadRequestedAccountsFromFile(); // Load existing account requests
    loadTransactionsFromFile();
    loadCardRequests();
    loadCards();

    Admin admins[MAX_ADMINS];
    int numAdmins = readCredentials(admins, MAX_ADMINS);

    int userType;
    while (1)
    { // Loop for role selection
        printf("Welcome to the PSD Bank!\n");
        printf("Select your role:\n1. Customer\n2. Admin\n3. Exit\nEnter your choice: ");
        scanf("%d", &userType);

        if (userType == 1)
        {
            // Customer Menu
            customerMenu();
        }
        else if (userType == 2)
        {
            // Admin Menu with username and password
            char username[MAX_TITLE_LENGTH];
            char password[MAX_TITLE_LENGTH];

            printf("Enter Admin Username: ");
            scanf("%s", username);
            printf("Enter Admin Password: ");
            scanf("%s", password);

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
                printf("Incorrect username or password! Access denied.\n");
            }
        }
        else if (userType == 3)
        {
            printf("Exiting the system. Goodbye!\n");
            break; // Exit the program
        }
        else
        {
            printf("Invalid selection! Please try again.\n");
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
