#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

/* ── Admin credentials ── */
#define ADMIN_NAME "PELEKAS"
#define ADMIN_PIN  5425

/* ── ATM cash reserves (number of banknotes) ── */
int twenties = 400;
int tens     = 800;
int fives    = 800;

/* ── Account record ── */
struct account
{
    unsigned short id;
    unsigned short pin;
    char lastname[28];
    char firstname[28];
    float balance;
} accounts[1600];

/* ── Transaction record ── */
struct transaction
{
    float          amount;
    char           code[3];   /* TRN, WDR, DEP, RNT, EXP */
    unsigned short account_id;
    unsigned char  day, month, year;
} transactions;

/* ── Load all accounts from binary file ── */
void loadAccounts()
{
    FILE *fp = fopen("accounts", "rb");
    if (!fp)
    {
        perror("accounts");
        printf("No data found in the binary file. The program will terminate.");
        exit(0);
    }
    fread(accounts, sizeof(struct account), 1600, fp);
    fclose(fp);
}

/* ── Save all accounts to binary file ── */
void saveAccounts(void)
{
    FILE *fp = fopen("accounts", "wb");
    if (!fp) { perror("accounts"); return; }
    fwrite(accounts, sizeof(struct account), 1600, fp);
    fclose(fp);
}

/* ── Get today's date ── */
void getToday(unsigned char *d, unsigned char *m, unsigned char *y)
{
    time_t t = time(NULL);
    struct tm *tmv = localtime(&t);
    *d = (unsigned char)tmv->tm_mday;
    *m = (unsigned char)(tmv->tm_mon + 1);
    *y = (unsigned char)(tmv->tm_year % 100);
}

/* ── Append a transaction to the transactions file ── */
void appendTransaction(const char code[3], float amount, unsigned short id)
{
    struct transaction t;
    t.amount     = amount;
    t.code[0]    = code[0];
    t.code[1]    = code[1];
    t.code[2]    = code[2];
    t.account_id = id;
    getToday(&t.day, &t.month, &t.year);
    FILE *fp = fopen("transactions", "ab");
    if (!fp) return;
    fwrite(&t, sizeof(t), 1, fp);
    fclose(fp);
}

/* ── Binary search on accounts array by account ID ── */
int binarySearch(struct account accounts[], int n, int key)
{
    int low = 0, high = n - 1;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        if (accounts[mid].id == key) return mid;
        if (accounts[mid].id < key)  low  = mid + 1;
        else                         high = mid - 1;
    }
    return -1;
}

/* ── Login: ask for account ID + PIN (3 attempts) ── */
int login()
{
    int tries = 3;
    unsigned short inputID, inputPIN;
    printf("                ENTER YOUR ACCOUNT NUMBER : ");
    scanf("%hu", &inputID);
    int pos = binarySearch(accounts, 1600, inputID);
    if (pos != -1)
    {
        while (tries > 0)
        {
            putchar('\n');
            printf("                        ENTER YOUR PIN ----\b\b\b\b");
            scanf("%hu", &inputPIN);
            putchar('\n');
            if (inputPIN == accounts[pos].pin) return pos;
            tries--;
            if (tries > 0)
                printf("                        YOU HAVE %d ATTEMPTS LEFT\n", tries);
        }
        return -1;
    }
    else printf("                ACCOUNT %d DOES NOT EXIST !!!", inputID);
    return -1;
}

/* ── Login without PIN (for deposits / payments) ── */
int loginNoPIN()
{
    unsigned short inputID;
    printf("                ENTER YOUR ACCOUNT NUMBER : ");
    scanf("%hu", &inputID);
    int pos = binarySearch(accounts, 1600, inputID);
    if (pos != -1) return pos;
    printf("                ACCOUNT %d DOES NOT EXIST !!!", inputID);
    return -1;
}

