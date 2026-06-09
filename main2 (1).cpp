#include <iostream>
#include <string>
#include <limits> // Required for numeric_limits
#include <ctime>  // Required for time functions (to get current date/time)
#include <cstdlib> // Required for rand() and srand()
#include "classes.h"
#include "FileManager.h" // Includes FileManager class methods

using namespace std;

// --- Forward Declarations of Menu Functions ---
void bankingEmployeeMenu(int employeeId);
void userClientMenu(int userId);
void companyClientMenu(int companyId);
void loginSelectionMenu(int userId);


// --- Helper Functions for Input and Data Conversion ---

// Function to safely get integer input
int getIntInput(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "X Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the rest of the line
    return value;
}

// Function to safely get string input
string getStringInput(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// Function to generate a random 16-digit card number (Simplification: uses 64-bit long long)
long long generateCardNumber() {
    // Note: Generating a truly unique 16-digit number is complex. 
    // This provides a simple random 16-digit number for the assignment.
    long long num = 0;
    srand(time(0));
    for (int i = 0; i < 16; ++i) {
        num = num * 10 + (rand() % 10);
    }
    return num;
}

// Function to get current date as a string (YYYY-MM-DD format)
string getCurrentDate() {
    time_t now = time(0);
    tm ltm;  // tm object 
    localtime_s(&ltm, &now);
    // tm* ltm = localtime(&now);
    string date = to_string(1900 + ltm.tm_year) + "-" +
        to_string(1 + ltm.tm_mon) + "-" +
        to_string(ltm.tm_mday);
    return date;
}

// --- Core Bank Operations (Encapsulating logic for menus) ---

// Handles the transaction logic for User and Company Clients (Withdrawal/Transfer)
bool performCardTransaction(int sourceId, const string& transactionType, int amount, int destId = 0) {
    // 1. Load User's Cards
    card userCards[10]; // Max 10 cards assumed based on userclient class
    int cardCount = 0;
    FileManager::loadCards(sourceId, userCards, cardCount, 10);

    if (cardCount == 0) {
        cout << "X No cards found for this account. Transaction declined.\n";
        return false;
    }

    cout << "\n--- Card Selection ---\n";
    for (int i = 0; i < cardCount; ++i) {
        cout << i + 1 << ". Card Number: " << userCards[i].getCardNumber() << endl;
    }
    int cardChoice = getIntInput("Select card (Enter number): ");

    if (cardChoice < 1 || cardChoice > cardCount) {
        cout << "X Invalid card selection. Transaction declined.\n";
        return false;
    }

    card selectedCard = userCards[cardChoice - 1];

    if (FileManager::checkCardFraud(selectedCard.getCardNumber())) {
        cout << "WARNING: This card is marked as fraudulent. Transaction declined.\n";
        return false;
    }

    // 2. PIN Verification Loop
    int declines = 0;
    bool pinVerified = false;
    while (declines < 3 && !pinVerified) {
        int enteredPin = getIntInput("Enter PIN for card " + to_string(selectedCard.getCardNumber()) + ": ");

        if (selectedCard.verifyPin(enteredPin)) {
            pinVerified = true;
        }
        else {
            declines++;
            cout << "X Incorrect PIN. (" << 3 - declines << " attempts remaining).\n";
        }
    }

    if (!pinVerified) {
        cout << "PIN declined 3 times. Marking card as fraudulent and freezing account.\n";
        FileManager::markCardFraud(selectedCard.getCardNumber());
        FileManager::freezeUser(sourceId);
        return false;
    }

    // 3. Perform Transaction (Logic based on sourceId type: User or Company)

    // Determine Account Type for loading and checking limits
    userclient u;
    companyclient c;
    account* acc = nullptr;
    string clientType;
    int currentBalance = 0;
    int limit = 0;

    // The assignment requires us to determine if the ID is a User or Company ID.
    // Assuming your system has a way to distinguish them (e.g., ID ranges or specific file checks).
    // For simplicity, we'll try loading both, but a robust design would need a common base ID check.

    // TRY AS USER
    u = FileManager::loadUser(sourceId);
    if (u.getLoginID() == sourceId && u.getAccount() != nullptr) {
        clientType = "User";
        acc = u.getAccount();
    }
    // TRY AS COMPANY
    else {
        c = FileManager::loadCompany(sourceId);
        if (c.getLoginID() == sourceId && c.getBusiness() != nullptr) {
            clientType = "Company";
            acc = c.getBusiness();
        }
    }

    if (acc == nullptr) {
        cout << "X Error: Account not found or account type invalid.\n";
        return false;
    }

    currentBalance = acc->getbalance();
    limit = acc->getlimit();

    if (FileManager::checkUserFrozen(sourceId)) {
        cout << "X Account is frozen. Transaction denied.\n";
        return false;
    }

    // 4. Apply Business Logic (Withdraw/Transfer checks)
    if (transactionType == "Withdraw" || transactionType == "Transfer") {
        if (amount > currentBalance) {
            cout << "X Insufficient funds. Current balance: " << currentBalance << " PKR\n";
            return false;
        }
        // Daily limit check requires more complex tracking (not explicitly provided in FileManager.h)
        // For now, we only check against the account limit type (Bronze/Gold/Business max limit)
        if (amount > limit) {
            cout << "X Amount exceeds account limit (" << limit << " PKR). Transaction denied.\n";
            return false;
        }
    }

    // 5. Finalize Transaction

    if (transactionType == "Withdraw") {
        currentBalance -= amount;

        // Update balance in file
        FileManager::updateBalance(sourceId, currentBalance);

        // Append transaction record
        transaction t(amount, getCurrentDate(), "", "Withdraw", sourceId, 0);
        FileManager::appendTransaction(t);

        cout << "Withdrawal successful. New balance: " << currentBalance << " PKR\n";
        return true;

    }
    else if (transactionType == "Transfer") {
        // Transfer requires two updates (sender and receiver)

        // Check if destination exists
        userclient destUser = FileManager::loadUser(destId);
        if (destUser.getLoginID() != destId) {
            cout << "X Destination User Client ID " << destId << " not found.\n";
            return false;
        }

        // Sender update
        currentBalance -= amount;
        FileManager::updateBalance(sourceId, currentBalance);

        // Receiver update (Assuming receiver is always a User Client)
        int destBalance = destUser.getAccount()->getbalance() + amount;
        FileManager::updateBalance(destId, destBalance);

        // Append transaction record for sender
        transaction t_out(amount, getCurrentDate(), "", "Transfer", sourceId, destId);
        FileManager::appendTransaction(t_out);

        // Append transaction record for receiver (simplified)
        transaction t_in(amount, getCurrentDate(), "", "Deposit(Transfer In)", destId, sourceId);
        FileManager::appendTransaction(t_in); // Note: Should probably be a Deposit type linked to sender

        cout << "Transfer successful. " << amount << " PKR sent to User ID " << destId << ". New balance: " << currentBalance << " PKR\n";
        return true;

    }
    else {
        cout << "X Invalid transaction type.\n";
        return false;
    }

    return true;
}


// --- Menu Function Definitions ---

void displayTransactionHistory(int accountId) {
    // We assume a reasonable max number of transactions to display
    transaction transHistory[100];
    int count = 0;
    FileManager::loadTransactionsByUser(accountId, transHistory, count, 100);

    cout << "\n--- Transaction History for ID " << accountId << " ---\n";
    if (count == 0) {
        cout << "No transactions found.\n";
        return;
    }

    cout << "Date\t\tType\t\tAmount (PKR)\tDestination ID\n";
    cout << "---------------------------------------------------------\n";
    for (int i = 0; i < count; ++i) {
        cout << transHistory[i].getDate() << "\t"
            << transHistory[i].getType() << "\t"
            << transHistory[i].getAmount() << "\t\t";

        if (transHistory[i].getType() == "Transfer") {
            cout << transHistory[i].getDestId();
        }
        else {
            cout << "N/A";
        }
        cout << endl;
    }
    cout << "---------------------------------------------------------\n";
}

// ------------------------------------------------------------
// 1. USER CLIENT MENU
// ------------------------------------------------------------
void userClientMenu(int userId) {
    int choice;
    do {
        cout << "\n======================================\n";
        cout << "     USER CLIENT MENU (ID: " << userId << ")    \n";
        cout << "======================================\n";
        cout << "1. View Account Balance & Details\n";
        cout << "2. Deposit Funds\n";
        cout << "3. Withdraw Funds\n";
        cout << "4. Transfer Funds to another User Client\n";
        cout << "5. View Transaction History\n";
        cout << "0. Logout\n";
        choice = getIntInput("Enter your choice: ");

        userclient u = FileManager::loadUser(userId);
        if (u.getLoginID() != userId || FileManager::checkUserFrozen(userId)) {
            cout << "\nWARNING: Account state changed (frozen/removed). Logging out.\n";
            return;
        }

        account* acc = u.getAccount();
        if (acc == nullptr) {
            cout << "\nWARNING: Error: Account object missing. Logging out.\n";
            return;
        }

        switch (choice) {
        case 1: {
            cout << "\n--- Account Details ---\n";
            cout << "Name: " << u.getName() << endl;
            cout << "Address: " << u.getAddress() << endl;
            cout << "CNIC: " << u.getCnic() << endl;
            cout << "Account ID: " << u.getLoginID() << endl;

            string accType = (acc->getlimit() == 100000) ? "Bronze" :
                (acc->getlimit() == 500000) ? "Gold" : "Unknown";
            cout << "Account Type: " << accType << endl;
            cout << "Daily Withdrawal Limit: " << acc->getlimit() << " PKR" << endl;
            cout << "**CURRENT BALANCE: " << acc->getbalance() << " PKR**\n";

            // Display cards
            card userCards[10];
            int cardCount = 0;
            FileManager::loadCards(userId, userCards, cardCount, 10);
            cout << "Associated Cards (" << cardCount << " total):\n";
            for (int i = 0; i < cardCount; ++i) {
                cout << "- Card No: " << userCards[i].getCardNumber()
                    << (FileManager::checkCardFraud(userCards[i].getCardNumber()) ? " (FRAUDULENT)" : "") << endl;
            }
            break;
        }
        case 2: {
            int amount = getIntInput("Enter amount to deposit (PKR): ");
            if (amount <= 0) {
                cout << "X Deposit amount must be positive.\n";
                break;
            }
            int newBalance = acc->getbalance() + amount;

            // Update file
            if (FileManager::updateBalance(userId, newBalance)) {
                // Log transaction
                transaction t(amount, getCurrentDate(), "", "Deposit", userId, 0);
                FileManager::appendTransaction(t);
                cout << "Deposit successful. New balance: " << newBalance << " PKR\n";
            }
            else {
                cout << "X Deposit failed due to file error.\n";
            }
            break;
        }
        case 3: {
            int amount = getIntInput("Enter amount to withdraw (PKR): ");
            if (amount <= 0) {
                cout << "X Withdrawal amount must be positive.\n";
                break;
            }
            performCardTransaction(userId, "Withdraw", amount);
            break;
        }
        case 4: {
            int amount = getIntInput("Enter amount to transfer (PKR): ");
            if (amount <= 0) {
                cout << "X Transfer amount must be positive.\n";
                break;
            }
            int destId = getIntInput("Enter destination User Client ID: ");
            performCardTransaction(userId, "Transfer", amount, destId);
            break;
        }
        case 5: {
            displayTransactionHistory(userId);
            break;
        }
        case 0:
            cout << "\nLogging out User Client...\n";
            break;
        default:
            cout << "\nWARNING: Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);
}

// ------------------------------------------------------------
// 2. COMPANY CLIENT MENU
// ------------------------------------------------------------
void companyClientMenu(int companyId) {
    int choice;
    do {
        cout << "\n======================================\n";
        cout << "    COMPANY CLIENT MENU (ID: " << companyId << ")  \n";
        cout << "======================================\n";
        cout << "1. View Account Balance & Details\n";
        cout << "2. Deposit Funds\n";
        cout << "3. Withdraw Funds\n";
        cout << "4. Transfer Funds to a User Client\n";
        cout << "5. View Transaction History\n";
        cout << "6. Request Loan (Placeholder)\n";
        cout << "0. Logout\n";
        choice = getIntInput("Enter your choice: ");

        companyclient c = FileManager::loadCompany(companyId);
        if (c.getLoginID() != companyId) {
            cout << "\nWARNING: Error: Company Account state changed. Logging out.\n";
            return;
        }

        business* acc = c.getBusiness();
        if (acc == nullptr) {
            cout << "\nWARNING: Error: Business Account object missing. Logging out.\n";
            return;
        }

        switch (choice) {
        case 1: {
            cout << "\n--- Company Account Details ---\n";
            cout << "Company Name: " << c.getCompanyName() << endl;
            cout << "Address: " << c.getAddress() << endl;
            cout << "Tax No: " << c.getTaxno() << endl;
            cout << "Account ID: " << c.getLoginID() << endl;
            cout << "Account Type: Business" << endl;
            cout << "Daily Withdrawal Limit: " << acc->getlimit() << " PKR" << endl;
            cout << "**CURRENT BALANCE: " << acc->getbalance() << " PKR**\n";
            break;
        }
        case 2: {
            int amount = getIntInput("Enter amount to deposit (PKR): ");
            if (amount <= 0) {
                cout << "X Deposit amount must be positive.\n";
                break;
            }
            int newBalance = acc->getbalance() + amount;

            if (FileManager::updateBalance(companyId, newBalance)) {
                transaction t(amount, getCurrentDate(), "", "Deposit", companyId, 0);
                FileManager::appendTransaction(t);
                cout << "Deposit successful. New balance: " << newBalance << " PKR\n";
            }
            else {
                cout << "X Deposit failed due to file error.\n";
            }
            break;
        }
        case 3: {
            // Assuming Company accounts also use card/PIN for employee access control (best practice)
            int amount = getIntInput("Enter amount to withdraw (PKR): ");
            if (amount <= 0) {
                cout << "X Withdrawal amount must be positive.\n";
                break;
            }
            // Note: The logic inside performCardTransaction needs to handle this ID as a Company ID
            performCardTransaction(companyId, "Withdraw", amount);
            break;
        }
        case 4: {
            int amount = getIntInput("Enter amount to transfer (PKR): ");
            if (amount <= 0) {
                cout << "X Transfer amount must be positive.\n";
                break;
            }
            // Company can only transfer to User Clients
            int destId = getIntInput("Enter destination User Client ID: ");
            performCardTransaction(companyId, "Transfer", amount, destId);
            break;
        }
        case 5: {
            displayTransactionHistory(companyId);
            break;
        }
        case 6: {
            // Placeholder for loan request
            cout << "\n--- Loan Request ---\n";
            // In a real system, this would write a loan request to a 'loans.txt' file for the employee to approve.
            // Since this is a simple menu, we just output a confirmation.
            cout << "Loan request for Company ID " << companyId << " has been submitted for review.\n";
            // You should implement a function like FileManager::saveLoanRequest(companyId, amount);
            break;
        }
        case 0:
            cout << "\nLogging out Company Client...\n";
            break;
        default:
            cout << "\nWARNING: Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);
}

// ------------------------------------------------------------
// 3. BANKING EMPLOYEE MENU
// ------------------------------------------------------------
void bankingEmployeeMenu(int employeeId) {
    int choice;
    do {
        cout << "\n======================================\n";
        cout << "      BANKING EMPLOYEE MENU (ID: " << employeeId << ")\n";
        cout << "======================================\n";
        cout << "1. View All Client Accounts (Placeholder)\n"; // Requires iteration over files
        cout << "2. Approve/Reject New User Account Applications\n";
        cout << "3. Approve/Reject Loan Requests (Placeholder)\n";
        cout << "4. View Transaction History for a Specific Account\n";
        cout << "5. Freeze or Close Account\n";
        cout << "0. Logout\n";
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
        case 1: {
            cout << "\nWARNING: This feature requires reading and iterating over all entries in users.txt and companies.txt.\n";
            cout << "   (Implementation depends on adding appropriate iteration methods to FileManager).\n";
            break;
        }
        case 2: {
            // Simplified: Assuming new users are added to a 'pending_users.txt' file first
            cout << "\n--- New Account Management (Approval is done by Employee) ---\n";
            int newUserId = getIntInput("Enter the User ID of the pending application to APPROVE/REJECT: ");
            string action = getStringInput("Approve (A) or Reject (R)? ");

            if (action == "A" || action == "a") {
                userclient pendingUser = FileManager::loadUser(newUserId);
                if (pendingUser.getLoginID() != newUserId) {
                    cout << "X User ID not found in pending list or already processed.\n";
                    break;
                }

                // 1. Assign Random Card/PIN (Requirement)
                long long cardNum = generateCardNumber();
                int pin = getIntInput("Prompting customer for PIN: Enter new 4-digit PIN: ");

                // 2. Assign appropriate account type based on estimated limit (Bronze/Gold logic)
                // The initial registration should provide this. We use a default/simple check here.
                int estLimit = getIntInput("Enter Approved Daily Withdrawal Limit (100000 for Bronze, 500000 for Gold): ");

                account* newAcc = nullptr;
                if (estLimit <= 100000) {
                    newAcc = new bronze(newUserId, pendingUser.getPassword(), 0);
                }
                else if (estLimit <= 500000) {
                    newAcc = new gold(newUserId, pendingUser.getPassword(), 0);
                }
                else {
                    cout << "WARNING: Limit too high for User Client. Defaulting to Gold.\n";
                    newAcc = new gold(newUserId, pendingUser.getPassword(), 0);
                }

                pendingUser.setAccount(newAcc);

                // 3. Save to file (assuming registration process saves preliminary data, and this updates the account type)
                FileManager::updateUser(pendingUser);

                // 4. Save Card details
                card newCard(cardNum, pin, newAcc);
                FileManager::saveCard(newUserId, newCard);

                cout << "User Client Account ID " << newUserId << " APPROVED.\n";
                cout << "   Card Assigned: " << cardNum << " | PIN: " << pin << endl;

            }
            else if (action == "R" || action == "r") {
                // Implement logic to remove the user from 'pending_users.txt' or mark as rejected
                cout << "User Client Account ID " << newUserId << " REJECTED.\n";
            }
            break;
        }
        case 3: {
            cout << "\nWARNING: This feature requires reading and managing a 'loans.txt' file.\n";
            cout << "   (Implementation depends on adding appropriate loan functions to FileManager).\n";
            break;
        }
        case 4: {
            int accountId = getIntInput("Enter the User or Company ID to view transactions: ");
            displayTransactionHistory(accountId);
            break;
        }
        case 5: {
            int accountId = getIntInput("Enter the User or Company ID to Freeze/Close: ");
            string action = getStringInput("Freeze (F) or Close (C)? ");

            if (action == "F" || action == "f") {
                FileManager::freezeUser(accountId);
                cout << "Account ID " << accountId << " has been FROZEN.\n";
            }
            else if (action == "C" || action == "c") {
                // Closing an account requires removing it from users.txt/companies.txt
                cout << "WARNING: Closing requires physically removing the entry from the client data file.\n";
                cout << "   (Implementation depends on adding a delete/close function to FileManager).\n";
            }
            break;
        }
        case 0:
            cout << "\nLogging out Banking Employee...\n";
            break;
        default:
            cout << "\nWARNING: Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);
}

// ------------------------------------------------------------
// 4. LOGIN SELECTION MENU (for users who are also employees)
// ------------------------------------------------------------
void loginSelectionMenu(int userId) {
    int choice;
    do {
        cout << "\n======================================\n";
        cout << "           ACCOUNT SELECTION            \n";
        cout << "======================================\n";
        cout << "You are a User Client and a Company Employee. Select account to access:\n";
        cout << "1. Access **Personal** User Client Account (ID: " << userId << ")\n";
        cout << "2. Access **Company** Client Account(s)\n";
        cout << "0. Return to Main Menu\n";
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
        case 1:
            userClientMenu(userId);
            choice = 0; // Exit this loop after activity
            break;
        case 2: {
            // This requires a list of companies the user works for (companies_employees.txt)
            // Since your FileManager::loadCompanyEmployees takes a companyID and returns userIDs,
            // you would need an inverse lookup function to list companies by userID.
            cout << "\nWARNING: Listing Company Accounts by User ID not implemented in current FileManager.h.\n";
            cout << "   Please login via Main Menu option 2 for now.\n";
            // For demonstration, let's allow a direct Company ID entry if the user knows it
            int companyId = getIntInput("Enter the Company ID you wish to access: ");
            companyClientMenu(companyId); // Assuming access is valid
            choice = 0;
            break;
        }
        case 0:
            cout << "\nReturning to Main Menu...\n";
            break;
        default:
            cout << "\nWARNING: Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);
}


// ------------------------------------------------------------
// 5. MAIN MENU (Entry Point)
// ------------------------------------------------------------
int main() {
    int choice;
    int loginId, password;

    cout << "Welcome to the ABC Bank Management System.\n";

    do {
        cout << "\n======================================\n";
        cout << "        ABC BANK MANAGEMENT SYSTEM      \n";
        cout << "======================================\n";
        cout << "1. User Client Login\n";
        cout << "2. Company Client Login\n";
        cout << "3. Banking Employee Login\n";
        cout << "4. Open New User Client Account (Pre-Approval)\n";
        cout << "5. Open New Company Client Account\n";
        cout << "0. Exit System\n";
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
        case 1: {
            loginId = getIntInput("Enter Login ID: ");
            password = getIntInput("Enter Password: ");

            userclient u = FileManager::loadUser(loginId);

            if (u.getLoginID() == loginId && u.getPassword() == password) {
                if (FileManager::checkUserFrozen(loginId)) {
                    cout << "X Login failed. Account is frozen.\n";
                    break;
                }

                // Simple check if user is also an employee (Requires an inverse lookup function in FileManager)
                // Since that isn't implemented, we skip the loginSelectionMenu for now and go straight to User Menu.
                userClientMenu(loginId);

            }
            else {
                cout << "\nX Login failed! Invalid ID or Password.\n";
            }
            break;
        }
        case 2: {
            loginId = getIntInput("Enter Account ID (Company ID): ");
            password = getIntInput("Enter Account Password: ");

            companyclient c = FileManager::loadCompany(loginId);

            if (c.getLoginID() == loginId && c.getPassword() == password) {
                companyClientMenu(loginId);
            }
            else {
                cout << "\nX Login failed! Invalid Account ID or Password.\n";
            }
            break;
        }
        case 3: {
            loginId = getIntInput("Enter Employee ID: ");
            password = getIntInput("Enter Password: ");

            if (FileManager::verifyAdmin(loginId, password)) {
                bankingEmployeeMenu(loginId);
            }
            else {
                cout << "\nX Login failed! Invalid Employee ID or Password.\n";
            }
            break;
        }
        case 4: {
            cout << "\n--- New User Client Account Registration ---\n";
            // Get all required information from the user
            string fullName = getStringInput("Enter Full Name: ");
            string address = getStringInput("Enter Address: ");
            int phone = getIntInput("Enter Phone Number: ");
            int cnic = getIntInput("Enter CNIC: ");
            int estLimit = getIntInput("Enter Estimated Daily Withdrawal Limit (PKR): ");

            // Get unique ID and set temporary login details
            int newId = FileManager::getNextUserID();
            int tempPass = 1234; // Temp password until approved

            // Create a temporary UserClient object
            userclient newUser(fullName, address, phone, cnic, newId, tempPass, nullptr);

            // Save it to the file (it's assumed to be 'pending' until approved by employee)
            if (FileManager::saveUser(newUser)) {
                cout << "\nAccount application submitted for review.\n";
                cout << "   Your temporary Login ID is: **" << newId << "**. You will be notified upon approval.\n";
            }
            else {
                cout << "\nX Failed to save application. Check file permissions.\n";
            }
            break;
        }
        case 5: {
            cout << "\n--- New Company Client Account Registration ---\n";
            string compName = getStringInput("Enter Company Name: ");
            string address = getStringInput("Enter Company Address: ");
            int taxNo = getIntInput("Enter Company Tax Number: ");

            int newId = FileManager::getNextCompanyID();
            int accPass = getIntInput("Enter Account Password (e.g., 4 digits): ");

            // Company Client accounts are Business type by default (Limit is set in business class constructor)
            business* newBusinessAcc = new business(newId, accPass, 0);
            companyclient newCompany(compName, address, taxNo, newId, accPass, newBusinessAcc, nullptr, nullptr);

            if (FileManager::saveCompany(newCompany)) {
                cout << "\nCompany Account successfully opened.\n";
                cout << "   Account ID: **" << newId << "**. Password: " << accPass << ".\n";
            }
            else {
                cout << "\nX Failed to open company account. Check file permissions.\n";
            }
            break;
        }
        case 0:
            cout << "\nThank you for using ABC Bank Management System. Goodbye!\n";
            break;
        default:
            cout << "\nWARNING: Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}