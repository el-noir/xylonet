#ifndef HYBRIDCONSENSUS_H
#define HYBRIDCONSENSUS_H

#include <vector>
#include <string>
#include "DAGGraph.h"
#include "Transaction.h"
#include "RSAEncryption.h"

class HybridConsensus {
private:
    DAGGraph dag;
    double minConfirmationThreshold;  // Threshold to confirm a transaction
    double networkLoadThreshold;      // Threshold to switch consensus method

public:
    HybridConsensus(double minConfirmationThreshold = 3.0, double networkLoadThreshold = 70.0);
    bool validateConsensus(const Transaction& tx);
    bool executeTipSelection(const Transaction& tx);
    bool executeGHOSTDAG(const Transaction& tx);
    bool validateTransaction(const Transaction& tx);
    void dynamicallyAdjustConsensus(double networkLoad);
    bool performTransactionValidation(const Transaction& tx);
};

#endif // HYBRIDCONSENSUS_H
