#include "Enclave_t.h"
#include <string.h>
#include <array>

/*
struct transaction_entry{
    char description[32];
    float amount;
};
*/

struct transaction{
    float balance;
    std::array<transaction_entry, 3> transactions;
    int nextIdx;
    int firstIdx;


};

static transaction myTransaction{1000.00f, {}, 0, -1};

void addTransaction(const char* type, float amount) {
    bool bufferFull = myTransaction.firstIdx != -1 && (myTransaction.firstIdx == myTransaction.nextIdx);

    transaction_entry entry;  // Create a local instance, not just a pointer
    entry.amount = amount;
    strncpy(entry.description, type, sizeof(entry.description) - 1);
    entry.description[sizeof(entry.description) - 1] = '\0';  // Ensure null termination

    myTransaction.transactions[myTransaction.nextIdx] = entry;

    if (bufferFull) {
        myTransaction.firstIdx = (myTransaction.firstIdx + 1) % 3;
    }
    else if (myTransaction.firstIdx == -1) {
        myTransaction.firstIdx = myTransaction.nextIdx;
    }

    myTransaction.nextIdx = (myTransaction.nextIdx + 1) % 3;
}

void ecall_deposit(float amount){


    // updates balance
    myTransaction.balance += amount;


    // log transaction

    addTransaction("DEPOSIT", amount);

}

void ecall_pay(float amount){
    myTransaction.balance -= amount;


    addTransaction("PAY", amount);
}

void ecall_viewBalance(){
    ocall_viewBalance(myTransaction.balance);
}

void ecall_checkTransaction() {
    transaction_entry entries[3]; // Stack-allocated (safe)
    size_t count = 0;

    if (myTransaction.firstIdx != -1) {
        int current = myTransaction.firstIdx;
        bool full = (myTransaction.nextIdx == myTransaction.firstIdx);

        do {
            entries[count] = myTransaction.transactions[current];
            current = (current + 1) % 3;
            count++;
        } while (count < 3 && (full || current != myTransaction.nextIdx));
    }

    
    ocall_checkTransaction(entries, count);
}