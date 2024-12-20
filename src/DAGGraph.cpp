#include "DAG.h"
#include <iostream>

void DAG::addTransaction(const Transaction& txn) {
    transactions.push_back(txn);
}

void DAG::printGraph() const {
    std::cout << "DAG Contents: \n";
    for (const auto& txn : transactions) {
        txn.display();
    }
}
