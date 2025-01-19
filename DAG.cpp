
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
#include <stdexcept>

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
    Stack<std::string>& stack) {
    if (!stack.isEmpty() && stack.top() == node) {
        return true; // Cycle detected
    }
    if (visited.find(node) != visited.end()) {
        return false; // Already processed
    }

    visited.insert(node);
    stack.push(node);

    for (const std::string& neighbor : adjList[node]) {
        if (hasCycle(neighbor, visited, stack)) {
            return true;
        }
    }


    stack.pop(); // Backtrack
    return false;
}

int DAG::updateCumulativeWeights(const std::string& hash) {
    if (cumulativeWeights.find(hash) != cumulativeWeights.end()) {
        return cumulativeWeights[hash]; 
    }

    cumulativeWeights[hash] = 1; // Base weight
    for (const auto& parent : adjList[hash]) {
        cumulativeWeights[hash] += updateCumulativeWeights(parent); // Accumulate parent's weight
    }

    return cumulativeWeights[hash];
}


// Select parents for a transaction using MCMC method
//std::vector<std::string> DAG::selectParentsMCMC(size_t numParents) {
//    std::vector<TipInfo> tips;
//
//    // Collect tips (transactions at the end of the graph)
//    for (const auto& pair : transactions) {
//        if (adjList[pair.first].empty()) {
//            tips.push_back(TipInfo{
//                pair.first,
//                cumulativeWeights[pair.first],
//                pair.second.timestamp,
//                static_cast<int>(pair.second.amount),
//                static_cast<int>(pair.second.fee)
//                });
//        }
//    }
//
//    std::cout << "Tips found: " << tips.size() << std::endl;
//
//    if (tips.empty()) {
//        std::cout << "No tips available for parent selection.\n";
//        return {};
//    }
//
//    // Ensure there are enough tips to select from
//    if (tips.size() < numParents) {
//        std::cerr << "Warning: Not enough tips available. Requested " << numParents
//            << " but only " << tips.size() << " available.\n";
//        numParents = tips.size();  // Adjust to select as many as available
//    }
//
//    // Calculate probabilities based on multiple factors (weight, fee, amount)
//    std::vector<double> probabilities;
//    double totalWeight = 0.0;
//
//    for (const auto& tip : tips) {
//        double timeDecay = exp(-static_cast<double>(difftime(time(nullptr), tip.timestamp)) / 3600.0);
//        double feeFactor = 1 + log(1 + tip.fee);
//        double amountFactor = log(1 + tip.amount);
//
//        // Combine factors to get weighted probability
//        double weightedProbability = tip.cumulativeWeight * timeDecay * feeFactor * amountFactor;
//
//        probabilities.push_back(weightedProbability);
//        totalWeight += weightedProbability;
//    }
//
//    if (totalWeight == 0.0) {
//        probabilities.assign(probabilities.size(), 1.0);
//        totalWeight = probabilities.size();
//    }
//
//    // Normalize probabilities
//    for (auto& prob : probabilities) {
//        prob /= totalWeight;
//    }
//
//    // Select multiple parents using MCMC with calculated probabilities
//    std::vector<std::string> selectedParents;
//    std::default_random_engine generator(std::random_device{}());
//    std::discrete_distribution<size_t> distribution(probabilities.begin(), probabilities.end());
//
//    std::unordered_set<std::string> selectedSet;
//
//    // Select unique parents to ensure diversity
//    while (selectedParents.size() < numParents) {
//        size_t index = distribution(generator);
//        if (selectedSet.insert(tips[index].hash).second) {
//            selectedParents.push_back(tips[index].hash);
//        }
//
//        if (selectedParents.size() == numParents) {
//            break;
//        }
//    }
//
//    if (selectedParents.size() < numParents) {
//        std::cerr << "Warning: Could not select enough unique parents. "
//            << "Selected " << selectedParents.size() << " out of " << numParents << ".\n";
//    }
//
//    return selectedParents;
//}


