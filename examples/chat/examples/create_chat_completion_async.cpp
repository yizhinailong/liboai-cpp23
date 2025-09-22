#include "liboai/liboai.h"
#include <cstdlib>

int main() {
    liboai::OpenAI oai(std::getenv("api_url"));

    // create a conversation
    liboai::Conversation convo;

    // add a message to the conversation
    auto _ = convo.AddUserData("What is the point of taxes?");

    if (oai.auth.SetKeyEnv("api_key")) {
        try {
            auto fut = oai.ChatCompletion->create_async(std::getenv("api_model"), convo);

            // do other work...

            // check if the future is ready
            fut.wait();

            // get the contained response
            auto response = fut.get();

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
