#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Validator.h"
#include "Transaction.h"
#include "DAGGraph.h"
#include "HybridConsensus.h"

TEST_CASE("Validator Validates Transaction") {
    DAGGraph dag;
    HybridConsensus consensus(10.0, 75.0);
    Validator validator(dag, consensus);

    Transaction tx("Alice", "Bob", 50.0);
    
    REQUIRE(validator.isValidTransaction(tx) == true);
}

TEST_CASE("Validator Detects Double Spending") {
    DAGGraph dag;
    HybridConsensus consensus(10.0, 75.0);
    Validator validator(dag, consensus);

    Transaction tx1("Alice", "Bob", 50.0);
    Transaction tx2("Alice", "Bob", 50.0);

    dag.addTransaction(tx1);
    REQUIRE(validator.isValidTransaction(tx2) == false); // Double spending should be detected
}
