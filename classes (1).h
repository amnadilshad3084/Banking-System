#pragma once
#include<iostream>
#include <string>
using namespace std;

class account {//abstract class
protected:
	int id;
	int password;
	int balance;
	int limit;
public:
	account() {
		id = 0;
		password = 0;
		balance = 0;
		limit = 0;
	}

	account(int i, int p, int b) {
		id = i;
		password = p;
		balance = b;
		limit = 0;
	}
	virtual void deposit() = 0;
	virtual void withdraw() = 0;
	virtual void transfer() = 0;

	void setid(int i) {
		id = i;
	}
	void setpassword(int p) {
		password = p;

	}
	void setbalance(int b) {
		balance = b;
	}
	void setlimit(int w) {
		limit = w;
	}
	int getid() {
		return id;
	}
	int getpassword() {
		return password;
	}
	int getbalance() {
		return balance;
	}
	int getlimit() {
		return limit;
	}
	virtual ~account() {}
};
class bronze :public account {
public:
	bronze(int i, int p, int b) :account(i, p, b) {
		limit = 100000;
	}
	void deposit()override {};
	void withdraw()override {};
	void transfer()override {};
	virtual~bronze() {}
};
class gold :public account {
public:
	gold(int i, int p, int b) :account(i, p, b) {
		limit = 500000;
	}
	void deposit()override {};
	void withdraw()override {};
	void transfer()override {};
	virtual~gold() {}
};
class business :public account {
public:
	business(int i, int p, int b) :account(i, p, b) {
		limit = 20000000;
	}
	void deposit()override {};
	void withdraw()override {};
	void transfer()override {};
	void requestloan() {};
	virtual~business() {}
};
class card {
private:
	int cardNumber;
	int pin;
	bool fraudulent;
	account* owner;         //array of owners 

public:

	card() {
		cardNumber = 0;
		pin = 0;
		fraudulent = false;
		owner = nullptr;
	}

	card(int num, int p, account* acc) {
		cardNumber = num;
		pin = p;
		fraudulent = false;
		owner = acc;        // linking card to account
	}

	void setCardNumber(int num) {
		cardNumber = num;
	}
	void setPin(int p) {
		pin = p;
	}
	void setFraudulent(bool f) {
		fraudulent = f;
	}
	void setOwner(account* acc) {
		owner = acc;
	}


	int getCardNumber() {
		return cardNumber;
	}
	int getPin() {
		return pin;
	}
	bool isFraudulent() {
		return fraudulent;
	}
	account* getOwner() {
		return owner;
	}


	bool verifyPin(int enteredPin) {
		return (enteredPin == pin);
	}
};
class userclient {
private:
	string name;
	string address;
	int phone;
	int cnic;
	int loginId;
	int password;
	account* accounts;
	card* cards;//i am keeping it 10 rn but we can change it to dynamic later if needed
	int cardcount;
public:
	userclient() {
		name = "";
		address = "";
		phone = 0;
		cnic = 0;
		loginId = 0;
		password = 0;
		accounts = nullptr;
		cardcount = 0;
	}
	userclient(string n, string addr, int ph, int  c, int id, int pass, account* acc) {
		name = n;
		address = addr;
		phone = ph;
		cnic = c;
		loginId = id;
		password = pass;
		cards = new card[10];
		accounts = acc;
		cardcount = 0;
	}
	void setName(string n) {
		name = n;
	}
	void setAddress(string a) {
		address = a;
	}
	void setPhone(int p) {
		phone = p;
	}
	void setCnic(int c) {
		cnic = c;
	}
	void setLoginID(int id) {
		loginId = id;
	}
	void setPassword(int p) {
		password = p;
	}
	void setAccount(account* acc) {
		accounts = acc;
	}

	string getName() {
		return name;
	}
	string getAddress() {
		return address;
	}
	int getPhone() {
		return phone;
	}
	int getCnic() {
		return cnic;
	}
	int getLoginID() {
		return loginId;
	}
	int getPassword() {
		return password;
	}
	account* getAccount() {
		return accounts;
	}
	int getCardCount() {
		return cardcount;
	}
	void addCard(const card& c) {
		cards[cardcount] = c;
		cardcount++;
	}
	card* getCards() {
		return cards;
	}
	void deposit() {}
	void withdraw() {}
	void transfer() {}
	void viewHistory() {}
	~userclient() {
		delete[]cards;
	}
};
class companyclient {
private:
	string companyName;
	string address;
	int taxno;
	int loginId;
	int password;
	business* businesses;
	userclient* employees;
	card* cards;
public:
	companyclient() {
		companyName = " ";
		address = " ";
		taxno = 0;
		loginId = 0;
		password = 0;
		businesses = nullptr;
		employees = nullptr;
		cards = nullptr;
	}
	companyclient(string cn, string a, int t, int l, int p, business* b, userclient* e, card* c) {
		companyName = cn;
		address = a;
		taxno = t;
		loginId = l;
		password = p;
		businesses = b;
		employees = e;
		cards = c;
	}
	string getCompanyName() { 
		return companyName; 
	}
	string getAddress() { 
		return address; 
	}
	int getTaxno() { 
		return taxno; 
	}
	int getLoginID() { 
		return loginId; 
	}
	int getPassword() { 
		return password; }

	business* getBusiness() {
		return businesses;
	}

	void deposit() {}
	void withdraw() {}
	void transfer() {}
	void requestloan() {}
	void addemployes(userclient* e) {}
};
class bankingemployee {
private:
	int employeeId;
	int password;
public:
	bankingemployee() {
		employeeId = 0;
		password = 0;
	}
	bankingemployee(int e, int p) {
		employeeId = e;
		password = p;
	}
	void approveAccount() {}
	void rejectAcoount() {}
	void viewallacounts() {}
	void approveloan() {}
	void freezeaccount() {}

};
class transaction {
private:
	int amount;
	string date;
	string time;
	string type;
	int sourceId;
	int destId;//destination user id
public:
	transaction() {
		amount = 0;
		date = " ";
		time = " ";
		type = " ";
		sourceId = 0;
		destId = 0;
	}
	transaction(int a, string d, string t, string ty, int si, int di) {
		amount = a;
		date = d;
		time = t;
		type = ty;
		sourceId = si;
		destId = di;
	}
	int getAmount() { 
		return amount; 
	}
	string getDate() { 
		return date; 
	}
	string getType() { 
		return type; 
	}
	int getSourceId() { 
		return sourceId; 
	}
	int getDestId() { 
		return destId; 
	}
	string getTime() {
		return time;
	}
	string stringify() {
	};
};

