#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <time.h>

#define MAX_ACCOUNTS 100
#define MAX_REQUESTS 100
#define FILENAME "accounts.txt"
#define FILENAME_1 "accountRequests.txt"
#define FILENAME_CARDS "cards.txt"
#define FILENAME_REQUESTS "cardRequests.txt"
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
    char password[50];
    char transactionPin[6];
    float balance;
    char status[10]; // "Active", "Frozen"
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
void customerPostLoginMenu(struct Account *loggedInCustomer);
void saveAccountsToFile();
void saveRequestedAccountsToFile();
void loadAccountsFromFile();
void loadRequestedAccountsFromFile();
void loadTransactionsFromFile();
void saveTransactionsToFile();
int findAccountByMobile(char *mobileNo, char *password);
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
        fprintf(file, "%d %s %s %s %s %s %s %s %.2f\n", accounts[i].accountNumber, accounts[i].name, accounts[i].mobileNo, accounts[i].aadharNo, accounts[i].panNo, accounts[i].password, accounts[i].transactionPin, accounts[i].status, accounts[i].balance);
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

    while (fscanf(file, "%d %s %s %s %s %s %s %s %f", &accounts[accountCount].accountNumber, accounts[accountCount].name, accounts[accountCount].mobileNo, accounts[accountCount].aadharNo, accounts[accountCount].panNo, accounts[accountCount].password, accounts[accountCount].transactionPin, &accounts[accountCount].status, &accounts[accountCount].balance) != EOF)
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

// Function to request account creation (customer side) with password and PIN confirmation
void requestAccountCreation()
{
    struct AccountRequest newRequest;
    char confirmPassword[50];      // For confirming the password
    char transactionPin[6];        // For storing the transaction PIN
    char confirmTransactionPin[6]; // For confirming the transaction PIN

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
                strcpy(newAccount.password, accountRequests[i].password);
                strcpy(newAccount.transactionPin, accountRequests[i].transactionPin);
                newAccount.balance = 0.0; // Initial balance
                strcpy(newAccount.status, "Active");

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
        printf("1. View Pending Account Requests\n");
        printf("2. Approve/Reject Account Requests\n");
        printf("3. View All Accounts\n");
        printf("4. View Pending Card Requests\n");
        printf("5. Approve/Reject Card Requests\n");
        printf("6. Freeze/Unfreeze an Account\n"); // New option for freezing/unfreezing accounts
        printf("7. Delete an Account\n");          // Option for deleting an account
        printf("8. Log Out\n");
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
            freezeOrUnfreezeAccount(); // Call freeze/unfreeze function
            break;
        case 7:
            deleteAccount(); // Call the function to delete an account
            break;
        case 8:
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
    printf("Enter Your Password: ");
    scanf("%s", password);

    int index = findAccountByMobile(mobileNo, password);
    if (index == -1)
    {
        printf("Invalid mobile number or password! Try again.\n");
        return 0;
    }
    else if (strcmp(accounts[index].status, "Frozen") == 0)
    {
        // Check if the account is frozen
        printf("Your account is currently frozen. Please contact the admin.\n");
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
    printf("Enter your 4-digit transaction PIN: ");
    scanf("%s", enteredPin);

    if (strcmp(loggedInCustomer->transactionPin, enteredPin) != 0)
    {
        printf("Invalid transaction PIN!\n");
        return;
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
        printf("6. Logout\n");
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
            transferMoney(loggedInCustomer);
            break;
        case 4:
            viewTransactionHistory(loggedInCustomer);
            break;
        case 5:
            customerCardManagement(loggedInCustomer);
            break;
        case 6:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }

        printf("\nPress enter to return to the previous menu...");
        getchar();
        getchar();
    }
}

// Function for customer menu (initial options)
void customerMenu()
{
    int choice;
    while (1)
    {
        printf("\nYour Menu:\n");
        printf("1. Request Account Creation\n");
        printf("2. Login\n");
        printf("3. Go To Home\n");
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
    printf("Pending Card Requests:\nRequest ID\tAccount Number\tCard Type\n");
    for (int i = 0; i < cardRequestCount; i++)
    {
        if (strcmp(cardRequests[i].status, "Pending") == 0)
        {
            printf(" %d\t\t%d\t\t%s\n",
                   cardRequests[i].requestID, cardRequests[i].accountNumber, cardRequests[i].cardType);
        }
    }
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
        printf("Account found: %s (Account No: XXXXXXXX%d)\n", accounts[found].name, accounts[found].accountNumber);
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

            // Save the updated accounts to file
            saveAccountsToFile();
            printf("Account successfully deleted.\n");
        }
        else if (confirmChoice == '2') // User chose "No"
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
        printf("Account found: %s (Account No: XXXXXXXX%d)\n", accounts[found].name, accounts[found].accountNumber);
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

// Main function with role selection
int main()
{
    loadAccountsFromFile();          // Load existing accounts
    loadRequestedAccountsFromFile(); // Load existing account requests
    loadTransactionsFromFile();
    loadCardRequests();
    loadCards();

    int userType;
    while (1) // This loop ensures that after exiting a menu, the user returns to the role selection.
    {
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
        else if (userType == 3)
        {
            printf("Exiting the system. Goodbye!\n");
            break; // This breaks the loop and exits the program when the user selects "Exit".
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
