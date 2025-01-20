#pragma once

#include <drogon/HttpController.h>

namespace userinfo {

    struct UserRegister {
        std::string name;
        std::string email;
        std::string password;
    };

    struct UserLogin {
        std::string email;
        std::string password;
    };

    struct TodoItem {
        std::string title;
        std::string description;
    };
}

namespace drogon {
    template <>
    inline userinfo::UserRegister fromRequest(const HttpRequest &req) {
        const auto& json = req.getJsonObject();
        userinfo::UserRegister user_register;
        if (json) {
            user_register.name = (*json)["name"].asString();
            user_register.email = (*json)["email"].asString();
            user_register.password = (*json)["password"].asString();
        }
        return user_register;
    }

    template <>
    inline userinfo::UserLogin fromRequest(const HttpRequest &req) {
        const auto& json = req.getJsonObject();
        userinfo::UserLogin user_login;
        if (json) {
            user_login.email = (*json)["email"].asString();
            user_login.password = (*json)["password"].asString();
        }
        return user_login;
    }

    template <>
    inline userinfo::TodoItem fromRequest(const HttpRequest &req) {
        const auto& json = req.getJsonObject();
        userinfo::TodoItem todo_item;
        if (json) {
            todo_item.title = (*json)["title"].asString();
            todo_item.description = (*json)["description"].asString();
        }
        return todo_item;
    }
}