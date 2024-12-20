#include "Wallet.h"
#include "RSAEncryption.h"
#include <iostream>

Wallet::Wallet() {
    RSAEncryption::generateKeyPair(privateKey, publicKey);  // Generate RSA key pair on wallet creation
    balance = 0.0;  // Initialize balance to 0
}

Wallet::Wallet(const std::string& privateKey, const std::string& publicKey)
    : privateKey(privateKey), publicKey(publicKey), balance(0.0) {}

std::string Wallet::getWalletID() const {
    return publicKey;  // Return public key as wallet ID
}

std::string Wallet::getPublicKey() const {
    return publicKey;
}

std::string Wallet::getPrivateKey() const {
    return privateKey;
}

double Wallet::getBalance() const {
    return balance;
}

void Wallet::setBalance(double newBalance) {
    balance = newBalance;
}

bool Wallet::sendTransaction(Wallet& recipient, double amount) {
    if (balance >= amount) {  // Check if the balance is sufficient for the transaction
        Transaction tx(publicKey, recipient.getPublicKey(), amount);  // Create new transaction
        signTransaction(tx);  // Sign the transaction with this wallet's private key
        recipient.receiveTransaction(tx);  // Send the transaction to the recipient
        balance -= amount;  // Deduct the amount from this wallet's balance
        return true;
    }
    return false;  // Insufficient funds
}

void Wallet::receiveTransaction(const Transaction& tx) {
    balance += tx.getAmount();  // Add the received amount to the wallet's balance
}

void Wallet::signTransaction(Transaction& tx) {
    tx.signTransaction(privateKey);  // Sign the transaction with the private key
}

bool Wallet::validateTransaction(const Transaction& tx) const {
    return tx.verifySignature();  // Verify the transaction's signature using the public key
}
