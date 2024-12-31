#include <iostream>
#include "DAG.h"
#include "Transaction.h"
#include "User.h"
#include "Wallet.h"
#include "Network.h"
#include "Xylonet.h"

// Function to simulate transaction creation for a logged-in user
void createTransactionExample(User& loggedInUser, Network& network) {
    try {
        // Show logged-in user information
        std::cout << "Logged in user: " << loggedInUser.getUsername() << "\n";
        std::cout << "Email: " << loggedInUser.getEmail() << "\n";
        std::cout << "Wallet Balance: " << loggedInUser.getWalletBalance() << " units.\n";

        // Get another user to interact with (for transaction purposes)
        std::string recipientUsername;
        std::cout << "Enter the recipient username: ";
        std::cin >> recipientUsername;

        User* recipient = network.getUser(recipientUsername);
        if (!recipient) {
            std::cout << "Recipient not found in the network!\n";
            return;
        }

        // Amount to transfer
        double amount;
        std::cout << "Enter the amount to transfer: ";
        std::cin >> amount;

        // Check if the sender has enough balance
        Wallet& senderWallet = loggedInUser.getWallet();
        if (senderWallet.getBalance() < amount) {
            std::cout << "Insufficient funds. Transaction aborted.\n";
            return;
        }

        // Attempt to transfer funds
        Wallet& recipientWallet = recipient->getWallet();
        senderWallet.transferFunds(recipientWallet, amount, loggedInUser.getUsername(), recipient->getUsername());

        std::cout << "Transaction successful!\n";

        // Create a new transaction in the DAG and link it to the previous transactions
        DAG& transactionGraph = network.getDAG();
        auto txn = transactionGraph.createTransaction(
            "txn" + std::to_string(transactionGraph.getTransactionCount() + 1),
            loggedInUser.getUsername(), recipient->getUsername(), amount, 
            0.5, "signature" + std::to_string(transactionGraph.getTransactionCount() + 1),
            "", 1); // Assume "" means no parent transaction
        txn->print();  // Print the transaction details

        // Display updated user info after transaction
        loggedInUser.displayUserInfo();
        recipient->displayUserInfo();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
