#include <iostream>
#include "Wallet.h"
#include "Transaction.h"
#include "DAGGraph.h"
#include "HybridConsensus.h"
#include "Validator.h"
#include "RSAEncryption.h"

int main() {
    std::cout << "Starting Blockchain System...\n";

    Wallet alice;
    Wallet bob;

    // Transaction between Alice and Bob
    bool success = alice.sendTransaction(bob, 50.0);
    if (success) {
        std::cout << "Transaction successful! Alice's balance: " << alice.getBalance() << ", Bob's balance: " << bob.getBalance() << "\n";
    } else {
        std::cout << "Transaction failed due to insufficient funds.\n";
    }

    return 0;
}