/* ── Lookup account by ID, print error if not found ── */
int findAccount(int id)
{
    int idx = binarySearch(accounts, 1600, id);
    if (idx != -1) return idx;
    printf(" ACCOUNT %d DOES NOT EXIST !!!\n", id);
    return -1;
}

/* ── Print the last 4 transactions for an account ── */
void printLastTransactions(unsigned short id)
{
    FILE *fp = fopen("transactions", "rb");
    if (!fp) { perror("transactions"); return; }
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return; }
    long sz = ftell(fp);
    if (sz < 0) { fclose(fp); return; }
    long recSz = (long)sizeof(struct transaction);
    if (recSz <= 0 || sz < recSz)
    {
        printf(" NO TRANSACTIONS FOUND.\n");
        fclose(fp);
        return;
    }
    long nRecs = sz / recSz;
    struct transaction last[4];
    int found = 0;
    for (long i = nRecs - 1; i >= 0 && found < 4; --i)
    {
        if (fseek(fp, i * recSz, SEEK_SET) != 0) break;
        struct transaction t;
        if (fread(&t, sizeof(t), 1, fp) != 1) break;
        if (t.account_id == id) last[found++] = t;
    }
    fclose(fp);
    if (found == 0) { printf(" NO TRANSACTIONS FOUND.\n"); return; }
    for (int i = 0; i < found; ++i)
    {
        struct transaction *t = &last[i];
        printf("                        %u/%u/%u %c%c%c %+.1f\n",
               (unsigned)t->day, (unsigned)t->month, (unsigned)t->year,
               t->code[0], t->code[1], t->code[2], t->amount);
    }
}

/* ── Calculate total withdrawals made today for an account ── */
float todayWithdrawals(unsigned short id)
{
    unsigned char d, m, y;
    getToday(&d, &m, &y);
    FILE *fp = fopen("transactions", "rb");
    if (!fp) return 0.0f;
    float sum = 0.0f;
    struct transaction t;
    while (fread(&t, sizeof(t), 1, fp) == 1)
    {
        if (t.account_id != id) continue;
        if (t.day != d || t.month != m || t.year != y) continue;
        if (t.code[0] == 'W' && t.code[1] == 'D' && t.code[2] == 'R')
            sum += (t.amount < 0) ? -t.amount : t.amount;
    }
    fclose(fp);
    return sum;
}

/* ── Main menu: returns user choice (1-6, or 101/102/103 for admin) ── */
int mainMenu()
{
    printf("\n");
    int choice;
    puts("                        EXPERIMENTAL ATM - COMPUTER SCIENCE DEPARTMENT");
    puts("            -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");
    puts("                1. INFORMATION                                  4. DEPOSIT\n");
    puts("                2. TRANSFER                                     5. PAYMENTS\n");
    puts("                3. WITHDRAWAL                                   6. CHANGE PIN\n");
    printf("                     PLEASE SELECT AN OPTION (1-6) _\b");
    scanf("%i", &choice);
    while (choice != 1 && choice != 2 && choice != 3 &&
           choice != 4 && choice != 5 && choice != 6 &&
           choice != 101 && choice != 102 && choice != 103)
    {
        system("cls");
        puts("                        EXPERIMENTAL ATM - COMPUTER SCIENCE DEPARTMENT");
        puts("            -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");
        puts("                1. INFORMATION                                  4. DEPOSIT\n");
        puts("                2. TRANSFER                                     5. PAYMENTS\n");
        puts("                3. WITHDRAWAL                                   6. CHANGE PIN\n");
        printf("                     PLEASE SELECT AN OPTION (1-6) _\b");
        scanf("%i", &choice);
    }
    putchar('\n');
    return choice;
}

