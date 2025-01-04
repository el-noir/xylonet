//#include "DAG.h"
//#include <iostream>
//#include <cmath>
//#include <ctime>
//#include <functional>
//#include <random>
//#include <sstream>
//#include <unordered_set>
//#include <unordered_map>
//#include <fstream>
//#include <vector>
//#include "TransactionNode.h"
//#include "HashUtils.h"
//
//// Check for cycles in the DAG (Depth-First Search approach)
//bool DAG::hasCycle(const std::string& node,
//    std::unordered_set<std::string>& visited,
//    std::unordered_set<std::string>& stack) {
//    if (stack.find(node) != stack.end()) {
//        return true;  // Cycle detected
//    }
//    if (visited.find(node) != visited.end()) {
//        return false; // Node already visited
//    }
//
//    visited.insert(node);
//    stack.insert(node);
//
//    for (const auto& neighbor : adjList[node]) {
//        if (hasCycle(neighbor, visited, stack)) {
//            return true;
//        }
//    }
//
//    stack.erase(node);
//    return false;
//}
//
//// Update cumulative weights of a transaction recursively
//int DAG::updateCumulativeWeights(const std::string& hash) {
//    if (cumulativeWeights.find(hash) != cumulativeWeights.end()) {
//        return cumulativeWeights[hash];
//    }
//
//    cumulativeWeights[hash] = 1; // Base weight of 1
//
//    for (const auto& neighbor : adjList[hash]) {
//        cumulativeWeights[hash] += updateCumulativeWeights(neighbor);
//    }
//
//    return cumulativeWeights[hash];
//}
//
//// Select parents for a transaction using MCMC method
////std::vector<std::string> DAG::selectParentsMCMC(size_t numParents) {
////    std::vector<TipInfo> unapprovedTips;
////
////    // Identify unapproved tips (nodes without any outgoing edges)
////    for (const auto& pair : transactions) {
////        if (adjList.find(pair.first) == adjList.end() || adjList[pair.first].empty()) {
////            unapprovedTips.push_back({ pair.first, updateCumulativeWeights(pair.first), pair.second.timestamp });
////        }
////    }
////
////    if (unapprovedTips.empty()) {
////        std::cout << "No tips available for parent selection.\n";
////        return {};
////    }
////
////    double totalWeight = 0.0;
////    std::vector<double> probabilities;
////
////    for (const auto& tip : unapprovedTips) {
////        double weight = tip.cumulativeWeight * exp(-difftime(time(nullptr), tip.timestamp) / 3600.0);
////        if (weight > 0) {
////            totalWeight += weight;
////            probabilities.push_back(weight);
////        }
////        else {
////            probabilities.push_back(0); // Prevent zero/negative weights
////        }
////    }
////
////    if (totalWeight == 0.0) {
////        std::cerr << "Error: Total weight is zero. Cannot normalize probabilities.\n";
////        return {};
////    }
////
////    for (auto& prob : probabilities) {
////        prob /= totalWeight; // Normalize probabilities
////    }
////
////    std::vector<std::string> selectedParents;
////    std::default_random_engine generator(std::random_device{}());
////    std::discrete_distribution<size_t> distribution(probabilities.begin(), probabilities.end());
////
////    std::unordered_set<std::string> selectedSet;
////    while (selectedParents.size() < numParents && selectedSet.size() < unapprovedTips.size()) {
////        size_t index = distribution(generator);
////        if (selectedSet.insert(unapprovedTips[index].hash).second) {
////            selectedParents.push_back(unapprovedTips[index].hash);
////        }
////    }
////
////    return selectedParents;
////}
//std::vector<std::string> DAG::selectParentsMCMC(size_t numParents) {
//    std::vector<TipInfo> unapprovedTips;
//
//    // Collect unapproved tips (transactions with no children)
//    for (const auto& pair : transactions) {
//        if (adjList[pair.first].empty()) {
//            unapprovedTips.push_back(TipInfo{
//                pair.first,
//                cumulativeWeights[pair.first],
//                pair.second.timestamp,
//                static_cast<int>(pair.second.amount),  // Fix narrowing conversion
//                static_cast<int>(pair.second.fee)      // Fix narrowing conversion
//                });
//        }
//    }
//
//    if (unapprovedTips.empty()) {
//        std::cout << "No tips available for parent selection.\n";
//        return {}; // No parents can be selected
//    }
//
//    // Calculate weights based on cumulative weight, timestamp, amount, and fee
//    std::vector<double> probabilities;
//    double totalWeight = 0.0;
//
//    for (const auto& tip : unapprovedTips) {
//        // Calculate time decay (older transactions have less weight)
//        double timeDecay = exp(-static_cast<double>(difftime(time(nullptr), tip.timestamp)) / 3600.0);
//
//        // Calculate fee and amount factors (higher fees and amounts give more weight)
//        double feeFactor = 1 + log(1 + tip.fee); // Fee modifier
//        double amountFactor = log(1 + tip.amount); // Amount modifier
//
//        // Combine all factors: Cumulative weight * time decay * fee factor * amount factor
//        double weightedProbability = tip.cumulativeWeight * timeDecay * feeFactor * amountFactor;
//
//        probabilities.push_back(weightedProbability);
//        totalWeight += weightedProbability;
//    }
//
//    // Check for zero total weight and handle by giving equal probability to all tips
//    if (totalWeight == 0.0) {
//        probabilities.assign(probabilities.size(), 1.0);
//        totalWeight = probabilities.size(); // Total weight equals number of tips
//    }
//
//    // Normalize probabilities to make sure they sum up to 1
//    for (auto& prob : probabilities) {
//        prob /= totalWeight;
//    }
//
//    // Select multiple unique parents using MCMC with the calculated probabilities
//    std::vector<std::string> selectedParents;
//    std::default_random_engine generator(std::random_device{}());
//    std::discrete_distribution<size_t> distribution(probabilities.begin(), probabilities.end());
//
//    std::unordered_set<std::string> selectedSet;
//    while (selectedParents.size() < numParents && selectedSet.size() < unapprovedTips.size()) {
//        size_t index = distribution(generator); // Randomly select a tip
//
//        // Check if this tip has already been selected (ensuring uniqueness)
//        if (selectedSet.insert(unapprovedTips[index].hash).second) {
//            selectedParents.push_back(unapprovedTips[index].hash);
//        }
//
//        // If we have selected enough unique parents, break the loop
//        if (selectedParents.size() == numParents) {
//            break;
//        }
//    }
//
//    // If we couldn't find enough unique parents, log a warning
//    if (selectedParents.size() < numParents) {
//        std::cerr << "Warning: Could not select enough unique parents. "
//            << "Selected " << selectedParents.size() << " out of " << numParents << ".\n";
//    }
//
//    return selectedParents;
//}
//
//
//bool DAG::addTransaction(TransactionNode& transaction) {
//    // Input validation for the transaction fields
//    double fee;
//    std::cout << "Enter Transaction Fee: ";
//    while (!(std::cin >> fee) || fee < 0) {
//        std::cout << "Invalid fee. Please enter a positive number: ";
//        std::cin.clear();
//        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//    }
//
//    // Proceed to select parents
//    std::vector<std::string> parentHashes = selectParentsMCMC(3);
//
//    transaction.parentHashes = parentHashes;
//    transactions[transaction.hash] = transaction;
//
//    for (const auto& parentHash : parentHashes) {
//        adjList[parentHash].push_back(transaction.hash);
//    }
//
//    std::unordered_set<std::string> visited;
//    std::unordered_set<std::string> stack;
//
//    // Check for cycles after adding the transaction
//    for (const auto& pair : adjList) {
//        if (hasCycle(pair.first, visited, stack)) {
//            for (const auto& parentHash : parentHashes) {
//                adjList[parentHash].pop_back();  // Remove parent relation if cycle is detected
//            }
//            transactions.erase(transaction.hash);  // Remove the transaction
//            std::cout << "Cannot add transaction " << transaction.id << " as it creates a cycle.\n";
//            return false;
//        }
//    }
//
//    // Update cumulative weights of the parents
//    for (const auto& parentHash : parentHashes) {
//        updateCumulativeWeights(parentHash);
//    }
//
//    std::cout << "Transaction " << transaction.id << " added successfully with hash: " << transaction.hash << ".\n";
//    return true;
//}
//
//// Print the DAG details
//void DAG::printDAG() const {
//    std::cout << "All transactions:\n";
//    for (const auto& pair : transactions) {
//        const auto& t = pair.second;
//        std::cout << "ID: " << t.id << ", Sender: " << t.senderAcc
//            << ", Receiver: " << t.receiverAcc << ", Amount: "
//            << t.amount << ", Fee: " << t.fee << ", Timestamp: " << t.timestamp
//            << ", Hash: " << t.hash << ", Parents: ";
//        for (const auto& parentHash : t.parentHashes) {
//            std::cout << parentHash << " ";
//        }
//        std::cout << ", Validated: " << (t.isValidated ? "Yes" : "No") << "\n";
//    }
//
//    std::cout << "\nGraph Adjacency List:\n";
//    for (const auto& pair : adjList) {
//        std::cout << pair.first << " -> ";
//        for (const auto& neighbor : pair.second) {
//            std::cout << neighbor << ", ";
//        }
//        std::cout << std::endl;
//    }
//}
//
//

