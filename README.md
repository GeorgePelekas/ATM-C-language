# ATM-C-language
This project is a console-based ATM (Automated Teller Machine) simulation implemented in C.
It models basic banking operations such as account management, withdrawals, deposits, transfers, and transaction logging using binary files.  
The system is designed with a focus on low-level data handling (file I/O, structs, memory) .
Features
🔐 User Authentication
Login using Account Number (AL) and PIN
Limited login attempts
💰 Account Operations
Balance inquiry
Deposit (with banknote validation)
Withdrawal (with ATM cash constraints)
Money transfer between accounts
📜 Transaction System
Stores all transactions in a binary file
Displays last 4 transactions
Tracks daily withdrawals (limit: 600€)
🧾 Payments
Rent payments
Utility expenses
🔑 Security
PIN change functionality
Admin (hidden) functions for ATM control
🏦 ATM Cash Management
Tracks available banknotes (20€, 10€, 5€)
Prevents withdrawals if exact amount cannot be formed
🗂️ File Structure
accounts → Binary file storing all account data
transactions → Binary file storing transaction history
🧠 Core Concepts Used
Structs (struct log, struct trns)
Binary file handling (fread, fwrite)
Searching (Binary Search)
Pointers and memory handling
Date/time handling (time.h)
State persistence via files

How to Run
Compile the program:
gcc atm.c -o atm
Run:
./atm

⚠️ Make sure the file paths exist:

C:/free time/ergo2/accounts
C:/free time/ergo2/transactions
🔒 Admin Access

Hidden admin options:

101 → View ATM cash
102 → Refill ATM
103 → Shutdown system

Credentials:

Username: PELEKAS
PIN: 5425
⚠️ Limitations
Fixed maximum accounts (1600)
No encryption for PINs
Windows-specific functions (conio.h, system("cls"))
📈 Possible Improvements
Replace binary files with a database
Add encryption for sensitive data
GUI interface
📚 Purpose

This project is intended for educational use, demonstrating how low-level systems like ATMs can be simulated using C and fundamental programming concepts.
