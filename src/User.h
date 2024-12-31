#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include "Wallet.h"
#include "DAG.h"

using namespace std;

class User {
private:
    std::string username;
    std::string password;
    std::string email;
    Wallet wallet;

public:
    User(const std::string& username, const std::string& password, const std::string& email, double initialBalance, DAG& dag);
   
    Wallet& getWallet();
    void performTransactionWithPoW(DAG& dag, const std::string& recipientUsername, double amount, int difficulty);
    std::string getUsername() const;
    std::string getEmail() const;
    double getWalletBalance() const;

    void displayUserInfo() const;

    bool validateLogin(const std::string& inputUsername, const std::string& inputPassword) const;

    void depositToWallet(double amount);

    bool withdrawFromWallet(double amount);

    void listTransactions() const;  // Renamed method for better clarity
};

#endif
