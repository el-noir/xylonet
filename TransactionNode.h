#ifndef TRANSACTION_NODE_H
#define TRANSACTION_NODE_H

#include <string>
#include <vector>
#include <ctime>

class TransactionNode {
public:
    // Attributes of a transaction
    std::string id;                        // Unique identifier of the transaction
    std::string senderAcc;                 // Sender account
    std::string receiverAcc;               // Receiver account
    double amount=0.0;                         // Transaction amount
    double fee=0.0;                            // Transaction fee
    time_t timestamp;                      // Timestamp of the transaction
    std::string hash;                      // Unique hash for the transaction
    std::vector<std::string> parentHashes; // Hashes of parent transactions
    bool isValidated = false;                       // Validation status of the transaction

    // Constructors
    TransactionNode() = default;  // Default constructor
    TransactionNode(const std::string& id, const std::string& senderAcc, const std::string& receiverAcc,
        double amount, double fee, time_t timestamp, const std::vector<std::string>& parentHashes, bool isValidated = false);

    // Destructor
    ~TransactionNode() = default;
};

#endif // TRANSACTION_NODE_H
