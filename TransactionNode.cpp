#include "TransactionNode.h"
#include <sstream>
#include <functional>
#include <ctime>
#include "HashUtils.h"
namespace std {
    template <>
    struct hash<TransactionNode> {
        size_t operator()(const TransactionNode& txn) const {
            return hash<string>{}(txn.hash);  // Using the hash of the 'hash' field in TransactionNode
        }
    };
}


// Constructor implementation
TransactionNode::TransactionNode(const std::string& id, const std::string& senderAcc, const std::string& receiverAcc,
    double amount, double fee, time_t timestamp, const std::vector<std::string>& parentHashes, bool isValidated)
    : id(id), senderAcc(senderAcc), receiverAcc(receiverAcc), amount(amount), fee(fee),
    timestamp(timestamp), parentHashes(parentHashes), isValidated(isValidated) {
    // Generate hash within the constructor
    this->hash = generateHash(std::to_string(timestamp) + id);
}