/* ── Option 1: Account information ── */
void optionInformation()
{
    int input;
    puts("                                WHAT INFORMATION DO YOU NEED?");
    puts("                -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    puts("        1. BALANCE          2. LAST TRANSACTIONS          3. ACCOUNT DETAILS");
    putchar('\n'); putchar('\n'); putchar('\n');
    printf("                PLEASE SELECT WHAT INFORMATION YOU NEED (1-3) _\b");
    scanf("%d", &input);
    if (input == 1)
    {
        int pos = login();
        if (pos != -1)
            printf("                        ACCOUNT BALANCE : %.1f\n", accounts[pos].balance);
    }
    if (input == 2)
    {
        int pos = login();
        if (pos != -1)
            printLastTransactions(accounts[pos].id);
    }
    if (input == 3)
    {
        int pos = login();
        if (pos != -1)
        {
            putchar('\n');
            printf("                        ACCOUNT DETAILS\n\n");
            puts("                        *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");
            printf("                        LAST NAME  : %s\n", accounts[pos].lastname);
            printf("                        FIRST NAME : %s\n", accounts[pos].firstname);
            printf("                        BALANCE    : %.1f\n", accounts[pos].balance);
        }
    }
}

/* ── Option 2: Transfer funds ── */
void optionTransfer()
{
    int destID;
    int pos = login();
    if (pos != -1)
    {
        printf("                ENTER THE RECIPIENT ACCOUNT NUMBER : ");
        scanf("%d", &destID);
        int posDest = findAccount(destID);
        if (posDest != -1)
        {
            printf("                ENTER THE AMOUNT TO TRANSFER : ");
            float amount;
            scanf("%f", &amount);
            if (amount <= accounts[pos].balance && amount >= 0)
            {
                accounts[pos].balance     -= amount;
                accounts[posDest].balance += amount;
                appendTransaction("TRN", -amount, accounts[pos].id);
                appendTransaction("TRN", +amount, accounts[posDest].id);
                saveAccounts();
            }
            else printf("                INSUFFICIENT ACCOUNT BALANCE !!!\n");
        }
    }
}

/* ── Option 3: Withdrawal ── */
void optionWithdrawal()
{
    int amount;
    int snapTwenties = twenties, snapTens = tens, snapFives = fives;
    int pos = login();
    if (pos != -1)
    {
        printf("                        ACCOUNT BALANCE : %.1f\n\n", accounts[pos].balance);
        printf("                ENTER THE AMOUNT YOU WISH TO WITHDRAW : ");
        scanf("%d", &amount);
        int requested = amount;
        if (amount % 5 != 0 || requested <= 0)
        {
            printf("                        WITHDRAWAL AMOUNT MUST BE A MULTIPLE OF 5 EUROS !!!\n");
            return;
        }
        if (requested > accounts[pos].balance)
        {
            printf("                        INSUFFICIENT ACCOUNT BALANCE !!!\n");
            return;
        }
        float usedToday = todayWithdrawals(accounts[pos].id);
        if (usedToday + (float)requested > 600.0f)
        {
            printf("        YOU HAVE EXCEEDED YOUR DAILY WITHDRAWAL LIMIT OF 600 EUROS !!!\n");
            return;
        }
        /* Dispense banknotes greedily: 20s -> 10s -> 5s */
        int fail = 0;
        if (amount >= 20)
        {
            if (amount / 20 > twenties) { amount -= 20 * twenties; twenties = 0; }
            else { twenties -= amount / 20; amount %= 20; }
        }
        if (amount >= 10)
        {
            if (amount / 10 > tens) { amount -= 10 * tens; tens = 0; }
            else { tens -= amount / 10; amount %= 10; }
        }
        if (amount >= 5)
        {
            if (amount / 5 > fives) { amount -= 5 * fives; fives = 0; }
            else { fives -= amount / 5; amount %= 5; }
        }
        if (amount != 0) fail = 1;
        if (fail)
        {
            twenties = snapTwenties; tens = snapTens; fives = snapFives;
            printf("                        THE ATM CANNOT FORM THE REQUESTED AMOUNT WITH AVAILABLE BANKNOTES !!!\n");
            return;
        }
        accounts[pos].balance -= requested;
        appendTransaction("WDR", -(float)requested, accounts[pos].id);
        printf("                        NEW ACCOUNT BALANCE : %.1f\n", accounts[pos].balance);
        saveAccounts();
    }
}

/* ── Option 4: Deposit ── */
void optionDeposit()
{
    int pos = loginNoPIN();
    if (pos != -1)
    {
        int t20 = 0, t10 = 0, t5 = 0;
        int requested;
        printf("                        ACCOUNT BALANCE : %.1f\n\n", accounts[pos].balance);
        printf("        ENTER THE AMOUNT TO DEPOSIT (MUST BE A MULTIPLE OF 5 EUROS): ");
        scanf("%d", &requested);
        int finalAmount = requested;
        if (requested <= 0 || requested % 5 != 0)
        {
            printf("                        DEPOSIT AMOUNT MUST BE A MULTIPLE OF 5 EUROS !!!\n");
            return;
        }
        while (requested != 0)
        {
            if (requested >= 20)
            {
                int max20 = requested / 20;
                printf("\n                        HOW MANY 20-EURO NOTES WILL YOU DEPOSIT : ");
                scanf("%d", &t20);
                if (t20 > max20) { printf("                INVALID INPUT, THE AMOUNT CANNOT BE FORMED !!!\n"); continue; }
                requested -= 20 * t20;
                if (requested == 0)
                {
                    accounts[pos].balance += finalAmount;
                    printf("                NEW ACCOUNT BALANCE : %.1f\n", accounts[pos].balance);
                    appendTransaction("DEP", +(float)finalAmount, accounts[pos].id);
                    saveAccounts();
                    twenties += t20;
                    return;
                }
            }
            if (requested >= 10)
            {
                int max10 = requested / 10;
                printf("\n                        HOW MANY 10-EURO NOTES WILL YOU DEPOSIT : ");
                scanf("%d", &t10);
                if (t10 > max10) { printf("                INVALID INPUT, THE AMOUNT CANNOT BE FORMED !!!\n"); continue; }
                requested -= 10 * t10;
                if (requested == 0)
                {
                    accounts[pos].balance += finalAmount;
                    printf("                NEW ACCOUNT BALANCE : %.1f", accounts[pos].balance);
                    appendTransaction("DEP", +(float)finalAmount, accounts[pos].id);
                    twenties += t20; tens += t10;
                    saveAccounts();
                    return;
                }
            }
            if (requested >= 5)
            {
                int max5 = requested / 5;
                printf("\n                        HOW MANY 5-EURO NOTES WILL YOU DEPOSIT : ");
                scanf("%d", &t5);
                if (t5 > max5) { printf("                INVALID INPUT, THE AMOUNT CANNOT BE FORMED !!!\n"); continue; }
                requested -= 5 * t5;
                if (requested == 0)
                {
                    accounts[pos].balance += finalAmount;
                    appendTransaction("DEP", +(float)finalAmount, accounts[pos].id);
                    saveAccounts();
                    printf("                NEW ACCOUNT BALANCE : %.1f\n", accounts[pos].balance);
                    twenties += t20; tens += t10; fives += t5;
                    return;
                }
            }
        }
    }
}

/* ── Option 5: Payments ── */
void optionPayments()
{
    int input;
    printf("\n                                WHAT PAYMENT DO YOU WISH TO MAKE?\n");
    printf("                -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    printf("        1. RENT              2. UTILITY BILLS (e.g. ELECTRICITY, WATER, PHONE)\n\n\n");
    printf("                PLEASE SELECT WHAT YOU WISH TO PAY (1-2) _\b");
    scanf("%d", &input);
    if (input == 1)
    {
        int pos = loginNoPIN();
        if (pos != -1)
        {
            float amount;
            printf("                ENTER THE PAYMENT AMOUNT : ");
            scanf("%f", &amount);
            if (amount <= accounts[pos].balance && amount >= 0)
            {
                accounts[pos].balance -= amount;
                appendTransaction("RNT", -amount, accounts[pos].id);
                saveAccounts();
            }
            else printf("                INVALID AMOUNT\n");
            return;
        }
    }
    if (input == 2)
    {
        int pos = loginNoPIN();
        if (pos != -1)
        {
            float amount;
            printf("                ENTER THE PAYMENT AMOUNT : ");
            scanf("%f", &amount);
            if (amount <= accounts[pos].balance && amount >= 0)
            {
                appendTransaction("EXP", -amount, accounts[pos].id);
                accounts[pos].balance -= amount;
                saveAccounts();
            }
            else printf("                INVALID AMOUNT\n");
            return;
        }
    }
}

/* ── Option 6: Change PIN ── */
void optionChangePIN()
{
    unsigned short newPIN, confirmPIN;
    int pos = login();
    if (pos != -1)
    {
        printf("                        ENTER NEW PIN : ----\b\b\b\b");
        scanf("%hu", &newPIN);
        printf("\n                        CONFIRM NEW PIN : ----\b\b\b\b");
        scanf("%hu", &confirmPIN);
        if (newPIN == confirmPIN)
        {
            accounts[pos].pin = newPIN;
            saveAccounts();
        }
    }
}

/* ── Admin authentication ── */
int adminLogin()
{
    int inputPIN, tries = 3, success = 0;
    char inputName[21];
    do
    {
        printf("                        ENTER YOUR LAST NAME (UPPERCASE LATIN) --------------------\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        scanf("%20s", inputName);
    } while (strcmp(inputName, ADMIN_NAME) != 0);
    do
    {
        printf("\n                        ENTER YOUR PIN ----\b\b\b\b");
        scanf("%d", &inputPIN);
        if (inputPIN == ADMIN_PIN) { success = 1; return success; }
        else { tries--; printf("\n                        YOU HAVE %d ATTEMPTS LEFT\n", tries); }
    } while (tries > 0);
    return success;
}

/* ── Admin option 101: View banknote counts ── */
void adminViewCash()
{
    if (adminLogin())
        printf("                        20s: %i   10s: %i   5s: %i\n", twenties, tens, fives);
}

/* ── Admin option 102: Reload ATM with banknotes ── */
void adminReloadCash()
{
    if (adminLogin())
    {
        printf("                        THIS OPERATION RELOADS THE ATM WITH BANKNOTES\n");
        int total = 20 * twenties + 10 * tens + 5 * fives;
        if (total >= 8000)  { printf("CURRENT TOTAL IN ATM: %d\n", total); return; }
        if (total + 7000 > 30000) { printf("CURRENT TOTAL IN ATM: %d\n", total); return; }
        twenties += 100;
        tens     += 250;
        fives    += 500;
    }
}

/* ── Admin option 103: Shut down the system ── */
void adminShutdown()
{
    if (adminLogin())
    {
        printf("                        THIS OPTION WILL PERMANENTLY SHUT DOWN THE SYSTEM");
        exit(0);
    }
}

/* ── Press any key and clear screen ── */
void clearScreen()
{
    printf("press any key to continue. . . .");
    _getch();
    system("cls");
}

/* ── Entry point ── */
int main()
{
    while (1)
    {
        loadAccounts();
        int choice = mainMenu();
        printf("\n");
        if      (choice == 1)   { optionInformation(); clearScreen(); }
        else if (choice == 2)   { optionTransfer();    clearScreen(); }
        else if (choice == 3)   { optionWithdrawal();  clearScreen(); }
        else if (choice == 4)   { optionDeposit();     clearScreen(); }
        else if (choice == 5)   { optionPayments();    clearScreen(); }
        else if (choice == 6)   { optionChangePIN();   clearScreen(); }
        else if (choice == 101) { adminViewCash();     clearScreen(); }
        else if (choice == 102) { adminReloadCash();   clearScreen(); }
        else if (choice == 103) { adminShutdown(); }
    }
    return 0;
}
