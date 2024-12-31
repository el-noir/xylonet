#include <iostream>
#include <limits>
#include <vector>
#include "src/Transaction.h"
#include "src/User.h"
#include "src/Network.h"
#include "src/Xylonet.h"

using namespace std;


// Helper function to display the main menu for logged-in users
void displayMainMenu(const User& user) {
    cout << "\n\t\t=============================================" << endl;
    cout << "\t\t Welcome, " << user.getUsername() << "!" << endl;
    cout << "\t\t=============================================" << endl;
    cout << "Email: " << user.getEmail() << endl;
    cout << "Wallet Balance: " << user.getWalletBalance() << " units." << endl;

    cout << "\n\t\t=============================================" << endl;
    cout << "\t\t Please choose an action:" << endl;
    cout << "\t\t 1. Transfer Money" << endl;
    cout << "\t\t 2. View Previous Transactions" << endl;
    cout << "\t\t 3. Logout" << endl;
    cout << "\t\t 4. Exit" << endl;
    cout << "\t\t=============================================" << endl;
}

// Helper function to handle login
bool login(Network& network, User*& loggedInUser) {
    string username, password;
    int loginAttempts = 0;

    while (loginAttempts < 3) {
        cout << "\nEnter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        if (network.loginUser(username, password)) {
            loggedInUser = network.getUser(username);
            return true;
        } else {
            cout << "\nInvalid credentials. Please try again.\n";
            loginAttempts++;
        }
    }

    cout << "\nToo many failed attempts. Please try again later.\n";
    return false;
}

// Function to create a new user account
void createAccount(Network& network, DAG& dag) {
    string username, password, email;
    double initialBalance;

    cout << "\n=== Create Account ===" << endl;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter email: ";
    cin >> email;

    cout << "Enter initial balance: ";
    while (!(cin >> initialBalance) || initialBalance <= 0) {
        // Clear the input buffer if the input is invalid
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (initialBalance <= 0) {
            cout << "Deposit amount must be positive. Please enter a positive value: ";
        } else {
            cout << "Invalid balance. Please enter a valid number: ";
        }
    }

    User newUser(username, password, email, 0.0, dag);

    if (network.addUser(newUser)) {
        // Deposit the valid initial balance
        newUser.depositToWallet(initialBalance);
        cout << "\nAccount created successfully with an initial balance of " << initialBalance << " units!\n";
    } else {
        cout << "\nFailed to create account. Username might already exist.\n";
    }
}

// Main program logic
int main() {
    DAG dag;
    Network network(dag);

    vector<User> predefinedUsers = {
        User("HassanAli", "password1", "hassan@example.com", 1000.0, dag),
        User("MudasirShah", "password2", "mudasir@example.com", 1500.0, dag),
        User("RabbiaJamil", "password3", "rabbia@example.com", 2000.0, dag)
    };

    for (const auto& user : predefinedUsers) {
        network.addUser(user);
    }

    while (true) {
        cout << "\n\t\t=============================================" << endl;
        cout << "\t\t Welcome to the Xylonet Transaction System!" << endl;
        cout << "\t\t=============================================" << endl;
        cout << "\t\t 1. Log In" << endl;
        cout << "\t\t 2. Create Account" << endl;
        cout << "\t\t 3. View Graph (DFS)" << endl;
        cout << "\t\t 4. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (choice == 1) {
            User* loggedInUser = nullptr;
            if (login(network, loggedInUser)) {
                bool loggedIn = true;
                while (loggedIn) {
                    displayMainMenu(*loggedInUser);
                    int action;
                    cout << "Enter your choice: ";
                    while (!(cin >> action)) {
                        cout << "Invalid input. Please enter a number: ";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }

                    switch (action) {
                        case 1:
                            createTransactionExample(*loggedInUser, network);
                            break;
                        case 2:
                            cout << "\nPrevious Transactions:" << endl;
                            loggedInUser->listTransactions();
                            break;
                        case 3:
                            cout << "\nLogging out..." << endl;
                            loggedIn = false;
                            break;
                        default:
                            cout << "\nInvalid choice! Please try again." << endl;
                            break;
                    }
                }
            }
        } else if (choice == 2) {
            createAccount(network, dag);
        } else if (choice == 3) {
            cout << "\nDAG Graph:" << endl;
            dag.printGraphDFS();
        } else if (choice == 4) {
            cout << "\nExiting the system. Goodbye!" << endl;
            break;
        } else {
            cout << "\nInvalid choice! Please try again." << endl;
        }
    }

    return 0;
}