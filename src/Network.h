#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "User.h"
#include "DAG.h"  // Include DAG header to use DAG class

using namespace std;

class Network {
private:
    vector<User> users;
    unordered_map<string, User*> userMap;
    unordered_map<string, vector<string>> connections;
    DAG dag;  // Adding DAG instance to Network

public:
    // Constructor accepts DAG instance
    Network(DAG& dagInstance);

    // Add User to the network
    bool addUser(const User& user);

    // List all users in the network
    void listUsers() const;

    // Validate user login
    bool loginUser(const string& username, const string& password);

    // Send message from one user to another
    void sendMessage(const string& fromUsername, const string& toUsername, const string& message);

    // Transfer funds between users
    bool transferFunds(const string& senderUsername, const string& recipientUsername, double amount);

    // List all transactions in the network
    void listAllTransactions() const;

    // Get User by username
    User* getUser(const string& username);

    // Access the DAG instance
    DAG& getDAG();
};

#endif
