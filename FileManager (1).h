#pragma once
#include <iostream>
#include "classes.h"
#include <fstream>
#include <string>
using namespace std;

class FileManager {
    //static used so we can call functions without creating a file manager object like filemanager::loaduser(...)
public:
    // --------- USERS ----
    static userclient loadUser(int userID);
    static bool saveUser(userclient& u);
    static bool updateUser(userclient& u);
    static int getNextUserID();

    // ----------- COMPANIES 
    static companyclient loadCompany(int companyID);
    static bool saveCompany(companyclient& c);
    static bool updateCompany(companyclient& c);
    static int getNextCompanyID();

    // ---- COMPANY EMPLOYEE  
    static void loadCompanyEmployees(int companyID, int outArray[], int& outCount, int maxCount);
    static bool saveCompanyEmployee(int userID, int companyID);

    // ------ ADMIN ------
    static bool verifyAdmin(int adminID, int adminPassword);

    // ------- CARDS ----
    static void loadCards(int userID, card outCards[], int& outCount, int maxCount);
    static bool saveCard(int userID, card& c);
    static void markCardFraud(long long cardNumber);
    static bool checkCardFraud(long long cardNumber);

    // ---------- TRANSACTIONS -------
    static void loadTransactionsByUser(int userID, transaction outTrans[], int& outCount, int maxCount);
    static bool appendTransaction(transaction& t);

    // ------- BALANCE,FREEZE ----
    static bool updateBalance(int userID, int newBalance);
    static void freezeUser(int userID);
    static bool checkUserFrozen(int userID);

    //helper function
    static int countLinesInFile(string& filename);
};
