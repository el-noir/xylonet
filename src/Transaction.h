#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include <ctime>
#include <memory>
#include <iostream>

namespace CryptoPP {
    class SHA256;
    class HexEncoder;
}

class Transaction {
public:
    // Constructor
    Transaction(const std::string& id, const std::string& sender, const std::string& receiver, 
        double amount, double fee, time_t timestamp, const std::string& signature, 
        const std::vector<std::shared_ptr<Transaction>>& parentRefs);

    // Generate transaction ID based on transaction details
    std::string generateTransactionId() const;

    // Perform Proof of Work
    bool performProofOfWork(int difficulty);

    // Generate the transaction hash
    std::string generateTransactionHash() const;

    // Print transaction details
    void print() const;

    // Getters
    std::string getId() const;
    std::string getSender() const;
    std::string getReceiver() const;
    double getAmount() const;
    double getFee() const;
    std::time_t getTimestamp() const;
    std::vector<std::shared_ptr<Transaction>> getParentReferences() const;
    bool getValidationStatus() const;

    // Validation methods
    void validate();
    void invalidate();

private:
    std::string id;
    std::string sender;
    std::string receiver;
    double amount;
    double fee;
    std::time_t timestamp;
    std::string signature;
    std::vector<std::shared_ptr<Transaction>> parentReferences;
    bool isValidated;
};

#endif
