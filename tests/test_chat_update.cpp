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
    const auto source = locate_repo_file("src/components/chat.cppm");
    if (expect(!source.empty(), "chat.cppm must be locatable") != 0) {
        return 1;
    }

    const auto content = read_file(source);
    if (expect(!content.empty(), "chat.cppm must be readable") != 0) {
        return 1;
    }

    const auto fn_start = content.find("auto Conversation::Update(std::string_view history)");
    if (expect(fn_start != std::string::npos, "Conversation::Update(history) implementation must exist") != 0) {
        return 1;
    }

    const auto fn_end = content.find("auto Conversation::Update(const Response& response)", fn_start);
    if (expect(fn_end != std::string::npos, "Conversation::Update(history) boundary must exist") != 0) {
        return 1;
    }

    const auto section = content.substr(fn_start, fn_end - fn_start);
    if (expect(section.find("if (j.contains(\"choices\"))") != std::string::npos, "Update must support choices.message payload shape") != 0) {
        return 1;
    }
    if (expect(section.find("else if (j.contains(\"message\"))") != std::string::npos, "Update must support top-level message payload shape") != 0) {
        return 1;
    }
    if (expect(section.find("else if (j.contains(\"role\") && j.contains(\"content\"))") != std::string::npos, "Update must support top-level role/content payload shape") != 0) {
        return 1;
    }
    if (expect(section.find("catch (const nlohmann::json::parse_error& e)") != std::string::npos, "Update must return parse_error for malformed JSON") != 0) {
        return 1;
    }
    if (expect(section.find("OpenAIError::parse_error") != std::string::npos, "Update malformed JSON path must map to parse_error") != 0) {
        return 1;
    }

    std::cout << "PASS: chat update regression checks" << '\n';
    return 0;
}
