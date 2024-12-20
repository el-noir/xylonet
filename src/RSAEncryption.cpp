#include "RSAEncryption.h"
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

std::string RSAEncryption::encrypt(const std::string& data, const std::string& publicKey) {
    RSA* rsaKey = createRSAKey(publicKey, false);
    if (!rsaKey) {
        // Handle error: Failed to create RSA key
        return "";
    }
    return rsaEncrypt(data, rsaKey);
}

std::string RSAEncryption::decrypt(const std::string& encryptedData, const std::string& privateKey) {
    RSA* rsaKey = createRSAKey(privateKey, true);
    if (!rsaKey) {
        // Handle error: Failed to create RSA key
        return "";
    }
    return rsaDecrypt(encryptedData, rsaKey);
}

bool RSAEncryption::generateKeyPair(std::string& privateKey, std::string& publicKey) {
    RSA* rsa = RSA_new();
    if (!rsa) {
        return false;
    }

    BIGNUM* exponent = BN_new();
    BN_set_word(exponent, RSA_F4);

    int ret = RSA_generate_key_ex(rsa, 2048, exponent, nullptr);
    BN_free(exponent);

    if (ret != 1) {
        RSA_free(rsa);
        return false;
    }

    BIO* pub = BIO_new(BIO_s_mem());
    BIO* priv = BIO_new(BIO_s_mem());

    if (!PEM_write_bio_RSAPublicKey(pub, rsa) || !PEM_write_bio_RSAPrivateKey(priv, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
        RSA_free(rsa);
        BIO_free_all(pub);
        BIO_free_all(priv);
        return false;
    }

    size_t pub_len = BIO_pending(pub);
    size_t priv_len = BIO_pending(priv);

    char* pub_key = (char*)malloc(pub_len + 1);
    char* priv_key = (char*)malloc(priv_len + 1);

    BIO_read(pub, pub_key, pub_len);
    BIO_read(priv, priv_key, priv_len);

    pub_key[pub_len] = '\0';
    priv_key[priv_len] = '\0';

    publicKey = std::string(pub_key);
    privateKey = std::string(priv_key);

    free(pub_key);
    free(priv_key);

    RSA_free(rsa);
    BIO_free_all(pub);
    BIO_free_all(priv);

    return true;
}

std::string RSAEncryption::sign(const std::string& data, const std::string& privateKey) {
    RSA* rsaKey = createRSAKey(privateKey, true);
    if (!rsaKey) {
        return "";
    }
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, data.c_str(), data.size());
    SHA256_Final(hash, &sha256_ctx);

    unsigned char* signature = (unsigned char*)malloc(RSA_size(rsaKey));
    if (!signature) {
        RSA_free(rsaKey);
        return "";
    }

    unsigned int signature_len;
    if (RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, signature, &signature_len, rsaKey) != 1) {
        free(signature);
        RSA_free(rsaKey);
        return "";
    }

    std::string signatureStr(reinterpret_cast<char*>(signature), signature_len);
    free(signature);
    RSA_free(rsaKey);
    return signatureStr;
}

bool RSAEncryption::verify(const std::string& data, const std::string& signature, const std::string& publicKey) {
    RSA* rsaKey = createRSAKey(publicKey, false);
    if (!rsaKey) {
        return false;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, data.c_str(), data.size());
    SHA256_Final(hash, &sha256_ctx);

    int result = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, (unsigned char*)signature.c_str(), signature.size(), rsaKey);
    RSA_free(rsaKey);

    return result == 1;
}

RSA* RSAEncryption::createRSAKey(const std::string& key, bool isPrivate) {
    BIO* bio = BIO_new_mem_buf(key.c_str(), -1);
    if (!bio) {
        return nullptr;
    }
    RSA* rsa = nullptr;

    if (isPrivate) {
        rsa = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
    } else {
        rsa = PEM_read_bio_RSAPublicKey(bio, nullptr, nullptr, nullptr);
    }

    BIO_free(bio);
    return rsa;
}

std::string RSAEncryption::rsaEncrypt(const std::string& data, RSA* rsaKey) {
    size_t rsa_len = RSA_size(rsaKey);
    unsigned char* encrypted = (unsigned char*)malloc(rsa_len);

    int len = RSA_public_encrypt(data.size(), (unsigned char*)data.c_str(), encrypted, rsaKey, RSA_PKCS1_OAEP_PADDING);
    if (len == -1) {
        free(encrypted);
        return "";
    }

    std::string encryptedStr(reinterpret_cast<char*>(encrypted), len);
    free(encrypted);
    return encryptedStr;
}

std::string RSAEncryption::rsaDecrypt(const std::string& encryptedData, RSA* rsaKey) {
    size_t rsa_len = RSA_size(rsaKey);
    unsigned char* decrypted = (unsigned char*)malloc(rsa_len);

    int len = RSA_private_decrypt(encryptedData.size(), (unsigned char*)encryptedData.c_str(), decrypted, rsaKey, RSA_PKCS1_OAEP_PADDING);
    if (len == -1) {
        free(decrypted);
        return "";
    }

    std::string decryptedStr(reinterpret_cast<char*>(decrypted), len);
    free(decrypted);
    return decryptedStr;
}

std::string RSAEncryption::getPublicKeyFromPrivate(const std::string& privateKey) {
    RSA* rsaKey = createRSAKey(privateKey, true);
    if (!rsaKey) {
        return "";
    }

    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(bio, rsaKey);

    size_t pub_len = BIO_pending(bio);
    char* pub_key = (char*)malloc(pub_len + 1);
    BIO_read(bio, pub_key, pub_len);
    pub_key[pub_len] = '\0';

    std::string publicKey(pub_key);
    free(pub_key);
    RSA_free(rsaKey);
    BIO_free(bio);

    return publicKey;
}
