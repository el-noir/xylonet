#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <functional>  // For std::hash
#include "DAG.h"

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

string generateSimpleHash(const string& transactionDetails) {
    std::hash<string> hashFunction;
    size_t hashValue = hashFunction(transactionDetails);
    stringstream ss;
    ss << hex << hashValue;

    return ss.str();
}

string formatTimestamp(time_t timestamp) {
    struct tm timeinfo;
    char buffer[80];

    if (localtime_s(&timeinfo, &timestamp) != 0) {
        cerr << "Error: Invalid timestamp provided.\n";
        return "Invalid Timestamp";
    }

    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo) == 0) {
        cerr << "Error: Failed to format timestamp.\n";
        return "Formatting Error";
    }

    return string(buffer);
}


bool isValidTransactionId(int id) {
    return id > 0;
}

bool isValidAmount(double amount) {
    return amount > 0.0;
}

bool isValidAccountName(const string& name) {
    // Assuming account names should be alphanumeric and between 3 to 20 characters
    return !name.empty() && name.length() >= 3 && name.length() <= 20 && all_of(name.begin(), name.end(), ::isalnum);
}

void addTransaction(DAG& dag) {
    int id;
    string sender, receiver;
    double amount;

    do {
        cout << "Enter Transaction ID (positive integer): ";
        cin >> id;
        if (!isValidTransactionId(id)) {
            cout << "Invalid Transaction ID. Please enter a positive integer.\n";
        }
    } while (!isValidTransactionId(id));

    do {
        cout << "Enter Sender Account (alphanumeric, 3-20 characters): ";
        cin >> sender;
        if (!isValidAccountName(sender)) {
            cout << "Invalid account name. It should be alphanumeric and between 3 to 20 characters.\n";
        }
    } while (!isValidAccountName(sender));

    do {
        cout << "Enter Receiver Account (alphanumeric, 3-20 characters): ";
        cin >> receiver;
        if (!isValidAccountName(receiver)) {
            cout << "Invalid account name. It should be alphanumeric and between 3 to 20 characters.\n";
        }
    } while (!isValidAccountName(receiver));


    do {
        cout << "Enter Amount (greater than 0): ";
        cin >> amount;
        if (!isValidAmount(amount)) {
            cout << "Invalid amount. Please enter a positive amount.\n";
        }
    } while (!isValidAmount(amount));

    time_t timestamp = time(nullptr);

    string transactionDetails = to_string(id) + sender + receiver + to_string(amount) + to_string(timestamp);

    string signature = generateSimpleHash(transactionDetails);

    TransactionNode transaction(to_string(id), sender, receiver, amount, 0, timestamp, {}, false); 
    transaction.hash = signature; 

    if (dag.addTransaction(transaction)) {
        cout << "Transaction " << id << " added successfully with hash: " << transaction.hash << ".\n";
    }
    else {
        cout << "Failed to add transaction.\n";
    }
}

void saveDAGToFile(const DAG& dag) {
    ofstream file("dag_transactions.txt");

    if (!file) {
        cerr << "Error opening file for writing.\n";
        return;
    }

    file << "Transaction ID | Sender Account | Receiver Account | Amount | Fee | Timestamp | Hash | Parent Hashes\n";
    file << "------------------------------------------------------------\n";

    // Serialize the transactions and DAG structure to the file
    for (const auto& pair : dag.getTransactions()) {
        const auto& t = pair.second;

        // Format the timestamp into a readable string
        string formattedTimestamp = formatTimestamp(t.timestamp);

        file << t.id << " | " << t.senderAcc << " | " << t.receiverAcc << " | "
            << t.amount << " | " << t.fee << " | " << formattedTimestamp << " | "
            << t.hash << " | ";

        // Save parent hashes
        for (const auto& parentHash : t.parentHashes) {
            file << parentHash << " ";
        }
        file << "\n";
    }

    cout << "DAG saved to file successfully.\n";
}

