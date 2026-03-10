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
    const auto source = locate_repo_file("src/core/authorization.cppm");
    if (expect(!source.empty(), "authorization.cppm must be locatable") != 0) {
        return 1;
    }

    const auto content = read_file(source);
    if (expect(!content.empty(), "authorization.cppm must be readable") != 0) {
        return 1;
    }

    const auto fn_start = content.find("auto Authorization::SetOrganizationFile(");
    if (expect(fn_start != std::string::npos, "SetOrganizationFile implementation must exist") != 0) {
        return 1;
    }

    const auto fn_end = content.find("auto Authorization::SetOrganizationEnv(", fn_start);
    if (expect(fn_end != std::string::npos, "SetOrganizationFile implementation boundary must exist") != 0) {
        return 1;
    }

    const auto section = content.substr(fn_start, fn_end - fn_start);
    if (expect(section.find("std::getline(file, this->m_org);") != std::string::npos, "SetOrganizationFile must read into m_org") != 0) {
        return 1;
    }
    if (expect(section.find("std::getline(file, this->m_key);") == std::string::npos, "SetOrganizationFile must not read into m_key") != 0) {
        return 1;
    }
    if (expect(section.find("OpenAI-Organization") != std::string::npos, "SetOrganizationFile must update OpenAI-Organization header") != 0) {
        return 1;
    }

    std::cout << "PASS: authorization regression checks" << '\n';
    return 0;
}
