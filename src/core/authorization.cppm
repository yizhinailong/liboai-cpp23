/**
 * @file authorization.cppm
 *
 * liboai authorization module.
 *
 * This module provides declarations for authorization directives
 * for authorizing requests with the OpenAI API. Each component class
 * makes use of a single object accessed via liboai::Authorization::Authorizer()
 * to retrieve and use user-set authorization information to successfully
 * complete component API requests.
 */

module;

#include <cpr/cpr.h>

export module liboai:core.authorization;

import std;
import :core.network;
import :core.response;

export namespace liboai {
    class Authorization final {
    public: // cons/des, operator deletions
        Authorization() = default;
        Authorization(const Authorization&) = delete;
        Authorization& operator=(const Authorization&) = delete;
        Authorization(Authorization&&) = delete;
        Authorization& operator=(Authorization&&) = delete;
        ~Authorization();

        /**
         * @brief Singleton paradigm access method.
         *
         * @return A reference to the singleton instance of this class to be
         *         used in all component classes.
         */
        [[nodiscard]]
        static Authorization& Authorizer() noexcept {
            static Authorization instance;
            return instance;
        }

        /**
         * @brief Sets the authorization key for the OpenAI API as the passed string.
         *
         * @param key The authorization key to use in component calls.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetKey(std::string_view key) noexcept -> bool;

        /**
         * @brief Sets the authorization key for the Azure OpenAI API as the passed string.
         *
         * @param key The authorization key to use in Azure component calls.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetAzureKey(std::string_view key) noexcept -> bool;

        /**
         * @brief Sets the Active Directory authorization token for the Azure OpenAI API
         *        as the passed string.
         *
         * @param key The authorization key to use in Azure component calls.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetAzureKeyAD(std::string_view key) noexcept -> bool;

        /**
         * @brief Sets the authorization key for the OpenAI API as the first line present
         *        in the file at the passed path.
         *
         * @param path The path to the file containing the authorization key.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetKeyFile(const std::filesystem::path& path) noexcept -> bool;

        /**
         * @brief Sets the authorization key for the Azure OpenAI API as the first line
         *        present in the file at the passed path.
         *
         * @param key The path to the file containing the authorization key.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetAzureKeyFile(const std::filesystem::path& path) noexcept -> bool;

        /**
         * @brief Sets the Active Directory authorization token for the Azure OpenAI API
         *        as the first line present in the file at the passed path.
         *
         * @param key The path to the file containing the authorization key.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetAzureKeyFileAD(const std::filesystem::path& path) noexcept -> bool;

        /**
         * @brief Sets the authorization key for the OpenAI API as the value stored in
         *        the environment variable with the passed name.
         *
         * @param var The name of the environment variable to retrieve the authorization
         *            key from.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetKeyEnv(std::string_view var) noexcept -> bool;

        /**
         * @brief Sets the authorization key for the Azure OpenAI API as the value stored
         *        in the environment variable with the passed name.
         *
         * @param var The name of the environment variable to retrieve the authorization
         *            key from.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetAzureKeyEnv(std::string_view var) noexcept -> bool;

        /**
         * @brief Sets the Active Directory authorization token for the Azure OpenAI API
         *        as the value stored in the environment variable with the passed name.
         *
         * @param var The name of the environment variable to retrieve the authorization
         *            key from.
         * @return True if the key was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetAzureKeyEnvAD(std::string_view var) noexcept -> bool;

        /**
         * @brief Sets the organization identifier as the passed string for use in
         *        component calls.
         *
         * @param org The organization identifier to use in component calls.
         * @return True if the ID was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetOrganization(std::string_view org) noexcept -> bool;

        /**
         * @brief Sets the organization identifier as the first line present in the file
         *        at the passed path for use in component calls.
         *
         * @param path The path to the file containing the organization identifier.
         * @return True if the ID was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetOrganizationFile(const std::filesystem::path& path) noexcept -> bool;

        /**
         * @brief Sets the organization identifier as the value stored in the environment
         *        variable with the passed name for use in component calls.
         *
         * @param var The name of the environment variable to retrieve the organization
         *            identifier from.
         * @return True if the ID was set successfully, false otherwise.
         */
        [[nodiscard]]
        auto SetOrganizationEnv(std::string_view var) noexcept -> bool;

        /**
         * @brief Sets proxies to use for component calls.
         *
         * @param hosts The hosts to use as proxies in paired { "protocol", "host" } format.
         */
        auto SetProxies(
            const std::initializer_list<std::pair<const std::string, std::string>>& hosts
        ) noexcept -> void;

        /**
         * @brief Sets proxies to use for component calls.
         *
         * @param hosts The hosts to use as proxies in paired { "protocol", "host" } format.
         */
        auto SetProxies(
            std::initializer_list<std::pair<const std::string, std::string>>&& hosts
        ) noexcept -> void;

