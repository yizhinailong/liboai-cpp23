#include "liboai/liboai.h"
#include <cstdlib>

int main() {
    liboai::OpenAI oai(std::getenv("api_url"));

    // create a conversation
    liboai::Conversation convo;

    // holds next user input
    std::string input;

    if (oai.auth.SetKeyEnv("api_key")) {
        try {
            while (true) {
                // get next user input
                std::cout << "You: ";
                std::getline(std::cin, input);

                // add user input to conversation
                auto _ = convo.AddUserData(input);

                // get response from OpenAI
                liboai::Response response = oai.ChatCompletion->create(std::getenv("api_model"), convo);

                // update our conversation with the response
                auto temp = convo.Update(response);

                // print the response
                std::cout << "Bot: " << convo.GetLastResponse() << std::endl;
            }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}
