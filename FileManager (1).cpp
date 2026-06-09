#include "FileManager.h"   
#include "classes.h" 
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

//since the data in files has commas so we need to separate that first
void separatecommas(string line, string parts[], int& count) { //parts[] is array to split the words
    count = 0;         
    string current; 

    //go through each character in the line
    for (int i = 0; i < line.length(); i++) { //no. of charactere

        if (line[i] == ',') { //finding a comma
            parts[count] = current;    //save word
            count++;                 //move next
            current = "";            //reset for next word
        }
        else {
            current = current + line[i];
        }
    }

    parts[count] = current;
    count++;
}

//data in our files is saved as text so we need to convert that to numbers for calcultaion
int TXTtoInt(string s) {
    int result = 0;

    for (int i = 0; i < s.length(); i++) {

        //if digit,0-9
        if (s[i] >= '0' && s[i] <= '9') {
            result = result * 10 + (s[i] - '0'); //0 ascii=48, subtracting makes it real number
            //result*10 shifts num left,then we add the new digit
        }
    }
    return result;
}

double TXTtoDec(string s) {
    double result = 0;
    bool afterDecimal = false;   
    double decimalPlace = 0.1;    //position after dot

    for (int i = 0; i < s.length(); i++) {

        if (s[i] == '.') {
            afterDecimal = true; 
        }
        else if (!afterDecimal) {
            result = result * 10 + (s[i] - '0');      //before dot
        }
        else {       
            result = result + (s[i] - '0') * decimalPlace;   //after dot = add decimal places
            decimalPlace = decimalPlace / 10;
        }
    }
    return result;
}



// -----------------   users.txt
userclient FileManager::loadUser(int userID) {


    ifstream file("users.txt"); //open file

    userclient emptyUser;

    if (!file.is_open()) {
        return emptyUser;  //file doesn't exist
    }

    string line;  //will store each line we read

    //read file line by line
    while (getline(file, line)) {

        //split the line into parts
        string parts[10];  //array to hold pieces
        int partCount = 0;
        separatecommas(line, parts, partCount);

        //so now we have:
        // parts[0] = userid
        // parts[1] = username
        // parts[2] = address
        // parts[3] = cnic
        // parts[4] = loginid
        // parts[5] = password
        // parts[6] = balance


        int id = TXTtoInt(parts[0]); //first part to number


        if (id == userID) {   //is it the user we want

            userclient user;


            user.setName(parts[1]);
            user.setAddress(parts[2]);
            user.setCnic(TXTtoInt(parts[3]));
            user.setLoginID(TXTtoInt(parts[4]));
            user.setPassword(TXTtoInt(parts[5]));

            double balance = TXTtoDec(parts[6]);

            account* acc = new bronze(id, TXTtoInt(parts[5]), balance);// create an account for user
            user.setAccount(acc);

            file.close();
            return user;
        }
    }

    file.close();
    return emptyUser; //user wasnt found
}

//---------------- save new user
bool FileManager::saveUser(userclient& u) {

    ofstream file("users.txt", ios::app);

    if (!file.is_open()) {
        return false;  
    }


    //writing userid,username,address,cnic,loginid,password,balance

    file << u.getLoginID() << ",";    
    file << u.getName() << ",";        
    file << u.getAddress() << ",";     
    file << u.getCnic() << ",";      
    file << u.getLoginID() << ",";     
    file << u.getPassword() << ",";  

    // Step 4: Write balance (from account)
    if (u.getAccount() != nullptr) { //if user has account then write otherwise 0
        file << u.getAccount()->getbalance();
    }
    else {
        file << 0;
    }
    file << endl; 
    file.close();
    return true; 
}

//----------------- for deposits/withdraws, balance changes

bool FileManager::updateUser(userclient& u) {

    ifstream fileIn("users.txt");

    ofstream fileOut("temp.txt");//temp file


    if (!fileIn.is_open() || !fileOut.is_open()) {
        return false;
    }

    string line;
    bool found = false;
    int targetID = u.getLoginID();  //id of user to update

    while (getline(fileIn, line)) {

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        int id = TXTtoInt(parts[0]);

        if (id == targetID) {//checking if its the user to update

            fileOut << u.getLoginID() << ","
                << u.getName() << ","
                << u.getAddress() << ","
                << u.getCnic() << ","
                << u.getLoginID() << ","
                << u.getPassword() << ",";

            if (u.getAccount() != nullptr) {
                fileOut << u.getAccount()->getbalance();
            }
            else {
                fileOut << 0;
            }

            fileOut << endl;
            found = true;

        }
        else {
            fileOut << line << endl;
        }
    }

    fileIn.close();
    fileOut.close();

 
    remove("users.txt");        //delete old file
    rename("temp.txt", "users.txt"); //rename temp to original name

    return found;
}


// new users need unique IDs

