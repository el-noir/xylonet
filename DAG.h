#ifndef DAG_H
#define DAG_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <cmath>
#include "TransactionNode.h"
#include "HashUtils.h"

using namespace std;

// Structure for tip information (transaction data)
struct TipInfo {
    std::string hash;           // Hash of the transaction
    int cumulativeWeight;       // Cumulative weight (approvals count)
    time_t timestamp;           // Timestamp of the transaction
    int amount;                 // Transaction amount
    int fee;                    // Transaction fee

    // Constructor to initialize TipInfo object with necessary parameters
    TipInfo(const std::string& h, int cWeight, time_t ts, int amt, int f)
        : hash(h), cumulativeWeight(cWeight), timestamp(ts), amount(amt), fee(f) {}
};

class DAG {
private:
    unordered_map<string, vector<string>> adjList;          // Adjacency list for the DAG
    unordered_map<string, TransactionNode> transactions;    // Transactions mapped by hash
    unordered_map<string, int> cumulativeWeights;           // Cumulative weights for transactions

    // Helper function to check for cycles in the DAG
    bool hasCycle(const string& node, unordered_set<string>& visited, unordered_set<string>& Stack);

    // Recursive function to update cumulative weights for each transaction
    int updateCumulativeWeights(const string& hash);

public:
    // Constructor and Destructor
    DAG() = default;
    ~DAG() = default;

    // Function to select parents using Markov Chain Monte Carlo (MCMC) method
    vector<string> selectParentsMCMC(size_t numParents = 2);
    void performConsensus(double validationThreshold);
    bool validateTransaction(const std::string& hash, double validationThreshold, std::unordered_set<std::string>& visited);
    // Function to add a new transaction to the DAG
    bool addTransaction(TransactionNode& transaction);

    // Function to print the DAG details (transactions and adjacency list)
    void printDAG() const;
    double calculateFee(double amount);
    // Function to save transactions to a file
    void saveTransactionsToFile(const std::string& filename);

    // Function to load transactions from a file
    bool loadTransactionsFromFile(const std::string& filename);

    // Getter for transactions map
    const unordered_map<string, TransactionNode>& getTransactions() const {
        return transactions;
    }
};

#endif // DAG_H
