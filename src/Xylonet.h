#ifndef TRANSACTION_EXAMPLE_H
#define TRANSACTION_EXAMPLE_H

#include <iostream>
#include <string>
#include "DAG.h"
#include "Transaction.h"
#include "User.h"
#include "Wallet.h"
#include "Network.h"
#include "Xylonet.h"

// Function to simulate transaction creation for a logged-in user
void createTransactionExample(User& loggedInUser, Network& network);

#endif // TRANSACTION_EXAMPLE_H
