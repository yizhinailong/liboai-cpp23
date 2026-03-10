/**
 * @file network.cppm
 *
 * liboai network implementation.
 * This module provides declarations for the abstracted liboai
 * Network implementation. Each component class will inherit from
 * this class to make use of the network functionality provided by
 * it.
 *
 * For instance, making a call to liboai::Image::Create(...) will
 * make use of both this class to send the request to the OpenAI API
 * and liboai::Authorization to provide the user's authorization
 * information to successfully complete the request.
 */

module;

// Standard library headers
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <future>
#include <optional>
#include <string>
#include <type_traits>

// Third-party library headers
#include <cpr/cpr.h>

export module liboai:core.network;

import :core.error;
import :core.response;

export namespace liboai {

    class Network {
    public:
        /**
         * @brief Initialise the Network instance to use the provided API url.
         *
         * @param root The URL to direct API calls to.
         */
        explicit Network(const std::string& root) noexcept : m_openai_root(root) {}

        Network(const Network&) = delete;
        Network& operator=(const Network&) = delete;
        Network(Network&&) = delete;
        Network& operator=(Network&&) = delete;
        ~Network() = default;

        /**
         * @brief Function to download a file at 'from' to file path 'to.'
         *
         * Useful for downloading images from the OpenAI API given a URL to 'from.'
         * This function is not to be confused with liboai::File::download(...)
         * which is used to download .jsonl files from the OpenAI API.
         *
         * @param to The path and filename to download the file to.
         * @param from Where to download the file data from (such as a URL).
         * @param authorization Authorization header for the request.
         *
         * @return Bool indicating success or failure.
         */
        [[nodiscard]]
        static auto Download(
            const std::string& to,
            const std::string& from,
            const cpr::Header& authorization
        ) noexcept -> Result<bool> {
            std::ofstream file(to, std::ios::binary);
            auto cpr_res = cpr::Download(file, cpr::Url{ from }, authorization);
            auto res = to_liboai_response(std::move(cpr_res));

            if (!res) {
                return std::unexpected(res.error());
            }
            return res->status_code == 200;
        }

        [[nodiscard]]
        static auto DownloadWithSession(
            const std::string& to,
            const std::string& from,
            const cpr::Header& authorization,
            cpr::Session& session
        ) noexcept -> Result<bool> {
            std::ofstream file(to, std::ios::binary);
            session.SetUrl(cpr::Url{ from });
            session.SetHeader(authorization);
            auto cpr_res =
                cpr::Download(file, cpr::Url{ session.GetFullRequestUrl() }, authorization);
            auto res = to_liboai_response(std::move(cpr_res));

            if (!res) {
                return std::unexpected(res.error());
            }
            return res->status_code == 200;
        }

        /**
         * @brief Function to asynchronously download a file at 'from' to file path 'to.'
         *
         * Useful for downloading images from the OpenAI API given a URL to 'from.'
         * This function is not to be confused with liboai::File::download(...)
         * which is used to download .jsonl files from the OpenAI API.
         *
         * @param to The path and filename to download the file to.
         * @param from Where to download the file data from (such as a URL).
         * @param authorization Authorization header for the request.
         *
         * @return Future bool indicating success or failure.
         */
        [[nodiscard]]
        static auto DownloadAsync(
            const std::string& to,
            const std::string& from,
            cpr::Header authorization
        ) noexcept -> FutureExpected<bool> {
            return std::async(std::launch::async,
                              [to, from, authorization = std::move(authorization)]() mutable
                              -> Result<bool> {
                std::ofstream file(to, std::ios::binary);
                auto cpr_res = cpr::Download(file, cpr::Url{ from }, std::move(authorization));
                auto res = to_liboai_response(std::move(cpr_res));

                if (!res) {
                    return std::unexpected(res.error());
                }
                return res->status_code == 200;
            });
        }

        [[nodiscard]]
        static auto DownloadAsyncWithSession(
            const std::string& to,
            const std::string& from,
            cpr::Header authorization,
            cpr::Session& session
        ) noexcept -> FutureExpected<bool> {
            return std::async(
                std::launch::async,
                [to, from, authorization = std::move(authorization), &session]() mutable
                -> Result<bool> {
                std::ofstream file(to, std::ios::binary);
                session.SetUrl(cpr::Url{ from });
                session.SetHeader(authorization);
                auto cpr_res = cpr::Download(
                    file,
                    cpr::Url{ session.GetFullRequestUrl() },
                    std::move(authorization)
                );
                auto res = to_liboai_response(std::move(cpr_res));

                if (!res) {
                    return std::unexpected(res.error());
                }
                return res->status_code == 200;
            });
        }

    protected:
        enum class Method : uint8_t {
            HTTP_GET,   // GET
            HTTP_POST,  // POST
            HTTP_DELETE // DELETE
        };

        template <class... Params>
        requires (... && !std::is_lvalue_reference_v<Params>)
        [[nodiscard]]
        auto Request(
            const Method& http_method,
            const std::string& root,
            const std::string& endpoint,
            const std::string& content_type,
            std::optional<cpr::Header> headers = std::nullopt,
            Params&&... parameters
        ) const -> Result<Response> {
            cpr::Header _headers = {
                { "Content-Type", content_type }
            };
            if (headers) {
                if (headers.value().size() != 0) {
                    for (auto& i : headers.value()) {
                        _headers.insert(std::move(i));
                    }
                }
            }

            cpr::Url url{ root + endpoint };
            cpr::Response cpr_res;

            switch (http_method) {
                case Method::HTTP_GET:
                    if constexpr (sizeof...(parameters) > 0) {
                        cpr_res = cpr::Get(url, _headers, std::forward<Params>(parameters)...);
                    } else {
                        cpr_res = cpr::Get(url, _headers);
                    }
                    break;
                case Method::HTTP_POST:
                    if constexpr (sizeof...(parameters) > 0) {
                        cpr_res = cpr::Post(url, _headers, std::forward<Params>(parameters)...);
                    } else {
                        cpr_res = cpr::Post(url, _headers);
                    }
                    break;
                case Method::HTTP_DELETE:
                    if constexpr (sizeof...(parameters) > 0) {
                        cpr_res = cpr::Delete(url, _headers, std::forward<Params>(parameters)...);
                    } else {
                        cpr_res = cpr::Delete(url, _headers);
                    }
                    break;
            }

            return to_liboai_response(std::move(cpr_res));
        }

        /**
         * @brief Function to validate the existence and validity of a file.
         *
         * This is used in functions that take a file path as a parameter
         * to ensure that the file exists and is valid.
         *
         * @param path The filesystem path to validate.
         *
         * @return True if the file exists, is a regular file, and is not empty.
         */
        [[nodiscard]]
        auto Validate(const std::filesystem::path& path) const -> bool {
            // checks if the file exists, is a regular file, and is not empty
            if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
                return std::filesystem::file_size(path) > 0;
            }
            return false;
        }

        [[nodiscard]]
        const std::string& GetOpenAIRoot() const noexcept {
            return m_openai_root;
        }

        [[nodiscard]]
        const std::string& GetAzureRoot() const noexcept {
            return m_azure_root;
        }

    private:
        const std::string m_openai_root;
        const std::string m_azure_root = ".openai.azure.com/openai";
    };

} // namespace liboai
