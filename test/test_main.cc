#define DROGON_TEST_MAIN
#include <drogon/drogon_test.h>
#include <drogon/drogon.h>

using namespace drogon;

DROGON_TEST(RegisterTest) {
    auto client = HttpClient::newHttpClient("http://localhost:5555");
    Json::Value params;
    params["name"] = "test";
    params["email"] = "test@test.com";
    params["password"] = "password";
    auto req = HttpRequest::newHttpJsonRequest(params);
    req->setPath("/register");
    req->setMethod(Post);
    client->sendRequest(req, [TEST_CTX](ReqResult res, const HttpResponsePtr& resp) {
        REQUIRE(res == ReqResult::Ok);
        REQUIRE(resp != nullptr);

        CHECK(resp->getStatusCode() == k200OK);
        CHECK(resp->contentType() == CT_APPLICATION_JSON);
    });
}

int main(int argc, char** argv) 
{
    using namespace drogon;

    std::promise<void> p1;
    std::future<void> f1 = p1.get_future();

    // Start the main loop on another thread
    std::thread thr([&]() {
        // Queues the promise to be fulfilled after starting the loop
        app().getLoop()->queueInLoop([&p1]() { p1.set_value(); });
        app().run();
    });

    // The future is only satisfied after the event loop started
    f1.get();
    int status = test::run(argc, argv);

    // Ask the event loop to shutdown and wait
    app().getLoop()->queueInLoop([]() { app().quit(); });
    thr.join();
    return status;
}
