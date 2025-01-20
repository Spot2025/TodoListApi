#include <chrono>
#include <random>
#include <openssl/sha.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "moduls.h"

std::string GenerateToken() {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::mt19937_64 rnd(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::string token(40, '0');
    for (size_t i = 0; i < token.size(); i++) {
        token[i] = characters[rnd() % (token.size() - 1)];
    }
    return token;
}

std::string HashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

void HandleDatabaseError(const orm::DrogonDbException &e,
    const std::function<void(const drogon::HttpResponsePtr &)> &callback) {
    std::cerr << "error: " << e.base().what() << std::endl;
    Json::Value ret;
    ret["message"] = e.base().what();
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    resp->setStatusCode(drogon::HttpStatusCode::k409Conflict);
    callback(resp);
}

bool CheckAuth(const std::function<void(const HttpResponsePtr &)> &callback, std::string token) {
    auto client_ptr = drogon::app().getDbClient();
    try {
        auto result = client_ptr->execSqlSync("SELECT COUNT(*) FROM users WHERE token=$1",
            token);
        int count = result[0]["count"].as<int>();
        if (count == 0) {
            Json::Value ret;
            ret["message"] = "Unauthorized";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k401Unauthorized);
            callback(resp);
            return false;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
        return false;
    }
    return true;
}