void loadDAGFromFile(DAG& dag) {
    ifstream file("dag_transactions.txt");
    if (!file) {
        cerr << "Error opening file for reading.\n";
        return;
    }

    string line;

    getline(file, line);

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        int id;
        string sender, receiver, hash, parentHash;
        double amount, fee;
        time_t timestamp;
        vector<string> parentHashes;

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

    loadDAGFromFile(dag);

    double validationThreshold = 1.0;

    int choice;
    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            addTransaction(dag);
            saveDAGToFile(dag);
            dag.performConsensus(validationThreshold); 
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





//#include <iostream>
//#include <string>
//#include <vector>
//#include <ctime>
//#include <cstdlib>
//#include <fstream>
//#include <sstream>
//#include <unordered_set>
//#include "DAG.h" // Assuming your DAG class code is in a separate file
//
//using namespace std;
//
//// Global variable to keep track of the last transaction ID
//int lastTransactionId = 0;
//
//// Helper functions for validation
//bool isValidAccount(const string& account) {
//    return account.size() >= 3 && account.substr(0, 2) == "ab" && stoi(account.substr(2)) >= 1 && stoi(account.substr(2)) <= 1000;
//}
//
//string getValidatedAccount(const string& prompt) {
//    string account;
//    while (true) {
//        cout << prompt;
//        cin >> account;
//        try {
//            if (isValidAccount(account)) {
//                return account;
//            }
//            else {
//                cout << "Invalid account. Please enter a valid account (ab1 to ab1000).\n";
//            }
//        }
//        catch (...) {
//            cout << "Invalid input. Please enter a valid account format (e.g., ab123).\n";
//        }
//    }
//}
//
//double getValidatedAmount() {
//    double amount;
//    while (true) {
//        cout << "Enter the amount to send (minimum: 500): ";
//        cin >> amount;
//        if (cin.fail() || amount < 500) {
//            cin.clear();
//            cin.ignore(numeric_limits<streamsize>::max(), '\n');
//            cout << "Invalid amount. Please enter a value greater than or equal to 500.\n";
//        }
//        else {
//            return amount;
//        }
//    }
//}
//
//// Function to generate a unique transaction ID
//int generateTransactionID() {
//    return ++lastTransactionId; // Increment and return
//}
//
//void displayMenu() {
//    cout << "\n===================================\n";
//    cout << "        DAG Transaction Menu       \n";
//    cout << "===================================\n";
//    cout << "      Welcome to the DAG-based system. Please select an option:\n";
//    cout << "===================================    \n";
//    cout << "1. Add a new transaction (create a transaction between two accounts).\n";
//    cout << "2. View all transactions and the DAG structure.\n";
//    cout << "3. Exit the program.\n";
//    cout << "===================================\n";
//    cout << "Enter your choice (1-3): ";
//}
//
//void addTransaction(DAG& dag) {
//    int id = generateTransactionID(); // Automatically generate transaction ID
//    cout << "Generated Transaction ID: " << id << endl;
//
//    string sender = getValidatedAccount("Enter Sender Account (ab1 to ab1000): ");
//    string receiver;
//    do {
//        receiver = getValidatedAccount("Enter Receiver Account (ab1 to ab1000): ");
//        if (receiver == sender) {
//            cout << "Sender and Receiver cannot be the same.\n";
//        }
//    } while (receiver == sender);
//
//    double amount = getValidatedAmount();
//
//    string signature;
//    cout << "Enter Signature: ";
//    cin >> signature;
//
//    // Generate a random timestamp
//    time_t timestamp = time(nullptr);
//
//    // Create the transaction node
//    TransactionNode transaction(to_string(id), sender, receiver, amount, 0, timestamp, {}, false);
//
//    // Add the transaction to the DAG
//    if (dag.addTransaction(transaction)) {
//        cout << "Transaction " << id << " added successfully with hash: " << transaction.hash << ".\n";
//    }
//    else {
//        cout << "Failed to add transaction.\n";
//    }
//}
//
//// Function to save the DAG to a file
//void saveDAGToFile(const DAG& dag) {
//    ofstream file("dag_transactions.txt");
//
//    if (!file) {
//        cerr << "Error opening file for writing.\n";
//        return;
//    }
//
//    // Save the last transaction ID
//    file << "LastTransactionID: " << lastTransactionId << "\n";
//
//    file << "Transaction ID | Sender Account | Receiver Account | Amount | Fee | Timestamp | Hash | Parent Hashes\n";
//    file << "------------------------------------------------------------\n";
//
//    for (const auto& pair : dag.getTransactions()) {
//        const auto& t = pair.second;
//        file << t.id << " | " << t.senderAcc << " | " << t.receiverAcc << " | "
//            << t.amount << " | " << t.fee << " | " << t.timestamp << " | "
//            << t.hash << " | ";
//
//        for (const auto& parentHash : t.parentHashes) {
//            file << parentHash << " ";
//        }
//        file << "\n";
//    }
//
//    cout << "DAG saved to file successfully.\n";
//}
//
//void loadDAGFromFile(DAG& dag) {
//    ifstream file("dag_transactions.txt");
//    if (!file) {
//        cerr << "Error opening file for reading.\n";
//        return;
//    }
//
//    string line;
//    getline(file, line);
//
//    // Parse the last transaction ID
//    if (line.find("LastTransactionID:") == 0) {
//        stringstream ss(line.substr(line.find(":") + 1));
//        ss >> lastTransactionId;
//    }
//
//    getline(file, line); // Skip separator
//
//    std::unordered_set<std::string> loadedTransactions;
//
//    while (getline(file, line)) {
//        stringstream ss(line);
//        int id;
//        string sender, receiver, hash, parentHash;
//        double amount, fee;
//        time_t timestamp;
//        vector<string> parentHashes;
//
//        ss >> id >> sender >> receiver >> amount >> fee >> timestamp >> hash;
//
//        if (loadedTransactions.find(hash) != loadedTransactions.end()) {
//            continue;
//        }
//
//        while (ss >> parentHash) {
//            parentHashes.push_back(parentHash);
//        }
//
//        TransactionNode transaction(to_string(id), sender, receiver, amount, fee, timestamp, parentHashes, false);
//        dag.addTransaction(transaction);
//        loadedTransactions.insert(hash);
//    }
//    cout << "DAG loaded from file successfully.\n";
//}
//
//
//// Main Function
//int main() {
//    DAG dag;
//    
//    loadDAGFromFile(dag);
//    int choice;
//    do {
//        displayMenu();
//        cin >> choice;
//
//        // Enhanced validation to handle invalid input
//        if (cin.fail()) {
//            cin.clear(); // Clear the error state
//            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
//            cout << "Invalid input. Please enter a valid option (1-3).\n";
//            continue; // Prompt the menu again
//        }
//
//        switch (choice) {
//        case 1:
//            addTransaction(dag);
//            saveDAGToFile(dag);
//            dag.performConsensus(1.0); // Re-run consensus after adding transactions
//            break;
//        case 2:
//            loadDAGFromFile(dag);
//            dag.printDAG();
//            break;
//        case 3:
//            saveDAGToFile(dag);
//            cout << "Exiting the program. Goodbye!\n";
//            break;
//        default:
//            cout << "Invalid choice. Please try again.\n";
//        }
//    } while (choice != 3);
//
//    return 0;
//}