#include "DAG.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <functional>
#include <random>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <vector>
#include "TransactionNode.h"
#include "HashUtils.h"

// Function to save transactions to a file
void DAG::saveTransactionsToFile(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile) {
        std::cerr << "Error opening file '" << filename << "' for saving transactions.\n";
        return;
    }

    for (const auto& pair : transactions) {
        const auto& t = pair.second;
        outFile << t.id << "," << t.senderAcc << "," << t.receiverAcc << ","
            << t.amount << "," << t.fee << "," << t.timestamp << ","
            << t.hash;

        for (const auto& parent : t.parentHashes) {
            outFile << "," << parent;
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Transactions saved to file: " << filename << "\n";
}

// Function to load transactions from a file
bool DAG::loadTransactionsFromFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile) {
        std::cerr << "Error opening file '" << filename << "' for loading transactions.\n";
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        TransactionNode transaction;

        if (!(std::getline(ss, transaction.id, ',') &&
            std::getline(ss, transaction.senderAcc, ',') &&
            std::getline(ss, transaction.receiverAcc, ',') &&
            (ss >> transaction.amount) && ss.ignore(1, ',') &&
            (ss >> transaction.fee) && ss.ignore(1, ',') &&
            (ss >> transaction.timestamp) && ss.ignore(1, ',') &&
            std::getline(ss, transaction.hash, ','))) {
            std::cerr << "Error parsing transaction line: " << line << "\n";
            continue;
        }

        // Parse parent hashes
        std::string parentHash;
        while (std::getline(ss, parentHash, ',')) {
            transaction.parentHashes.push_back(parentHash);
        }

        transactions[transaction.hash] = transaction;
    }

    inFile.close();
    std::cout << "Transactions loaded from file: " << filename << "\n";
    return true;
}


