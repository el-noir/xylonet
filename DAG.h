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
#include <stdexcept>

using namespace std;

struct TipInfo {
    std::string hash;         
    int cumulativeWeight;      
    time_t timestamp;        
    int amount;               
    int fee;                   

    TipInfo(const std::string& h, int cWeight, time_t ts, int amt, int f)
        : hash(h), cumulativeWeight(cWeight), timestamp(ts), amount(amt), fee(f) {}
};

template <typename T>
class Stack {
private:
    std::vector<T> container;

public:
    void push(const T& element) {
        container.push_back(element);
    }

    T pop() {
        if (container.empty()) {
            throw std::runtime_error("Stack Underflow! No elements to pop.");
        }
        T topElement = container.back();
        container.pop_back();
        return topElement;
    }

    T top() const {
        if (container.empty()) {
            throw std::runtime_error("Stack is empty! No top element.");
        }
        return container.back();
    }

    bool isEmpty() const {
        return container.empty();
    }

    size_t size() const {
        return container.size();
    }
};

class DAG {
private:
    std::unordered_map<std::string, std::vector<std::string>> adjList;      
    unordered_map<string, TransactionNode> transactions;   
    unordered_map<string, int> cumulativeWeights;           

    // Helper function to check for cycles in the DAG
    bool hasCycle(const std::string& node,
        std::unordered_set<std::string>& visited,
        Stack<std::string>& stack);


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
