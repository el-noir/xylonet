#include "Transaction.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "RSAEncryption.h"
#include "RSAEncryption.cpp"

Transaction::Transaction(std::string sender, std::string receiver, double amount, const std::string& privateKey) 
    : sender(sender), receiver(receiver), amount(amount), timestamp(time(0)) {
    generateTransactionID();
    this->privateKey = privateKey;

    // Generate public key from the private key
    this->publicKey = RSAEncryption::getPublicKeyFromPrivate(privateKey);
}

void Transaction::generateTransactionID() {
    std::stringstream ss;
    ss << sender << receiver << amount << timestamp;
    this->transactionID = std::to_string(std::hash<std::string>{}(ss.str()));
}

std::string Transaction::getTransactionID() const {
    return transactionID;
}

std::string Transaction::getSender() const {
    return sender;
}

std::string Transaction::getReceiver() const {
    return receiver;
}

double Transaction::getAmount() const {
    return amount;
}

time_t Transaction::getTimestamp() const {
    return timestamp;
}

std::string Transaction::getSignature() const {
    return signature;
}

void Transaction::signTransaction() {
    signature = RSAEncryption::sign(transactionID, privateKey);
}

bool Transaction::verifySignature() const {
    return RSAEncryption::verify(transactionID, signature, publicKey);
}

bool Transaction::isValid() const {
    return verifySignature();
}

std::string Transaction::encryptData(const std::string& data) const {
    return RSAEncryption::encrypt(data, publicKey);
}

std::string Transaction::decryptData(const std::string& encryptedData) const {
    return RSAEncryption::decrypt(encryptedData, privateKey);
}
