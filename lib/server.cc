#include "server.h"
#include "moduls.h"

void Server::Register(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  userinfo::UserRegister &&user_register) {
    auto client_ptr = drogon::app().getDbClient();
    try {
        auto result = client_ptr->execSqlSync("SELECT COUNT(*) FROM users WHERE email=$1",
            user_register.email);
        int count = result[0]["count"].as<int>();
        if (count > 0) {
            Json::Value ret;
            ret["message"] = "Account with such email is already exists";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k409Conflict);
            callback(resp);
            return;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
        return;
    }
    std::string token = GenerateToken();
    try {
        client_ptr->execSqlSync("INSERT INTO users (username, email, password, token) "
                                "VALUES ($1, $2, $3, $4)",
                                user_register.name,
                                user_register.email,
                                HashPassword(user_register.password),
                                token);
        Json::Value ret;
        ret["token"] = token;
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
        return;
    }
}

void Server::Login(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  userinfo::UserLogin &&user_login) {
    auto client_ptr = drogon::app().getDbClient();
    try {
        auto result = client_ptr->execSqlSync("SELECT token FROM users WHERE "
                                              "email=$1 AND password=$2",
                                              user_login.email,
                                              HashPassword(user_login.password));
        if (!result.empty()) {
            Json::Value ret;
            ret["token"] = result[0]["token"].as<std::string>();
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k200OK);
            callback(resp);
        } else {
            Json::Value ret;
            ret["message"] = "There aren't account with such email and password";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k409Conflict);
            callback(resp);
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
    }
}

void Server::CreateItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  userinfo::TodoItem &&todo_item) {
    auto client_ptr = drogon::app().getDbClient();
    std::string token = req->getHeader("Authorization");
    if (!CheckAuth(callback, token)) return;

    try {
        auto result = client_ptr->execSqlSync("INSERT INTO todos (token, title, description) "
                                              "VALUES ($1, $2, $3) "
                                              "RETURNING id",
                                              token,
                                              todo_item.title,
                                              todo_item.description);
        if (!result.empty()) {
            Json::Value ret;
            ret["id"] = result[0]["id"].as<int>();
            ret["title"] = todo_item.title;
            ret["description"] = todo_item.description;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k200OK);
            callback(resp);
            return;
        } else {
            std::cerr << "error: " << "Something went wrond in database" << std::endl;
            Json::Value ret;
            ret["message"] = "Something went wrond in database";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(drogon::HttpStatusCode::k409Conflict);
            callback(resp);
            return;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
        return;
    }
}

void Server::UpdateItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int item_id,
                  userinfo::TodoItem &&todo_item) {
    auto client_ptr = drogon::app().getDbClient();
    try {
        auto result = client_ptr->execSqlSync("UPDATE todos SET title=$1, description=$2 "
                                              "WHERE id=$3 AND token=$4 "
                                              "RETURNING id",
                                              todo_item.title,
                                              todo_item.description,
                                              item_id,
                                              req->getHeader("Authorization"));
        if (!result.empty()) {
            Json::Value ret;
            ret["id"] = result[0]["id"].as<int>();
            ret["title"] = todo_item.title;
            ret["description"] = todo_item.description;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k200OK);
            callback(resp);
        } else {
            Json::Value ret;
            ret["message"] = "Forbidden";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k403Forbidden);
            callback(resp);
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
    }
}

void Server::DeleteItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int item_id) {
    auto client_ptr = drogon::app().getDbClient();
    try {
        auto result = client_ptr->execSqlSync("DELETE FROM todos WHERE id=$1 AND token=$2 "
                                              "RETURNING id",
                                              item_id,
                                              req->getHeader("Authorization"));
        if (!result.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(HttpStatusCode::k204NoContent);
            callback(resp);
        } else {
            Json::Value ret;
            ret["message"] = "Forbidden";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(HttpStatusCode::k403Forbidden);
            callback(resp);
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
    }
}

void Server::GetItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int page,
                  int limit) {
    auto client_ptr = drogon::app().getDbClient();
    std::string token = req->getHeader("Authorization");
    int offset = (page - 1) * limit;
    try {
        auto result = client_ptr->execSqlSync("SELECT * FROM todos WHERE token=$1 "
                                               "ORDER BY id "
                                               "LIMIT $2 OFFSET $3",
                                               token,
                                               std::to_string(limit),
                                               std::to_string(offset));
        Json::Value data;
        for (const auto &row : result) {
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["title"] = row["title"].as<std::string>();
            item["description"] = row["description"].as<std::string>();
            data.append(item);
        }
        Json::Value ret;
        ret["data"] = data;
        ret["page"] = page;
        ret["limit"] = limit;
        ret["total"] = result.size();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(HttpStatusCode::k200OK);
        callback(resp);
    } catch (const drogon::orm::DrogonDbException &e) {
        HandleDatabaseError(e, callback);
    }
}