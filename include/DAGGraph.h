#ifndef DAGGRAPH_H
#define DAGGRAPH_H

#include <vector>
#include <string>
#include "Transaction.h"
#include "RSAEncryption.h"

class DAGGraph {
private:
    std::vector<Transaction> transactions; // Vector to store transactions
    std::vector<std::vector<Transaction>> graph; // Representing DAG structure
    
public:
    DAGGraph();
    void addTransaction(const Transaction& tx);
    bool validateTransaction(const Transaction& tx);
    std::vector<Transaction> getTransactions() const;
    bool isValidTransactionOrder(const Transaction& tx1, const Transaction& tx2);
    void printGraph() const;
    bool isDoubleSpending(const Transaction& tx) const;
};

#endif // DAGGRAPH_H
