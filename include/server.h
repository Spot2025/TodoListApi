#pragma once

#include <drogon/HttpController.h>
#include "structures.h"

using namespace drogon;

class Server : public drogon::HttpController<Server>
{

    public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(Server::Register, "/register", Post);
        ADD_METHOD_TO(Server::Login, "/login", Post);
        ADD_METHOD_TO(Server::CreateItem, "/todos", Post);
        ADD_METHOD_TO(Server::UpdateItem, "/todos/{1}", Put);
        ADD_METHOD_TO(Server::DeleteItem, "/todos/{1}", Delete);
        ADD_METHOD_TO(Server::GetItem, "/todos?page={page}&limit={limit}", Get);
    METHOD_LIST_END

    void Register(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  userinfo::UserRegister &&user_register);
    void Login(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  userinfo::UserLogin &&user_login);
    void CreateItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  userinfo::TodoItem &&todo_item);
    void UpdateItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int item_id,
                  userinfo::TodoItem &&todo_item);
    void DeleteItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int item_id);
    void GetItem(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int page,
                  int limit);
};
