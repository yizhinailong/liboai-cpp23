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
    const auto source = locate_repo_file("src/core/network.cppm");
    if (expect(!source.empty(), "network.cppm must be locatable") != 0) {
        return 1;
    }

    const auto content = read_file(source);
    if (expect(!content.empty(), "network.cppm must be readable") != 0) {
        return 1;
    }

    // Locate the Request function
    const auto fn_start = content.find("auto Request(");
    if (expect(fn_start != std::string::npos, "Request function implementation must exist") != 0) {
        return 1;
    }

    // Find the header merge section - look for the headers initialization
    const auto headers_init = content.find("cpr::Header _headers", fn_start);
    if (expect(headers_init != std::string::npos, "Request must initialize _headers") != 0) {
        return 1;
    }

    // Extract the section containing the header merge logic
    // Look for the end of the function or the next method
    const auto section_end = content.find("protected:", headers_init);
    const auto section = content.substr(headers_init, section_end - headers_init);

    // Check for move semantics in header insertion
    if (expect(section.find("std::move") != std::string::npos, "Header merge must use move semantics") != 0) {
        return 1;
    }

    // Check for either std::ranges::for_each (after refactor) or traditional loop
    const auto has_ranges_for_each = section.find("std::ranges::for_each") != std::string::npos;
    const auto has_for_loop = section.find("for (auto& i : headers.value())") != std::string::npos ||
                              section.find("for (const auto& i : headers.value())") != std::string::npos;

    if (expect(has_ranges_for_each || has_for_loop, "Header merge must use ranges algorithm or traditional loop") != 0) {
        return 1;
    }

    // Verify the merge logic is present (headers insertion into _headers)
    if (expect(section.find("_headers.insert") != std::string::npos, "Header merge must insert into _headers") != 0) {
        return 1;
    }

    std::cout << "PASS: network header merge regression checks" << '\n';
    return 0;
}
