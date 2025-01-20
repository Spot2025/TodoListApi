#include <drogon/drogon.h>

void InitDatabase() {
    auto client_ptr = drogon::app().getDbClient();
    try {
        client_ptr->execSqlSync("CREATE TABLE IF NOT EXISTS users ("
                                             "username VARCHAR(80) NOT NULL,"
                                             "email VARCHAR(80) NOT NULL UNIQUE,"
                                             "password VARCHAR(80) NOT NULL,"
                                             "token VARCHAR(80) NOT NULL UNIQUE");
    } catch (const drogon::orm::DrogonDbException &e) {
        std::cerr << "error: " << e.base().what() << std::endl;
        exit(1);
    }
    try {
        client_ptr->execSqlSync("CREATE TABLE IF NOT EXISTS todos ("
                                "token VARCHAR(80) NOT NULL,"
                                "title VARCHAR(80),"
                                "description VARCHAR(160)");
    } catch (const drogon::orm::DrogonDbException &e) {
        std::cerr << "error: " << e.base().what() << std::endl;
        exit(1);
    }
}

int main() {
    drogon::app().loadConfigFile("../config.json");
    //InitDatabase();
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
