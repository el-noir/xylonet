#include "Transaction.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sha.h>
#include <hex.h>
#include <string>
#include <sstream>
#include <memory>
#include <aes.h>
#include <modes.h>
#include <filters.h>

using namespace std;

// Constructor for Transaction
Transaction::Transaction(const std::string& id, const std::string& sender, const std::string& receiver, 
    double amount, double fee, time_t timestamp, const std::string& signature, 
    const std::vector<std::shared_ptr<Transaction>>& parentRefs) 
    : sender(sender), receiver(receiver), amount(amount), fee(fee),
    timestamp(timestamp), signature(signature), parentReferences(parentRefs), isValidated(false) {
    if (id.empty()) {
        this->id = generateTransactionId(); // Generate ID if not provided
    } else {
        this->id = id; // Use provided ID
    }
}

// Generate a unique transaction ID based on transaction details
std::string Transaction::generateTransactionId() const {
    // Simplified ID generation using the timestamp
    return std::to_string(std::time(nullptr));
}

// Perform Proof of Work
bool Transaction::performProofOfWork(int difficulty) {
    std::string target(difficulty, '0');
    std::string hash;
    int nonce = 0;

    CryptoPP::SHA256 hashFunction;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hash));

    do {
        nonce++;
        std::string input = generateTransactionHash() + std::to_string(nonce);
        CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
        hashFunction.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(input.c_str()), input.length());

        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();
        hash = hash;
    } while (hash.substr(0, difficulty) != target);

    std::cout << "Proof of Work completed: " << hash << std::endl;
    return true;
}

// Generate the transaction hash
std::string Transaction::generateTransactionHash() const {
    std::string input = id + sender + receiver + std::to_string(amount) +
                        std::to_string(fee) + std::to_string(timestamp) + signature;

    CryptoPP::SHA256 hash;
    CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
    hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(input.c_str()), input.length());

    CryptoPP::HexEncoder encoder;
    std::string hashHex;
    encoder.Attach(new CryptoPP::StringSink(hashHex));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return hashHex;
}

// Print the transaction details
void Transaction::print() const {
    std::cout << "Transaction ID: " << id << "\n";
    std::cout << "Sender: " << (sender.empty() ? "System" : sender) << "\n";
    std::cout << "Receiver: " << receiver << "\n";
    std::cout << "Amount: " << amount << "\n";
    std::cout << "Fee: " << fee << "\n";

    std::tm tm;
    localtime_r(&timestamp, &tm);
    std::cout << "Timestamp: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n";
    std::cout << "Signature: " << (signature.empty() ? "None" : signature) << "\n";

    if (!parentReferences.empty()) {
        std::cout << "Parent Transaction IDs: ";
        for (const auto& parent : parentReferences) {
            std::cout << parent->getId() << " ";
        }
        std::cout << "\n";
    } else {
        std::cout << "Parent Transactions: None\n";
    }

    std::cout << "Validation Status: " << (isValidated ? "Validated" : "Not Validated") << "\n";
    std::cout << std::endl;
}

// Getters
std::string Transaction::getId() const { return id; }
std::string Transaction::getSender() const { return sender; }
std::string Transaction::getReceiver() const { return receiver; }
double Transaction::getAmount() const { return amount; }
double Transaction::getFee() const { return fee; }
std::time_t Transaction::getTimestamp() const { return timestamp; }

std::vector<std::shared_ptr<Transaction>> Transaction::getParentReferences() const {
    return parentReferences;
}

bool Transaction::getValidationStatus() const { return isValidated; }

// Validation methods
void Transaction::validate() {
    isValidated = true;
}

void Transaction::invalidate() {
    isValidated = false;
}