std::vector<std::string> DAG::selectParentsMCMC(size_t numParents) {
    std::vector<std::string> tips;

    for (const auto& pair : transactions) {
        if (adjList[pair.first].empty()) {
            tips.push_back(pair.first);
        }
    }

    std::cout << "Tips found: " << tips.size() << std::endl;

    if (tips.empty()) {
        std::cout << "No tips available for parent selection.\n";
        return {};
    }

    if (tips.size() < numParents) {
        std::cout << "Warning: Not enough tips available. Requested " << numParents
            << " but only " << tips.size() << " available.\n";
        numParents = tips.size();  
    }

    std::unordered_set<std::string> selectedSet;
    std::vector<std::string> selectedParents;

    while (selectedParents.size() < numParents) {
        size_t index = rand() % tips.size();
        if (selectedSet.insert(tips[index]).second) {
            selectedParents.push_back(tips[index]);
        }

        if (selectedParents.size() == numParents) {
            break;
        }
    }

    if (selectedParents.size() < numParents) {
        std::cerr << "Warning: Could not select enough unique parents. "
            << "Selected " << selectedParents.size() << " out of " << numParents << ".\n";
    }

    return selectedParents;
}

double DAG::calculateFee(double amount) {
    double fee;
    if (amount < 1000) {
        fee = amount * 0.01;
    }
    else {
        fee = 50.0; 
    }
    return fee;
}
bool DAG::addTransaction(TransactionNode& transaction) {
    double fee = calculateFee(transaction.amount); 
    transaction.fee = fee;  
    std::cout << "Calculated Fee: " << fee << std::endl;

    std::vector<std::string> parentHashes = selectParentsMCMC(3);  // Select 3 parents

    transaction.parentHashes = parentHashes;
    transactions[transaction.hash] = transaction;

    std::unordered_set<std::string> visited;
    Stack<std::string> stack;

    if (hasCycle(transaction.hash, visited, stack)) {
        std::cout << "Cannot add transaction " << transaction.id << " as it creates a cycle.\n";
        return false;
    }

    return true;
}

void DAG::performConsensus(double validationThreshold) {
    std::unordered_set<std::string> visited;

    for (const auto& pair : transactions) {
        const auto& hash = pair.first;
        if (visited.find(hash) == visited.end()) {
            validateTransaction(hash, validationThreshold, visited);
        }
    }
}

bool DAG::validateTransaction(const std::string& hash, double validationThreshold, std::unordered_set<std::string>& visited) {
    if (visited.find(hash) != visited.end()) {
        return transactions[hash].isValidated;
    }

    visited.insert(hash);

    int cumulativeWeight = updateCumulativeWeights(hash);
    std::cout << "Validating transaction " << hash << " with cumulative weight: " << cumulativeWeight << " against threshold " << validationThreshold << std::endl;

    if (cumulativeWeight >= validationThreshold) {
        transactions[hash].isValidated = true;
        std::cout << "Transaction " << hash << " validated!" << std::endl;
        return true;
    }

    for (const auto& parent : transactions[hash].parentHashes) {
        if (!validateTransaction(parent, validationThreshold, visited)) {
            std::cout << "Parent " << parent << " validation failed!" << std::endl;
            return false;
        }
    }

    return transactions[hash].isValidated;
}



//bool DAG::validateTransaction(const std::string& hash, double validationThreshold, std::unordered_set<std::string>& visited) {
//    if (visited.find(hash) != visited.end()) {
//        return transactions[hash].isValidated; // Already validated
//    }
//
//    visited.insert(hash);
//
//    // Recalculate cumulative weight
//    int cumulativeWeight = updateCumulativeWeights(hash);
//    std::cout << "Validating transaction " << hash << " with cumulative weight: " << cumulativeWeight << " against threshold " << validationThreshold << std::endl;
//
//    // Mark as validated if the cumulative weight meets the threshold
//    if (cumulativeWeight >= validationThreshold) {
//        transactions[hash].isValidated = true;
//        std::cout << "Transaction " << hash << " validated!" << std::endl;
//        return true;
//    }
//
//    // Validate parents recursively
//    for (const auto& parent : transactions[hash].parentHashes) {
//        if (!validateTransaction(parent, validationThreshold, visited)) {
//            std::cout << "Parent " << parent << " validation failed!" << std::endl;
//            return false; // If any parent is invalid, this transaction is invalid
//        }
//    }
//
//    return transactions[hash].isValidated;
//}

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

    // Dynamically create the adjacency list
    std::unordered_map<std::string, std::vector<std::string>> dynamicAdjList;

    for (const auto& pair : transactions) {
        const auto& t = pair.second;
        for (const auto& parentHash : t.parentHashes) {
            dynamicAdjList[parentHash].push_back(t.hash); // Add this transaction as a child of the parent
        }
    }

    std::cout << "\nGraph Adjacency List:\n";
    for (const auto& pair : dynamicAdjList) {
        std::cout << pair.first << " -> ";
        for (const auto& neighbor : pair.second) {
            std::cout << neighbor << ", ";
        }
        std::cout << std::endl;
    }
}