// Check for cycles in the DAG (Depth-First Search approach)
bool DAG::hasCycle(const std::string& node,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& stack) {
    if (stack.find(node) != stack.end()) {
        return true;  // Cycle detected
    }
    if (visited.find(node) != visited.end()) {
        return false; // Node already visited
    }

    visited.insert(node);
    stack.insert(node);

    for (const auto& neighbor : adjList[node]) {
        if (hasCycle(neighbor, visited, stack)) {
            return true;
        }
    }

    stack.erase(node);
    return false;
}

// Update cumulative weights of a transaction recursively
int DAG::updateCumulativeWeights(const std::string& hash) {
    if (cumulativeWeights.find(hash) != cumulativeWeights.end()) {
        return cumulativeWeights[hash];
    }

    cumulativeWeights[hash] = 1; // Base weight of 1

    for (const auto& neighbor : adjList[hash]) {
        cumulativeWeights[hash] += updateCumulativeWeights(neighbor);
    }

    return cumulativeWeights[hash];
}
// Select parents for a transaction using MCMC method
std::vector<std::string> DAG::selectParentsMCMC(size_t numParents) {
    std::vector<TipInfo> unapprovedTips;

    // Collect unapproved tips
    for (const auto& pair : transactions) {
        if (adjList[pair.first].empty()) {
            unapprovedTips.push_back(TipInfo{
                pair.first,
                cumulativeWeights[pair.first],
                pair.second.timestamp,
                static_cast<int>(pair.second.amount),
                static_cast<int>(pair.second.fee)
                });
        }
    }

    std::cout << "Unapproved tips found: " << unapprovedTips.size() << std::endl;

    if (unapprovedTips.empty()) {
        std::cout << "No tips available for parent selection.\n";
        return {}; // No parents can be selected
    }
    // Check if there are enough unapproved tips
    if (unapprovedTips.size() < numParents) {
        std::cerr << "Warning: Not enough unapproved tips available. Requested "
            << numParents << " but only " << unapprovedTips.size() << " available.\n";
        numParents = unapprovedTips.size();  // Adjust to select as many as available
    }

    // Calculate weights based on cumulative weight, timestamp, amount, and fee
    std::vector<double> probabilities;
    double totalWeight = 0.0;

    for (const auto& tip : unapprovedTips) {
        // Calculate time decay (older transactions have less weight)
        double timeDecay = exp(-static_cast<double>(difftime(time(nullptr), tip.timestamp)) / 3600.0);

        // Calculate fee and amount factors (higher fees and amounts give more weight)
        double feeFactor = 1 + log(1 + tip.fee); // Fee modifier
        double amountFactor = log(1 + tip.amount); // Amount modifier

        // Combine all factors: Cumulative weight * time decay * fee factor * amount factor
        double weightedProbability = tip.cumulativeWeight * timeDecay * feeFactor * amountFactor;

        probabilities.push_back(weightedProbability);
        totalWeight += weightedProbability;
    }

    // If total weight is zero, all tips are equally probable
    if (totalWeight == 0.0) {
        probabilities.assign(probabilities.size(), 1.0);
        totalWeight = probabilities.size(); // Total weight equals number of tips
    }

    // Normalize probabilities to make sure they sum up to 1
    for (auto& prob : probabilities) {
        prob /= totalWeight;
    }

    // Select multiple unique parents using MCMC with the calculated probabilities
    std::vector<std::string> selectedParents;
    std::default_random_engine generator(std::random_device{}());
    std::discrete_distribution<size_t> distribution(probabilities.begin(), probabilities.end());

    std::unordered_set<std::string> selectedSet;
    while (selectedParents.size() < numParents) {
        size_t index = distribution(generator); // Randomly select a tip

        // If this tip has not been selected yet for the current transaction, add it
        if (selectedSet.insert(unapprovedTips[index].hash).second) {
            selectedParents.push_back(unapprovedTips[index].hash);
        }

        // If we've selected enough unique parents, break the loop
        if (selectedParents.size() == numParents) {
            break;
        }
    }

    // If we couldn't find enough unique parents, log a warning
    if (selectedParents.size() < numParents) {
        std::cerr << "Warning: Could not select enough unique parents. "
            << "Selected " << selectedParents.size() << " out of " << numParents << ".\n";
    }

    return selectedParents;
}

