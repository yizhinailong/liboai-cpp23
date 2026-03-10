module;

#include <cpr/cpr.h>

/**
 * @file files.cppm
 *
 * Files component class for OpenAI.
 * This class contains all the methods for the Files component
 * of the OpenAI API. This class provides access to 'Files'
 * endpoints on the OpenAI API and should be accessed via the
 * liboai.h header file through an instantiated liboai::OpenAI
 * object after setting necessary authentication information
 * through the liboai::Authorization::Authorizer() singleton
 * object.
 */

export module liboai:components.files;

import std;
import :core.authorization;
import :core.error;
import :core.response;
import :core.network;

export namespace liboai {
    class Files final : private Network {
    public:
        explicit Files(const std::string& root) : Network(root) {}

        Files(const Files&) = delete;
        Files& operator=(const Files&) = delete;
        Files(Files&&) = delete;
        Files& operator=(Files&&) = delete;
        ~Files() = default;

        /**
         * @brief Returns a list of files that belong to the user's organization.
         *
         * @return A liboai::Response object containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto List() const& noexcept -> Result<Response>;

        /**
         * @brief Asynchronously returns a list of files that belong to the
         *        user's organization.
         *
         * @return A liboai::Response future containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto ListAsync() const& noexcept -> FutureExpected<Response>;

        /**
         * @brief Upload a file that contains document(s) to be
         *        used across various endpoints/features. Currently,
         *        the size of all the files uploaded by one organization
         *        can be up to 1 GB.
         *
         * @param file     The JSON Lines file to be uploaded (path).
         * @param purpose  The intended purpose of the uploaded documents.
         *
         * @return A liboai::Response object containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto Create(
            const std::filesystem::path& file,
            const std::string& purpose
        ) const& noexcept -> Result<Response>;

        /**
         * @brief Asynchronously upload a file that contains document(s)
         *        to be used across various endpoints/features. Currently,
         *        the size of all the files uploaded by one organization
         *        can be up to 1 GB.
         *
         * @param file     The JSON Lines file to be uploaded (path).
         * @param purpose  The intended purpose of the uploaded documents.
         *
         * @return A liboai::Response future containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto CreateAsync(
            const std::filesystem::path& file,
            const std::string& purpose
        ) const& noexcept -> FutureExpected<Response>;

        /**
         * @brief Delete [remove] a file.
         *
         * @param *file_id   The ID of the file to use for this request
         *
         * @return A liboai::Response object containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto Remove(const std::string& file_id) const& noexcept -> Result<Response>;

        /**
         * @brief Asynchronously delete [remove] a file.
         *
         * @param *file_id   The ID of the file to use for this request
         *
         * @return A liboai::Response future containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto RemoveAsync(
            const std::string& file_id
        ) const& noexcept -> FutureExpected<Response>;

        /**
         * @brief Returns information about a specific file.
         *
         * @param *file_id   The ID of the file to use for this request
         *
         * @return A liboai::Response object containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto Retrieve(
            const std::string& file_id
        ) const& noexcept -> Result<Response>;

        /**
         * @brief Asynchronously returns information about a specific file.
         *
         * @param *file_id   The ID of the file to use for this request
         *
         * @return A liboai::Response future containing the image(s)
         *         data in JSON format.
         */
        [[nodiscard]]
        auto RetrieveAsync(
            const std::string& file_id
        ) const& noexcept -> FutureExpected<Response>;

        /**
         * @brief Downloads the contents of the specified file
         *        to the specified path.
         *
         * @param *file_id    The ID of the file to use for this request
         * @param *save_to    The path to save the file to
         *
         * @return a boolean value indicating whether the file was
         *         successfully downloaded or not.
         */
        [[nodiscard]]
        auto Download(
            const std::string& file_id,
            const std::string& save_to
        ) const& noexcept -> Result<bool>;

