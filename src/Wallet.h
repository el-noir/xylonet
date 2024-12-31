#ifndef WALLET_H
#define WALLET_H

#include <vector>
#include <memory>
#include <string>
#include <ctime>
#include "Transaction.h"
#include "DAG.h"
#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <crypto++/sha.h>
#include <crypto++/files.h>
#include <crypto++/base64.h>

class Wallet {
public:
    // Constructor that initializes Wallet with userId and DAG reference
    Wallet(const std::string& userId, DAG& dag);
    
    double getBalance() const;
    void deposit(double amount);
    bool withdraw(double amount);
    bool transferFunds(Wallet& recipient, double amount, const std::string& senderUsername, const std::string& recipientUsername);
    void recordTransaction(const std::shared_ptr<Transaction>& txn);
    const std::vector<std::shared_ptr<Transaction>>& getTransactionHistory() const;
    void viewTransactionHistory() const;

private:
    std::string userId;  // Added userId to the Wallet class
    DAG& dag;            // Reference to the DAG instance
    std::vector<std::shared_ptr<Transaction>> transactionHistory;

    std::string generateTransactionID(const std::string& sender, const std::string& receiver, double amount) const;
    std::string generateTransactionSignature(const std::string& data) const;
};

#endif // WALLET_H
