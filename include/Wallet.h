#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include "RSAEncryption.h"
#include "Transaction.h"

class Wallet {
private:
    std::string walletID;
    std::string privateKey;
    std::string publicKey;
    double balance;
    
public:
    Wallet();
    Wallet(const std::string& privateKey, const std::string& publicKey);
    
    std::string getWalletID() const;
    std::string getPublicKey() const;
    std::string getPrivateKey() const;
    double getBalance() const;
    void setBalance(double newBalance);
    
    bool sendTransaction(Wallet& recipient, double amount);
    void receiveTransaction(const Transaction& tx);
    void signTransaction(Transaction& tx);
    
    bool validateTransaction(const Transaction& tx) const;
};

#endif // WALLET_H
