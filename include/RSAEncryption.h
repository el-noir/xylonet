#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <vector>
#include "Transaction.h"
#include "DAGGraph.h"
#include "RSAEncryption.h"
#include "HybridConsensus.h"
class Validator {
private:
    DAGGraph dag;
    HybridConsensus consensus;

public:
    Validator(DAGGraph& dag, HybridConsensus& consensus);
    bool isValidTransaction(const Transaction& tx);
    bool validateTransactionOrder(const Transaction& tx1, const Transaction& tx2);
    bool isTransactionValidForConsensus(const Transaction& tx);
    bool validateDoubleSpending(const Transaction& tx);
    bool validateSignature(const Transaction& tx);
};

#endif // VALIDATOR_H
