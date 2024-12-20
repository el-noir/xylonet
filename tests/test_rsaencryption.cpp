#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "RSAEncryption.h"

TEST_CASE("RSA Key Generation") {
    std::string privateKey, publicKey;
    bool result = RSAEncryption::generateKeyPair(privateKey, publicKey);
    REQUIRE(result == true);
    REQUIRE(privateKey != "");
    REQUIRE(publicKey != "");
}

TEST_CASE("RSA Encryption and Decryption") {
    std::string privateKey, publicKey;
    RSAEncryption::generateKeyPair(privateKey, publicKey);

    std::string data = "Hello, Blockchain!";
    std::string encryptedData = RSAEncryption::encrypt(data, publicKey);
    REQUIRE(encryptedData != data); // Ensure data is encrypted

    std::string decryptedData = RSAEncryption::decrypt(encryptedData, privateKey);
    REQUIRE(decryptedData == data); // Ensure decrypted data matches original
}

TEST_CASE("RSA Signing and Verification") {
    std::string privateKey, publicKey;
    RSAEncryption::generateKeyPair(privateKey, publicKey);

    std::string data = "Hello, Blockchain!";
    std::string signature = RSAEncryption::sign(data, privateKey);
    REQUIRE(signature != "");

    bool isValid = RSAEncryption::verify(data, signature, publicKey);
    REQUIRE(isValid == true);
}
