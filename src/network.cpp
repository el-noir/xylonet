#include "Network.h"

// Constructor accepts DAG instance
Network::Network(DAG& dagInstance) : dag(dagInstance) {}

// Add User to the network
bool Network::addUser(const User& user) {
    std::cout << "Adding user: " << user.getUsername() << std::endl;
    std::string username = user.getUsername();

    // Check if the user already exists
    for (const auto& existingUser : users) {
        if (existingUser.getUsername() == username) {
            std::cout << "Error: User with username " << username << " already exists." << std::endl;
            return false;  // User already exists, return false
        }
    }

    // If user doesn't exist, add the user
    users.push_back(user);
    connections[username] = {};  // Initialize the user's connection list

    // Update connections for the new user and the existing users
    for (const auto& existingUser : users) {
        std::string existingUsername = existingUser.getUsername();
        if (existingUsername != username) {
            connections[username].push_back(existingUsername);
            connections[existingUsername].push_back(username);  // Create bidirectional connections
        }
    }

    return true;  // Successfully added the user
}

// List all users in the network
void Network::listUsers() const {
    cout << "Users in the Network:" << endl;
    for (const auto& user : users) {
        cout << user.getUsername() << endl;
    }
}

// Validate user login
bool Network::loginUser(const string& username, const string& password) {
    for (const auto& user : users) {
        if (user.getUsername() == username) {
            if (user.validateLogin(username, password)) {
                return true;
            } else {
                cout << "Password mismatch for user: " << username << endl;
                return false;
            }
        }
    }
    cout << "Username not found!" << endl;
    return false;
}

// Send message from one user to another
void Network::sendMessage(const string& fromUsername, const string& toUsername, const string& message) {
    if (connections[fromUsername].empty()) {
        cout << fromUsername << " has no connections to send messages!" << endl;
        return;
    }

    if (find(connections[fromUsername].begin(), connections[fromUsername].end(), toUsername) != connections[fromUsername].end()) {
        cout << fromUsername << " sent a message to " << toUsername << ": " << message << endl;
    } else {
        cout << "Users are not connected!" << endl;
    }
}

// Transfer funds between users
bool Network::transferFunds(const string& senderUsername, const string& recipientUsername, double amount) {
    User* sender = getUser(senderUsername);
    User* recipient = getUser(recipientUsername);

    if (!sender) {
        std::cout << "Sender not found in the network!" << std::endl;
        return false;
    }
    if (!recipient) {
        std::cout << "Recipient not found in the network!" << std::endl;
        return false;
    }

    try {
        sender->getWallet().transferFunds(recipient->getWallet(), amount, senderUsername, recipientUsername);
        std::cout << "Funds transferred successfully from " << senderUsername << " to " << recipientUsername << "!" << std::endl;
        return true;
    } catch (const std::invalid_argument& e) {
        std::cout << "Transfer failed: " << e.what() << std::endl;
        return false;
    }
}

// List all transactions in the network
void Network::listAllTransactions() const {
    auto transactions = dag.getTransactions();  // Assuming dag.getTransactions() returns a list of transactions

    cout << "\nDisplaying all transactions:\n";
    for (const auto& txn : transactions) {
        // Assuming Transaction has a print method
        txn.second->print();
    }
}

// Get User by username
User* Network::getUser(const string& username) {
    for (auto& user : users) {
        if (user.getUsername() == username) {
            return &user;
        }
    }
    return nullptr;
}

// Access the DAG instance
DAG& Network::getDAG() {
    return dag;
}
