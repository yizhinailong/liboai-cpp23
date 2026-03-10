import std;
import liboai;

using namespace liboai;

int main() {
    OpenAI oai;
    if (oai.auth.SetKeyEnv("OPENAI_API_KEY")) {
        auto response =
            oai.Completion->Create("gpt-3.5-turbo-instruct", "Say this is a test", std::nullopt, 7);
        if (response) {
            std::cout << response.value()["choices"][0]["text"].get<std::string>() << std::endl;
        } else {
            std::cout << response.error().message << std::endl;
        }
    }
}
