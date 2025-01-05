#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "DAG.h" // Assuming your DAG class code is in a separate file

using namespace std;

void displayMenu() {
    cout << "===================================\n";
    cout << "            DAG Menu               \n";
    cout << "===================================\n";
    cout << "1. Add Transaction\n";
    cout << "2. View All Transactions and DAG\n";
    cout << "3. Exit\n";
    cout << "===================================\n";
    cout << "Enter your choice: ";
}

void addTransaction(DAG& dag) {
    int id;
    string sender, receiver, signature;
    double amount, fee;

    cout << "Enter Transaction ID: ";
    cin >> id;

    cout << "Enter Sender Account: ";
    cin >> sender;

    cout << "Enter Receiver Account: ";
    cin >> receiver;

    cout << "Enter Amount: ";
    cin >> amount;

    cout << "Enter Signature: ";
    cin >> signature;

    // Generate a random timestamp
    time_t timestamp = time(nullptr);

    // Create the transaction node
    TransactionNode transaction(to_string(id), sender, receiver, amount, 0, timestamp, {}, false);  // Fee is passed later in the class

    // Add the transaction to the DAG
    if (dag.addTransaction(transaction)) {
        cout << "Transaction " << id << " added successfully with hash: " << transaction.hash << ".\n";
    }
    else {
        cout << "Failed to add transaction.\n";
    }
}


// Function to save the DAG to a file
// Function to save the DAG to a file with titles
void saveDAGToFile(const DAG& dag) {
    ofstream file("dag_transactions.txt");

    if (!file) {
        cerr << "Error opening file for writing.\n";
        return;
    }

    // Write title/headers to the file
    file << "Transaction ID | Sender Account | Receiver Account | Amount | Fee | Timestamp | Hash | Parent Hashes\n";
    file << "------------------------------------------------------------\n";

    // Serialize the transactions and DAG structure to the file
    for (const auto& pair : dag.getTransactions()) {
        const auto& t = pair.second;
        file << t.id << " | " << t.senderAcc << " | " << t.receiverAcc << " | "
            << t.amount << " | " << t.fee << " | " << t.timestamp << " | "
            << t.hash << " | ";

        // Save parent hashes
        for (const auto& parentHash : t.parentHashes) {
            file << parentHash << " ";
        }
        file << "\n";
    }

    cout << "DAG saved to file successfully.\n";
}

// Function to load the DAG from a file and skip the title row
void loadDAGFromFile(DAG& dag) {
    ifstream file("dag_transactions.txt");
    if (!file) {
        cerr << "Error opening file for reading.\n";
        return;
    }

    string line;

    // Skip the header/title line
    getline(file, line);

    // Skip the separator line
    getline(file, line);

    // Now parse the actual data
    while (getline(file, line)) {
        stringstream ss(line);
        int id;
        string sender, receiver, hash, parentHash;
        double amount, fee;
        time_t timestamp;
        vector<string> parentHashes;

        // Parsing the transaction data
        ss >> id >> sender >> receiver >> amount >> fee >> timestamp >> hash;

        while (ss >> parentHash) {
            parentHashes.push_back(parentHash);
        }

        cout << "Loaded Transaction ID: " << id << " with parents: ";
        for (const auto& p : parentHashes) {
            cout << p << " ";
        }
        cout << endl;

        TransactionNode transaction(to_string(id), sender, receiver, amount, fee, timestamp, parentHashes, false);
        dag.addTransaction(transaction);
    }
    cout << "DAG loaded from file successfully.\n";
}
int main() {
    DAG dag;

    // Load transactions from file
    loadDAGFromFile(dag);

    // Perform consensus with a threshold of 10
    double validationThreshold = 1.0;
    dag.performConsensus(validationThreshold);

    int choice;
    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            addTransaction(dag);
            saveDAGToFile(dag);
            dag.performConsensus(validationThreshold); // Re-run consensus after adding transactions
            break;
        case 2:
            dag.printDAG();
            break;
        case 3:
            saveDAGToFile(dag);
            cout << "Exiting the program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);

    return 0;
}
