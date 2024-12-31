#ifndef DAG_H
#define DAG_H

#include "Transaction.h"
#include <string>
#include <map>
#include <memory>
#include <unordered_set>

class DAG {
public:
    // Create transaction
    std::shared_ptr<Transaction> createTransaction(const std::string& id, const std::string& sender,
        const std::string& receiver, double amount, double fee, const std::string& signature,
        const std::string& parentId, int difficulty);

    // Get transaction by ID
    std::shared_ptr<Transaction> getTransactionById(const std::string& id) const;

    // Add transaction to DAG
    void addTransaction(const std::shared_ptr<Transaction>& txn);
// Return all transactions in the DAG
std::map<std::string, std::shared_ptr<Transaction>> getTransactions() const;

    // Detect cycles in DAG
    bool isCyclic(const std::shared_ptr<Transaction>& txn, const std::shared_ptr<Transaction>& parentTxn,
        std::unordered_set<std::string>& visited, std::unordered_set<std::string>& recStack);
        void printGraphDFS() const;
    void dfsHelper(const std::shared_ptr<Transaction>& txn, std::unordered_set<std::string>& visited) const ;
    // Get transaction count
    int getTransactionCount() const;

    // Calculate balance
    double calculateBalance(const std::string& userId) const;

    // Get ancestry
    std::vector<std::shared_ptr<Transaction>> getAncestry(const std::shared_ptr<Transaction>& txn) const;

    // Get descendants
    std::vector<std::shared_ptr<Transaction>> getDescendants(const std::shared_ptr<Transaction>& txn) const;

    // List all transactions
    void listTransactions() const;

    // Add funds to user
    void addFundsToUser(const std::string& userId, double amount);

    // Subtract funds from user
    bool subtractFundsFromUser(const std::string& userId, double amount);

private:
    std::map<std::string, std::shared_ptr<Transaction>> transactions;
};

#endif
