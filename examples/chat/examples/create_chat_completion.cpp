#include "liboai/liboai.h"
#include <cstdlib>
#include <print>

int main() {

    std::println("OpenAI API C++ Example: Chat Completion");
    std::println("api_url: {}", std::getenv("api_url"));
    std::println("api_model: {}", std::getenv("api_model"));

    liboai::OpenAI oai(std::getenv("api_url"));

    // create a conversation
    liboai::Conversation convo;

    // add a message to the conversation
    auto _ = convo.AddUserData("What is the point of taxes?");

    if (oai.auth.SetKeyEnv("api_key")) {
        try {
            auto response = oai.ChatCompletion->create(std::getenv("api_model"), convo);

            // update our conversation with the response
            auto _ = convo.Update(response);

            // print the response
            std::cout << convo.GetLastResponse() << std::endl;
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}
