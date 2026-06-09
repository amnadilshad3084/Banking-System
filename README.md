# 🏦 Banking Management System

A console-based Banking Management System built in C++ as a university project, 
simulating real bank operations for three types of users:
User Clients, Company Clients, and Banking Employees.

## 🛠️ Built With
- **Language:** C++
- **IDE:** Visual Studio 2022

## ✨ Features

### 👤 User Client
- Open a personal bank account (Bronze, Gold based on withdrawal limit)
- Deposit, withdraw, and transfer funds to other accounts
- View account balance and transaction history
- Card-based PIN verification for every transaction
- Account flagged as fraudulent after 3 incorrect PIN attempts

### 🏢 Company Client
- CEO opens a shared account accessible by all company employees
- Supports deposit, withdrawal, fund transfers to user accounts
- Loan requests submitted to banking employee for approval

### 🧑‍💼 Banking Employee
- Approve or reject new account applications
- Approve or reject company loan requests
- View all client accounts and transaction histories

## Project Structure
```text
banking-system/
├── classes(1).h              # Class declarations (OOP structure)
├── FileManager(1).h          # File manager class declaration
├── FileManager(1).cpp        # File read/write operations
├── main2(1).cpp              # Main program entry point
├── users.txt                 # Registered user client data
├── companies.txt             # Company client data
├── companies_employees.txt   # Company-employee mappings
├── cards.txt                 # Credit card data
├── transactions.txt          # Transaction history
├── admin.txt                 # Banking employee credentials
├── frozen_accounts.txt       # Frozen account records
└── fraud_cards.txt           # Flagged fraudulent cards
```
## 🚀 How to Run
1. Open the project folder in Visual Studio 2022
2. Build the solution (`Ctrl + Shift + B`)
3. Run (`Ctrl + F5`)
4. Log in as a User Client, Company Client, or Banking Employee

## 📚 Concepts Used
- Object-Oriented Programming — inheritance, polymorphism, 
  encapsulation, abstraction, aggregation, composition
- File handling for persistent data storage across sessions
- Input validation and fraud detection logic
- Multi-user account management

- Freeze, close, or flag accounts as fraudulent

## 📁 Project Structure