        /**
         * @brief Asynchronously downloads the contents of the specified file
         *        to the specified path.
         *
         * @param *file_id    The ID of the file to use for this request
         * @param *save_to    The path to save the file to
         *
         * @return a boolean future indicating whether the file was
         *         successfully downloaded or not.
         */
        [[nodiscard]]
        auto DownloadAsync(
            const std::string& file_id,
            const std::string& save_to
        ) const& noexcept -> FutureExpected<bool>;

    private:
        Authorization& m_auth = Authorization::Authorizer();
    };

    // Implementation
    auto Files::List() const& noexcept -> Result<Response> {
        return this->Request(
            Method::HTTP_GET,
            this->GetOpenAIRoot(),
            "/files",
            "application/json",
            this->m_auth.GetAuthorizationHeaders(),
            this->m_auth.GetProxies(),
            this->m_auth.GetProxyAuth(),
            this->m_auth.GetMaxTimeout()
        );
    }

    auto Files::ListAsync() const& noexcept -> FutureExpected<Response> {
        return std::async(std::launch::async, &Files::List, this);
    }

    auto
    Files::Create(
        const std::filesystem::path& file,
        const std::string& purpose
    ) const& noexcept -> Result<Response> {
        if (!this->Validate(file)) {
            return std::unexpected(
                OpenAIError::file_error(
                    "File path provided is non-existent, is not a file, or is empty."
                )
            );
        }

        cpr::Multipart form = {
            { "purpose",                            purpose },
            {    "file", cpr::File{ file.generic_string() } }
        };

        return this->Request(
            Method::HTTP_POST,
            this->GetOpenAIRoot(),
            "/files",
            "multipart/form-data",
            this->m_auth.GetAuthorizationHeaders(),
            std::move(form),
            this->m_auth.GetProxies(),
            this->m_auth.GetProxyAuth(),
            this->m_auth.GetMaxTimeout()
        );
    }

    auto Files::CreateAsync(
        const std::filesystem::path& file,
        const std::string& purpose
    ) const& noexcept -> FutureExpected<Response> {
        return std::async(std::launch::async, &Files::Create, this, file, purpose);
    }

    auto Files::Remove(
        const std::string& file_id
    ) const& noexcept -> Result<Response> {
        return this->Request(
            Method::HTTP_DELETE,
            this->GetOpenAIRoot(),
            "/files/" + file_id,
            "application/json",
            this->m_auth.GetAuthorizationHeaders(),
            this->m_auth.GetProxies(),
            this->m_auth.GetProxyAuth(),
            this->m_auth.GetMaxTimeout()
        );
    }

    auto Files::RemoveAsync(const std::string& file_id) const& noexcept
        -> FutureExpected<Response> {
        return std::async(std::launch::async, &Files::Remove, this, file_id);
    }

    auto Files::Retrieve(
        const std::string& file_id
    ) const& noexcept -> Result<Response> {
        return this->Request(
            Method::HTTP_GET,
            this->GetOpenAIRoot(),
            "/files/" + file_id,
            "application/json",
            this->m_auth.GetAuthorizationHeaders(),
            this->m_auth.GetProxies(),
            this->m_auth.GetProxyAuth(),
            this->m_auth.GetMaxTimeout()
        );
    }

    auto Files::RetrieveAsync(const std::string& file_id) const& noexcept
        -> FutureExpected<Response> {
        return std::async(std::launch::async, &Files::Retrieve, this, file_id);
    }

    auto Files::Download(
        const std::string& file_id,
        const std::string& save_to
    ) const& noexcept -> Result<bool> {
        return Network::Download(
            save_to,
            (this->GetOpenAIRoot() + "/files/" + file_id + "/content"),
            this->m_auth.GetAuthorizationHeaders()
        );
    }

    auto Files::DownloadAsync(
        const std::string& file_id,
        const std::string& save_to
    ) const& noexcept -> FutureExpected<bool> {
        return std::async(std::launch::async, &Files::Download, this, file_id, save_to);
    }

} // namespace liboai