        /**
         * @brief Sets proxies to use for component calls.
         *
         * @param hosts The hosts to use as proxies in paired { "protocol", "host" } format.
         */
        auto SetProxies(const std::map<std::string, std::string>& hosts) noexcept
            -> void;

        /**
         * @brief Sets proxies to use for component calls.
         *
         * @param hosts The hosts to use as proxies in paired { "protocol", "host" } format.
         */
        auto SetProxies(std::map<std::string, std::string>&& hosts) noexcept -> void;

        /**
         * @brief Sets authentication information for proxies per-protocol.
         *
         * @param proto_up A {protocol, {uname, passwd}} map to use for authentication
         *                 with proxies on a per-protocol basis.
         */
        auto
        SetProxyAuth(const std::map<std::string, cpr::EncodedAuthentication>& proto_up) noexcept
            -> void;

        /**
         * @brief Sets the timeout for component calls in milliseconds.
         */
        auto SetMaxTimeout(int32_t ms) noexcept -> void {
            this->m_timeout = cpr::Timeout(ms);
        }

        /**
         * @brief Returns currently the set authorization key.
         */
        [[nodiscard]]
        constexpr auto GetKey() const noexcept -> const std::string& {
            return this->m_key;
        }

        /**
         * @brief Returns the currently set organization identifier.
         */
        [[nodiscard]]
        constexpr auto GetOrganization() const noexcept -> const std::string& {
            return this->m_org;
        }

        /**
         * @return The currently set proxies.
         */
        [[nodiscard]]
        auto GetProxies() const noexcept -> cpr::Proxies {
            return this->m_proxies;
        }

        /**
         * @return The currently set proxy authentication information.
         */
        [[nodiscard]]
        auto GetProxyAuth() const noexcept -> cpr::ProxyAuthentication {
            return this->m_proxyAuth;
        }

        /**
         * @return The currently set timeout.
         */
        [[nodiscard]]
        auto GetMaxTimeout() const noexcept -> cpr::Timeout {
            return this->m_timeout;
        }

        /**
         * @return An authorization header with the currently set authorization information
         *         for use in component calls.
         */
        [[nodiscard]]
        constexpr auto GetAuthorizationHeaders() const noexcept -> const cpr::Header& {
            return this->m_openai_auth_headers;
        }

        /**
         * @return An authorization header with the currently set Azure authorization
         *         information for use in Azure component calls.
         */
        [[nodiscard]]
        constexpr auto GetAzureAuthorizationHeaders() const noexcept -> const cpr::Header& {
            return this->m_azure_auth_headers;
        }

    private: // member variables
        std::string m_key, m_org;
        cpr::Header m_openai_auth_headers, m_azure_auth_headers;
        cpr::Proxies m_proxies;
        cpr::ProxyAuthentication m_proxyAuth;
        cpr::Timeout m_timeout{ 30000 };
    };

    // Implementation
    Authorization::~Authorization() {
        // Securely clear the key memory
        if (!this->m_key.empty()) {
            volatile char* p = const_cast<volatile char*>(this->m_key.data());
            for (size_t i = 0; i < this->m_key.size(); ++i) {
                p[i] = '\0';
            }
        }
    }

    auto Authorization::SetKey(std::string_view key) noexcept -> bool {
        if (!key.empty()) {
            this->m_key = key;
            if (this->m_openai_auth_headers.count("Authorization") > 0) {
                this->m_openai_auth_headers.erase("Authorization");
            }
            this->m_openai_auth_headers["Authorization"] = ("Bearer " + this->m_key);
            return true;
        }
        return false;
    }

    auto Authorization::SetAzureKey(std::string_view key) noexcept -> bool {
        if (!key.empty()) {
            this->m_key = key;
            if (this->m_azure_auth_headers.size() > 0) {
                this->m_azure_auth_headers.clear();
            }
            this->m_azure_auth_headers["api-key"] = this->m_key;
            return true;
        }
        return false;
    }

    auto Authorization::SetAzureKeyAD(std::string_view key) noexcept -> bool {
        if (!key.empty()) {
            this->m_key = key;
            if (this->m_azure_auth_headers.size() > 0) {
                this->m_azure_auth_headers.clear();
            }
            this->m_azure_auth_headers["Authorization"] = ("Bearer " + this->m_key);
            return true;
        }
        return false;
    }