int FileManager::getNextUserID() {


    ifstream file("users.txt");

    if (!file.is_open()) {
        return 1;  // first user gets id 1
    }

    int maxID = 0 ;
    string line;


    while (getline(file, line)) {

   
        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        int id = TXTtoInt(parts[0]);


        if (id > maxID) {
            maxID = id; 
        }
    }

    file.close();

    // return next ID (max+1)
    return maxID + 1;
}

//-------------------- COMPANY FUNCTIONS -------------

//-----  companies.txt
companyclient FileManager::loadCompany(int companyID) {

    ifstream file("companies.txt");
    companyclient emptyCompany;

    if (!file.is_open()) {
        return emptyCompany;
    }

    string line;

    while (getline(file, line)) {

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        // parts[0] = companyid
        // parts[1] = companyname
        // parts[2] = address
        // parts[3] = tax number
        // parts[4] = loginid
        // parts[5] = password
        // parts[6] = balance

        int id = TXTtoInt(parts[0]);

        if (id == companyID) {

            double balance = TXTtoDec(parts[6]);

            //create business account
            business* bAcc = new business(id, TXTtoInt(parts[5]), balance);

            //create company using constructor
            companyclient company(
                parts[1],             // company name
                parts[2],               // address
                TXTtoInt(parts[3]),       // tax number
                TXTtoInt(parts[4]),       // login id
                TXTtoInt(parts[5]),       // password
                bAcc,                        // business account
                nullptr,                     // employees
                nullptr                      // cards 
            );

            file.close();
            return company;
        }
    }

    file.close();
    return emptyCompany;
}

bool FileManager::saveCompany(companyclient& c) {

    ofstream file("companies.txt", ios::app);

    if (!file.is_open()) {
        return false;
    }

    // writing company data
    file << c.getLoginID() << ","
        << c.getCompanyName() << ","
        << c.getAddress() << ","
        << c.getTaxno() << ","
        << c.getLoginID() << ","
        << c.getPassword() << ",";

    if (c.getBusiness() != nullptr) {
        file << c.getBusiness()->getbalance();
    }
    else {
        file << 0;
    }

    file << endl;
    file.close();
    return true;
}


bool FileManager::updateCompany(companyclient& c) {

    ifstream fileIn("companies.txt");
    ofstream fileOut("temp.txt");

    if (!fileIn.is_open() || !fileOut.is_open()) {
        return false;
    }

    string line;
    bool found = false;
    int targetID = c.getLoginID();

    while (getline(fileIn, line)) {

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        int id = TXTtoInt(parts[0]);

        if (id == targetID) {
            // Write updated company
            fileOut << c.getLoginID() << ","
                << c.getCompanyName() << ","
                << c.getAddress() << ","
                << c.getTaxno() << ","
                << c.getLoginID() << ","
                << c.getPassword() << ",";

            if (c.getBusiness() != nullptr) {
                fileOut << c.getBusiness()->getbalance();
            }
            else {
                fileOut << 0;
            }

            fileOut << endl;
            found = true;
        }
        else {
            fileOut << line << endl;
        }
    }

    fileIn.close();
    fileOut.close();

    remove("companies.txt");
    rename("temp.txt", "companies.txt");

    return found;
}


int FileManager::getNextCompanyID() {

    ifstream file("companies.txt");

    if (!file.is_open()) {
        return 1;
    }

    int maxID = 0;
    string line;

    while (getline(file, line)) {
        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        int id = TXTtoInt(parts[0]);

        if (id > maxID) {
            maxID = id;
        }
    }

    file.close();
    return maxID + 1;
}


//--------------- EMPLOYEE COMPANY -------------

// companies_employees.txt
void FileManager::loadCompanyEmployees(int companyID, int outArray[], int& outCount, int maxCount) {

    ifstream file("companies_employees.txt");

    if (!file.is_open()) {
        outCount = 0;  //no employees found
        return;
    }

    outCount = 0;
    string line;

    while (getline(file, line) && outCount < maxCount) {// stop when maximum allowed employees

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        // parts[0] = userid
        // parts[1] = companyid

        int uid = TXTtoInt(parts[0]); //user id
        int cid = TXTtoInt(parts[1]);  ///comment id

   
        if (cid == companyID) {      //does this employee work for company
            outArray[outCount] = uid;  //add to array
            outCount++;            //count it
        }
    }

    file.close();
}

//add employee to company
bool FileManager::saveCompanyEmployee(int userID, int companyID) {

    ofstream file("companies_employees.txt", ios::app);

    if (!file.is_open()) {
        return false;
    }
    //userid,companyid
    file << userID << "," << companyID << endl;

    file.close();
    return true;
}


// -------------------------- ADMIN-------------

//   admin.txt
//check if the admin trying to log in is valid
bool FileManager::verifyAdmin(int adminID, int adminPassword) {

    ifstream file("admin.txt");

    if (!file.is_open()) {
        return false;
    }

    string line;

    while (getline(file, line)) {

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        int id = TXTtoInt(parts[0]);
        int pass = TXTtoInt(parts[1]);

  
        if (id == adminID && pass == adminPassword) {
            file.close();
            return true; 
        }
    }

    file.close();
    return false; 
}


