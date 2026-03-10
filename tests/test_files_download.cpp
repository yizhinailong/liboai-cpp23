#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
    auto expect(bool condition, const std::string& message) -> int {
        if (!condition) {
            std::cerr << "FAIL: " << message << '\n';
            return 1;
        }
        return 0;
    }

    auto locate_repo_file(const std::filesystem::path& rel) -> std::filesystem::path {
        auto current = std::filesystem::current_path();
        for (int i = 0; i < 12; ++i) {
            const auto candidate = current / rel;
            if (std::filesystem::exists(candidate)) {
                return candidate;
            }
            if (!current.has_parent_path()) {
                break;
            }
            current = current.parent_path();
        }
        return {};
    }

    auto read_file(const std::filesystem::path& path) -> std::string {
        std::ifstream in(path);
        if (!in.is_open()) {
            return {};
        }
        std::ostringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }
} // namespace

auto main() -> int {
    const auto source = locate_repo_file("src/components/files.cppm");
    if (expect(!source.empty(), "files.cppm must be locatable from test working directory") != 0) {
        return 1;
    }

    const auto content = read_file(source);
    if (expect(!content.empty(), "files.cppm must be readable") != 0) {
        return 1;
    }

    if (expect(content.find("this->GetOpenAIRoot() + \"/files/\" + file_id + \"/content\"") != std::string::npos, "Files::Download should derive the URL from GetOpenAIRoot") != 0) {
        return 1;
    }

    if (expect(content.find("https://api.openai.com/v1/files/") == std::string::npos, "Files::Download should not hardcode the default API root") != 0) {
        return 1;
    }

    std::cout << "PASS: files download root regression checks" << '\n';
    return 0;
}
