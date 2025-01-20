#pragma once
#include <string>
#include <drogon/HttpController.h>

using namespace drogon;

std::string GenerateToken();
std::string HashPassword(const std::string& password);
void HandleDatabaseError(const orm::DrogonDbException &e,
    const std::function<void(const HttpResponsePtr &)> &callback);
bool CheckAuth(const std::function<void(const HttpResponsePtr &)> &callback, std::string token);