// ----------------  CARD FUNCTIONS --------------

// cards.txt

void FileManager::loadCards(int userID, card outCards[], int& outCount, int maxCount) {

    ifstream file("cards.txt");

    if (!file.is_open()) {
        outCount = 0;
        return;
    }

    outCount = 0;
    string line;

    while (getline(file, line) && outCount < maxCount) {

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

        // parts[0] = userid
        // parts[1] = cardnumber
        // parts[2] = pin

        int uid = TXTtoInt(parts[0]);

        //if card belong to user
        if (uid == userID) {

      
            card c;
            c.setCardNumber(TXTtoInt(parts[1]));  //card no. to integer
            c.setPin(TXTtoInt(parts[2]));  //card pin to integer

            outCards[outCount] = c;
            outCount++;
           
        }
    }

    file.close();
}

//save a card to cards.txt
bool FileManager::saveCard(int userID, card& c) {

    ofstream file("cards.txt", ios::app);

    if (!file.is_open()) {
        return false;
    }

    file << userID << ","
        << c.getCardNumber() << ","
        << c.getPin() << endl;

    file.close();
    return true;
}


// fraudulent
void FileManager::markCardFraud(long long cardNumber) {

    ofstream file("fraud_cards.txt", ios::app);

    if (!file.is_open()) {
        return;
    }

    file << cardNumber << endl;
    file.close();
}


bool FileManager::checkCardFraud(long long cardNumber) {

    ifstream file("fraud_cards.txt");

    if (!file.is_open()) {
        return false;
    }

    string line;

    while (getline(file, line)) {

        //convert line to number
        long long num = 0;
        for (int i = 0; i < line.length(); i++) {
            if (line[i] >= '0' && line[i] <= '9') {
                num = num * 10 + (line[i] - '0');
            }
        }

     
        if (num == cardNumber) {
            file.close();
            return true;  // its fraudulent
        }
    }

    file.close();
    return false;  //not fraudulent
}


// --------------- TRANSACTION FUNCTIONS-----------------
// transactions.txt

void FileManager::loadTransactionsByUser(int userID, transaction outTrans[], int& outCount, int maxCount) {

    //outTrans[] = array of transaction objects, outCount= num of transactions

    ifstream file("transactions.txt");

    if (!file.is_open()) {
        outCount = 0;
        return;
    }

    outCount = 0;
    string line;

    while (getline(file, line) && outCount < maxCount) {

        string parts[10];
        int partCount = 0;
        separatecommas(line, parts, partCount);

     // parts[0] = date
     // parts[1] = userid
      // parts[2] = amount
        // parts[3] = type (Deposit/Withdraw/Transfer)
      // parts[4] = destid (only if transfer)

        int uid = TXTtoInt(parts[1]);

    
        if (uid == userID) {

            int amount = TXTtoInt(parts[2]);
            int destId = 0;

            //if transfer, get destination id
            if (partCount > 4) {
                destId = TXTtoInt(parts[4]);
            }

   
            transaction t(
                amount,      // amount
                parts[0],    // date
                "",          //time (not in file)
                parts[3],    //type
                uid,         //source id
                destId       //destination id
            );

            outTrans[outCount] = t;
            outCount++;
        }
    }

    file.close();
}

//new transaction to transactions.txt , doesnt change old data
bool FileManager::appendTransaction(transaction& t) { 

    ofstream file("transactions.txt", ios::app);

    if (!file.is_open()) {
        return false;
    }


    file << t.getDate() << ","
        << t.getSourceId() << ","
        << t.getAmount() << ","
        << t.getType();

    //if transfer, add destination id
    if (t.getType() == "Transfer" && t.getDestId() != 0) {
        file << "," << t.getDestId();
    }

    file << endl;
    file.close();
    return true;
}


// ========== BALANCE & FREEZE ==========

//update user's balance
bool FileManager::updateBalance(int userID, int newBalance) {


    userclient user = loadUser(userID);

    // update balance
    if (user.getAccount() != nullptr) {
        user.getAccount()->setbalance(newBalance);

        return updateUser(user);
    }

    return false;
}


//----------------   freeze or close account
void FileManager::freezeUser(int userID) {

    ofstream file("frozen_accounts.txt", ios::app);

    if (!file.is_open()) {
        return;
    }
    //adding id to frozen account
    file << userID << endl;
    file.close();
}

//checks if user is already frozen
bool FileManager::checkUserFrozen(int userID) {

    ifstream file("frozen_accounts.txt");

    if (!file.is_open()) {
        return false;
    }

    string line;

    while (getline(file, line)) {

        int id = TXTtoInt(line);

        if (id == userID) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}


// ---------  helper function
// to find how many entries are in a file
// Count how many lines in a file like if we want to know how many users exist in users.txt
int FileManager::countLinesInFile(string& filename) {

    ifstream file(filename);

    if (!file.is_open()) {
        return 0;
    }

    int count = 0;
    string line;

    while (getline(file, line)) {
        count++;
    }

    file.close();
    return count;
}