// Calculate the fee based on the transaction amount
double DAG::calculateFee(double amount) {
    double fee;
    if (amount < 1000) {
        fee = amount * 0.01; // 1% fee for amounts less than $1000
    }
    else {
        fee = 50.0; // Flat fee of $50 for amounts greater than or equal to $1000
    }
    return fee;
}

// Add a transaction to the DAG
bool DAG::addTransaction(TransactionNode& transaction) {
    // Calculate the fee based on the transaction amount
    double fee = calculateFee(transaction.amount);
    transaction.fee = fee;  // Set the calculated fee

    std::cout << "Calculated Fee: " << fee << std::endl;

    // Proceed to select parents
    std::vector<std::string> parentHashes = selectParentsMCMC(3);

    transaction.parentHashes = parentHashes;
    transactions[transaction.hash] = transaction;

    // Update the adjacency list: Parent -> Transaction
    for (const auto& parentHash : parentHashes) {
        adjList[parentHash].push_back(transaction.hash);  // Add this transaction as a child of the parent
    }

    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> stack;

    // Check for cycles after adding the transaction
    for (const auto& pair : adjList) {
        if (hasCycle(pair.first, visited, stack)) {
            // If cycle is detected, remove this transaction and undo changes
            for (const auto& parentHash : parentHashes) {
                adjList[parentHash].pop_back();  // Remove this transaction from the parent's child list
            }
            transactions.erase(transaction.hash);  // Remove the transaction
            std::cout << "Cannot add transaction " << transaction.id << " as it creates a cycle.\n";
            return false;
        }
    }

    // Update cumulative weights of the parents
    for (const auto& parentHash : parentHashes) {
        updateCumulativeWeights(parentHash);
    }

    return true;
}

// Print the DAG details
void DAG::printDAG() const {
    std::cout << "All transactions:\n";
    for (const auto& pair : transactions) {
        const auto& t = pair.second;
        std::cout << "ID: " << t.id << ", Sender: " << t.senderAcc
            << ", Receiver: " << t.receiverAcc << ", Amount: "
            << t.amount << ", Fee: " << t.fee << ", Timestamp: " << t.timestamp
            << ", Hash: " << t.hash << ", Parents: ";
        for (const auto& parentHash : t.parentHashes) {
            std::cout << parentHash << " ";
        }
        std::cout << ", Validated: " << (t.isValidated ? "Yes" : "No") << "\n";
    }

    std::cout << "\nGraph Adjacency List:\n";
    for (const auto& pair : adjList) {
        std::cout << pair.first << " -> ";
        for (const auto& neighbor : pair.second) {
            std::cout << neighbor << ", ";
        }
        std::cout << std::endl;
    }
}

