#include "Wallet.h"
#include <iostream>
#include <stdexcept>
#include <ctime>  // For generating unique transaction IDs
#include <crypto++/base64.h>
#include <crypto++/sha.h>
#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <crypto++/files.h>
#include <sstream>
#include <iomanip>
#include "DAG.h"
#include "User.h"

using namespace CryptoPP;

// Constructor that initializes Wallet with userId and DAG reference
Wallet::Wallet(const std::string& userId, DAG& dag)
    : userId(userId), dag(dag) {}

double Wallet::getBalance() const {
    // Assuming the DAG handles the balance calculation for the user
    return dag.calculateBalance(userId);
}

void Wallet::viewTransactionHistory() const {
    auto transactions = dag.getTransactions();  // Get all transactions from DAG
    std::cout << "Transaction History for User: " << userId << "\n";
    for (const auto& txn : transactions) {
        if (txn.second->getSender() == userId || txn.second->getReceiver() == userId) {
            txn.second->print();  // Print transaction details
        }
    }
}

std::string Wallet::generateTransactionID(const std::string& sender, const std::string& receiver, double amount) const {
    // Concatenate sender, receiver, and amount for hash input
    std::string data = sender + receiver + std::to_string(amount) + std::to_string(std::time(nullptr));
    
    // Hash the data using SHA256
    CryptoPP::SHA256 hash;
    CryptoPP::byte hashResult[CryptoPP::SHA256::DIGESTSIZE];
    hash.Update(reinterpret_cast<const CryptoPP::byte*>(data.c_str()), data.length());
    hash.Final(hashResult);

    // Convert hash to hexadecimal string
    std::ostringstream oss;
    for (int i = 0; i < CryptoPP::SHA256::DIGESTSIZE; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hashResult[i];
    }

    return oss.str();
}

std::string Wallet::generateTransactionSignature(const std::string& data) const {
    // Generate a dummy RSA key for signing
    CryptoPP::RSA::PrivateKey privateKey;
    CryptoPP::AutoSeededRandomPool rng;
    privateKey.Initialize(rng, 2048);  // Initialize with a 2048-bit key

    // Sign the data
    CryptoPP::RSASSA_PKCS1v15_SHA_Signer signer(privateKey);
    std::string signature;

    CryptoPP::StringSource ss(data, true,
        new CryptoPP::SignerFilter(rng, signer, new CryptoPP::StringSink(signature)));

    return signature;
}

bool Wallet::transferFunds(Wallet& recipient, double amount, const std::string& senderUsername, const std::string& recipientUsername) {
    // Check if the transfer amount is valid (greater than zero)
    if (amount <= 0) {
        throw std::invalid_argument("Transfer amount must be positive.");
    }

    // Ensure the sender has enough balance to perform the transfer
    double senderBalance = getBalance();  // This will call the getBalance method
    if (senderBalance < amount) {
        throw std::invalid_argument("Insufficient funds for transfer.");
    }

    // Generate a unique transaction ID using SHA256
    std::string txnId = generateTransactionID(senderUsername, recipientUsername, amount);
    
    // Create the transaction data string
    std::string data = txnId + senderUsername + recipientUsername + std::to_string(amount);
    
    // Generate the signature for the transaction
    std::string signature = generateTransactionSignature(data);

    // Create the transaction in the DAG
    dag.createTransaction(txnId, senderUsername, recipientUsername, amount, 0.0, signature, data, 0);

    // Process the transfer in the DAG (subtract from sender and add to recipient)
    dag.subtractFundsFromUser(senderUsername, amount);
    dag.addFundsToUser(recipientUsername, amount);

    std::cout << "Successfully transferred " << amount << " from " << senderUsername << " to " << recipientUsername << std::endl;
    
    return true;  // Return true after successful transfer
}

// Deposit funds to this wallet (for this example, it's a direct deposit without using transactions)
void Wallet::deposit(double amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Deposit amount must be positive.");
    }
    // Assuming you can directly add funds to the user's balance in DAG (this is for simplicity)
    dag.addFundsToUser(userId, amount);
    std::cout << "Successfully deposited " << amount << " to wallet." << std::endl;
}

// Withdraw funds from this wallet (checks balance and modifies DAG accordingly)
bool Wallet::withdraw(double amount) {
    if (amount <= 0) {
        throw std::invalid_argument("Withdrawal amount must be positive.");
    }

    if (getBalance() < amount) {
        std::cout << "Insufficient funds for withdrawal." << std::endl;
        return false;  // Insufficient funds
    }

    // Assuming you can reduce funds for the user in the DAG (this is for simplicity)
    dag.subtractFundsFromUser(userId, amount);
    std::cout << "Successfully withdrew " << amount << " from wallet." << std::endl;
    return true;
}
