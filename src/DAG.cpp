#include "DAG.h"
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include "Transaction.h"

// Create transaction
std::shared_ptr<Transaction> DAG::createTransaction(const std::string& id, const std::string& sender,
    const std::string& receiver, double amount, double fee, const std::string& signature,
    const std::string& parentId, int difficulty) {

    // Use timestamp as the ID if the provided ID is empty
    std::string txnId = id.empty() ? std::to_string(std::time(nullptr)) : id;

    // Create a vector to hold parent references
    std::vector<std::shared_ptr<Transaction>> parentRefs;
    if (!parentId.empty()) {
        try {
            auto parentTxn = getTransactionById(parentId);  // Assuming this function returns a shared pointer
            parentRefs.push_back(parentTxn);
        } catch (const std::invalid_argument& e) {
            std::cout << "Warning: " << e.what() << " - Creating a first transaction without parent." << std::endl;
        }
    }

    // Now, create the transaction with the simplified ID
    auto txnPtr = std::make_shared<Transaction>(txnId, sender, receiver, amount, fee, std::time(nullptr), signature, parentRefs);

    std::cout << "Created transaction with ID: " << txnPtr->getId() << std::endl;

    // Perform Proof of Work here
    if (txnPtr->performProofOfWork(difficulty)) {
        addTransaction(txnPtr);
        std::cout << "Transaction " << txnPtr->getId() << " added to DAG." << std::endl;
    } else {
        throw std::runtime_error("Proof of Work failed.");
    }

    return txnPtr;
}



// Get transaction by ID
std::shared_ptr<Transaction> DAG::getTransactionById(const std::string& id) const {
    auto it = transactions.find(id);
    if (it == transactions.end()) {
        std::cout << "Error: Transaction with ID " << id << " not found!" << std::endl;
        throw std::invalid_argument("Transaction not found.");
    }
    return it->second;
}

// Add transaction
void DAG::addTransaction(const std::shared_ptr<Transaction>& txn) {
    if (transactions.find(txn->getId()) != transactions.end()) {
        throw std::invalid_argument("Transaction already exists.");
    }

    std::cout << "Adding transaction with ID: " << txn->getId() << std::endl;

    for (const auto& parentTxn : txn->getParentReferences()) {
        if (transactions.find(parentTxn->getId()) == transactions.end()) {
            throw std::invalid_argument("Parent transaction not found.");
        }

        std::unordered_set<std::string> visited;
        std::unordered_set<std::string> recStack;
        if (isCyclic(txn, parentTxn, visited, recStack)) {
            throw std::invalid_argument("Cycle detected in the DAG.");
        }
    }

    transactions[txn->getId()] = txn;
    std::cout << "Transaction " << txn->getId() << " added successfully!" << std::endl;
}

// Cycle detection
bool DAG::isCyclic(const std::shared_ptr<Transaction>& txn, const std::shared_ptr<Transaction>& parentTxn,
    std::unordered_set<std::string>& visited, std::unordered_set<std::string>& recStack) {

    if (recStack.find(parentTxn->getId()) != recStack.end()) {
        return true;
    }

    if (visited.find(parentTxn->getId()) != visited.end()) {
        return false;
    }

    visited.insert(parentTxn->getId());
    recStack.insert(parentTxn->getId());

    for (const auto& parent : parentTxn->getParentReferences()) {
        if (isCyclic(txn, parent, visited, recStack)) {
            return true;
        }
    }

    recStack.erase(parentTxn->getId());
    return false;
}

// Get total number of transactions in the DAG
int DAG::getTransactionCount() const {
    return transactions.size();
}

// Calculate balance of user based on sender-receiver transactions
double DAG::calculateBalance(const std::string& userId) const {
    double balance = 0.0;
    for (const auto& txnPair : transactions) {
        const auto& txn = txnPair.second;
        if (txn->getSender() == userId) {
            balance -= txn->getAmount();
        }
        if (txn->getReceiver() == userId) {
            balance += txn->getAmount();
        }
    }
    return balance;
}

// List all transactions
void DAG::listTransactions() const {
    for (const auto& txnPair : transactions) {
        txnPair.second->print();
    }
}

// Add funds to user's balance
void DAG::addFundsToUser(const std::string& userId, double amount) {
    // Add funds to the user by creating a transaction with the receiver as the user
    std::shared_ptr<Transaction> txn = createTransaction("", "System", userId, amount, 0, "", "", 0);
}
// Return all transactions in the DAG
std::map<std::string, std::shared_ptr<Transaction>> DAG::getTransactions() const {
    return transactions;
}

// Subtract funds from user's balance
bool DAG::subtractFundsFromUser(const std::string& userId, double amount) {
    double balance = calculateBalance(userId);
    if (balance >= amount) {
        createTransaction("", userId, "System", amount, 0, "", "", 0);
        return true;
    }
    return false;
}

// Print the graph using DFS
void DAG::printGraphDFS() const {
    std::unordered_set<std::string> visited;

    for (const auto& txnPair : transactions) {
        if (visited.find(txnPair.first) == visited.end()) {
            dfsHelper(txnPair.second, visited);
        }
    }
}

void DAG::dfsHelper(const std::shared_ptr<Transaction>& txn, std::unordered_set<std::string>& visited) const {
    if (!txn || visited.find(txn->getId()) != visited.end()) {
        return;
    }

    visited.insert(txn->getId());
    std::cout << "Transaction ID: " << txn->getId() << std::endl;

    for (const auto& parent : txn->getParentReferences()) {
        dfsHelper(parent, visited);
    }
}
