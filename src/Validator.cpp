#include "Validator.h"
#include "Transaction.h"
#include "RSAEncryption.h"
#include <iostream>

Validator::Validator(DAGGraph& dag, HybridConsensus& consensus) 
    : dag(dag), consensus(consensus) {}

bool Validator::isValidTransaction(const Transaction& tx) {
    return validateSignature(tx) && !validateDoubleSpending(tx);
}

bool Validator::validateTransactionOrder(const Transaction& tx1, const Transaction& tx2) {
    return dag.isValidTransactionOrder(tx1, tx2);
}

bool Validator::isTransactionValidForConsensus(const Transaction& tx) {
    return consensus.validateTransaction(tx);
}

bool Validator::validateDoubleSpending(const Transaction& tx) {
    return dag.isDoubleSpending(tx);
}

bool Validator::validateSignature(const Transaction& tx) {
    return tx.verifySignature();
}
