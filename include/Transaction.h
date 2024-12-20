#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <openssl/sha.h>
#include "Wallet.h"

class Transaction {
public:
    std::string id;                // Unique ID of the transaction
    std::string senderAddress;     // Sender's wallet address (public key hash)
    std::string receiverAddress;   // Receiver's wallet address (public key hash)
    double amount;                 // Amount being transferred
    double fee;                    // Transaction fee
    std::time_t timestamp;         // Timestamp of the transaction
    std::string signature;         // Signature to prove the authenticity of the transaction

    // Constructor to initialize a transaction
    Transaction(std::string sender, std::string receiver, double amt, double txnFee);

    // Method to generate a unique transaction ID
    std::string generateTransactionId();

    // Serialize the transaction data (for storing or transmitting)
    std::string serializeTransaction();

    // Sign the transaction using the sender's private key
    void signTransaction(Wallet& wallet);

    // Verify the transaction signature using the sender's public key
    bool verifyTransactionSignature(Wallet& wallet);

    // Validate the transaction (e.g., check if sender has enough balance, etc.)
    bool validateTransaction(Wallet& wallet);

private:
    // Helper method to compute the SHA256 hash of a string
    std::string computeHash(const std::string& data);
};

#endif // TRANSACTION_H