    auto Authorization::SetKeyFile(const std::filesystem::path& path) noexcept -> bool {
        if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path) &&
            std::filesystem::file_size(path) > 0) {
            std::ifstream file(path);
            if (file.is_open()) {
                std::getline(file, this->m_key);
                if (this->m_openai_auth_headers.count("Authorization") > 0) {
                    this->m_openai_auth_headers.erase("Authorization");
                }
                this->m_openai_auth_headers["Authorization"] = ("Bearer " + this->m_key);
                return true;
            }
        }
        return false;
    }

    auto Authorization::SetAzureKeyFile(const std::filesystem::path& path) noexcept -> bool {
        if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path) &&
            std::filesystem::file_size(path) > 0) {
            std::ifstream file(path);
            if (file.is_open()) {
                std::getline(file, this->m_key);
                if (this->m_azure_auth_headers.size() > 0) {
                    this->m_azure_auth_headers.clear();
                }
                this->m_azure_auth_headers["api-key"] = this->m_key;
                return true;
            }
        }
        return false;
    }

    auto Authorization::SetAzureKeyFileAD(const std::filesystem::path& path) noexcept -> bool {
        if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path) &&
            std::filesystem::file_size(path) > 0) {
            std::ifstream file(path);
            if (file.is_open()) {
                std::getline(file, this->m_key);
                if (this->m_azure_auth_headers.size() > 0) {
                    this->m_azure_auth_headers.clear();
                }
                this->m_azure_auth_headers["Authorization"] = ("Bearer " + this->m_key);
                return true;
            }
        }
        return false;
    }

    auto Authorization::SetKeyEnv(std::string_view var) noexcept -> bool {
        if (!var.empty()) {
            const char* key = std::getenv(var.data());
            if (key != nullptr) {
                this->m_key = key;
                if (this->m_openai_auth_headers.count("Authorization") > 0) {
                    this->m_openai_auth_headers.erase("Authorization");
                }
                this->m_openai_auth_headers["Authorization"] = ("Bearer " + this->m_key);
                return true;
            }
            return false;
        }
        return false;
    }

    auto Authorization::SetAzureKeyEnv(std::string_view var) noexcept -> bool {
        if (!var.empty()) {
            const char* key = std::getenv(var.data());
            if (key != nullptr) {
                this->m_key = key;
                if (this->m_azure_auth_headers.size() > 0) {
                    this->m_azure_auth_headers.clear();
                }
                this->m_azure_auth_headers["api-key"] = this->m_key;
                return true;
            }
            return false;
        }
        return false;
    }

    auto Authorization::SetAzureKeyEnvAD(std::string_view var) noexcept -> bool {
        if (!var.empty()) {
            const char* key = std::getenv(var.data());
            if (key != nullptr) {
                this->m_key = key;
                if (this->m_azure_auth_headers.size() > 0) {
                    this->m_azure_auth_headers.clear();
                }
                this->m_azure_auth_headers["Authorization"] = ("Bearer " + this->m_key);
                return true;
            }
            return false;
        }
        return false;
    }

    auto Authorization::SetOrganization(std::string_view org) noexcept -> bool {
        if (!org.empty()) {
            this->m_org = std::move(org);
            if (this->m_openai_auth_headers.count("OpenAI-Organization") > 0) {
                this->m_openai_auth_headers.erase("OpenAI-Organization");
            }
            this->m_openai_auth_headers["OpenAI-Organization"] = this->m_org;
            return true;
        }
        return false;
    }

    auto Authorization::SetOrganizationFile(const std::filesystem::path& path) noexcept -> bool {
        if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path) &&
            std::filesystem::file_size(path) > 0) {
            std::ifstream file(path);
            if (file.is_open()) {
                std::getline(file, this->m_org);
                if (this->m_openai_auth_headers.count("OpenAI-Organization") > 0) {
                    this->m_openai_auth_headers.erase("OpenAI-Organization");
                }
                this->m_openai_auth_headers["OpenAI-Organization"] = this->m_org;
                return true;
            }
        }
        return false;
    }

    auto Authorization::SetOrganizationEnv(std::string_view var) noexcept -> bool {
        if (!var.empty()) {
            const char* org = std::getenv(var.data());
            if (org != nullptr) {
                this->m_org = org;
                if (this->m_openai_auth_headers.count("OpenAI-Organization") > 0) {
                    this->m_openai_auth_headers.erase("OpenAI-Organization");
                }
                this->m_openai_auth_headers["OpenAI-Organization"] = this->m_org;
                return true;
            }
            return false;
        }
        return false;
    }

    auto Authorization::SetProxies(
        const std::initializer_list<std::pair<const std::string, std::string>>& hosts
    ) noexcept -> void {
        this->m_proxies = cpr::Proxies(hosts);
    }

    auto Authorization::SetProxies(
        std::initializer_list<std::pair<const std::string, std::string>>&& hosts
    ) noexcept -> void {
        this->m_proxies = cpr::Proxies(std::move(hosts));
    }

    auto Authorization::SetProxies(const std::map<std::string, std::string>& hosts) noexcept
        -> void {
        this->m_proxies = cpr::Proxies(hosts);
    }

    auto Authorization::SetProxies(std::map<std::string, std::string>&& hosts) noexcept -> void {
        this->m_proxies = cpr::Proxies(std::move(hosts));
    }

    auto Authorization::SetProxyAuth(
        const std::map<std::string, cpr::EncodedAuthentication>& proto_up
    ) noexcept -> void {
        this->m_proxyAuth = cpr::ProxyAuthentication(proto_up);
    }

} // namespace liboai
