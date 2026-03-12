module;

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>

/**
 * @file chat.cppm
 * @brief Chat component module file
 *
 * This module contains all the methods for the Chat component of the OpenAI API.
 * This class provides access to 'Chat' endpoints on the OpenAI API and should be
 * accessed via the liboai module through an instantiated liboai::OpenAI
 * object after setting necessary authentication information through the
 * liboai::Authorization::Authorizer() singleton object.
 */

export module liboai:components.chat;

import std;
import :core.authorization;
import :core.error;
import :core.response;
import :core.network;

namespace {
    auto SplitNonEmptyLines(std::string_view data) -> std::vector<std::string> {
        std::vector<std::string> split_data;
        split_data.reserve(static_cast<std::size_t>(std::ranges::count(data, '\n')) + 1U);

        for (auto&& line_view : data | std::views::split('\n')) {
            std::string line(std::ranges::begin(line_view), std::ranges::end(line_view));
            if (!line.empty()) {
                split_data.push_back(std::move(line));
            }
        }

        return split_data;
    }
}

export namespace liboai {
    /**
     * @brief Class containing methods for building Function objects to supply
     *        to the OpenAI ChatCompletions component class via the associated
     *        Conversation class.
     */
    class Functions final {
    public:
        Functions();
        Functions(const Functions& other);
        Functions(Functions&& old) noexcept;

        template <class... _Fname>
        requires (... && std::convertible_to<_Fname, std::string_view>)
        Functions(_Fname... function_names) {
            auto result = this->AddFunctions(function_names...);
        }

        ~Functions() = default;

        Functions& operator=(const Functions& other);
        Functions& operator=(Functions&& old) noexcept;

        /**
         * @brief Denotes a parameter of a function, which includes the
         *        parameter's name, type, description, and an optional enumeration.
         *
         * @param name        The name of the parameter.
         * @param type        The type of the parameter.
         * @param description The description of the parameter.
         * @param enumeration An optional enumeration of possible values for the parameter.
         */
        struct FunctionParameter {
            FunctionParameter() = default;

            FunctionParameter(
                std::string_view name,
                std::string_view type,
                std::string_view description,
                std::optional<std::vector<std::string>> enumeration = std::nullopt
            )
                : name(name), type(type), description(description), enumeration(enumeration) {}

            FunctionParameter(const FunctionParameter& other) = default;
            FunctionParameter(FunctionParameter&& old) noexcept = default;
            ~FunctionParameter() = default;

            FunctionParameter& operator=(const FunctionParameter& other) = default;
            FunctionParameter& operator=(FunctionParameter&& old) noexcept = default;

            std::string name;
            std::string type;
            std::string description;
            std::optional<std::vector<std::string>> enumeration;
        };

        /**
         * @brief Adds a function named 'function_name' to the list of functions.
         *
         * This function, once added, can then be referenced in subsequent 'Functions'
         * class method calls by the name provided here.
         *
         * @param function_name The name of the function to add.
         * @return True/False denoting whether the function was added successfully.
         */
        [[nodiscard]]
        auto AddFunction(std::string_view function_name) & noexcept -> Result<bool>;

        /**
         * @brief Same as AddFunction, but allows for adding multiple functions at once.
         *
         * @param function_names The name of the function to add.
         * @return True/False denoting whether the functions were added successfully.
         */
        [[nodiscard]]
        auto AddFunctions(std::initializer_list<std::string_view> function_names) & noexcept
            -> Result<bool>;

        /**
         * @brief Same as AddFunction, but allows for adding multiple functions at once.
         *
         * @param function_names The name of the function to add.
         * @return True/False denoting whether the functions were added successfully.
         */
        [[nodiscard]]
        auto AddFunctions(std::vector<std::string> function_names) & noexcept
            -> Result<bool>;

        /**
         * @brief Same as AddFunction, but allows for adding multiple functions at once.
         *
         * @param function_names The name of the function to add.
         * @return True/False denoting whether the functions were added successfully.
         */
        template <
            class... _Fnames,
            std::enable_if_t<
                std::conjunction_v<std::is_convertible<_Fnames, std::string_view>...>,
                int> = 0>
        [[nodiscard]] auto AddFunctions(_Fnames... function_names) & noexcept
            -> Result<bool> {
            return this->AddFunctions({ function_names... });
        }

        /**
         * @brief Pops the specified function from the list of functions.
         *
         * This will also remove any associated name, description, parameters, and so on
         * as it involves removing the entire 'function_name' key from the JSON object.
         *
         * @param function_name The name of the function to pop.
         * @return True/False denoting whether the function was popped successfully.
         */
        [[nodiscard]]
        auto PopFunction(std::string_view function_name) & noexcept -> Result<bool>;

        /**
         * @brief Same as PopFunction, but allows for popping multiple functions at once.
         *
         * @param function_names The name of the function to pop.
         * @return True/False denoting whether the functions were popped successfully.
         */
        [[nodiscard]]
        auto PopFunctions(std::initializer_list<std::string_view> function_names) & noexcept
            -> Result<bool>;

        /**
         * @brief Same as PopFunction, but allows for popping multiple functions at once.
         *
         * @param function_names The name of the function to pop.
         * @return True/False denoting whether the functions were popped successfully.
         */
        [[nodiscard]]
        auto PopFunctions(std::vector<std::string> function_names) & noexcept
            -> Result<bool>;

        /**
         * @brief Same as PopFunction, but allows for popping multiple functions at once.
         *
         * @param function_names The name of the function to pop.
         * @return True/False denoting whether the functions were popped successfully.
         */
        template <
            class... _Fnames,
            std::enable_if_t<
                std::conjunction_v<std::is_convertible<_Fnames, std::string_view>...>,
                int> = 0>
        [[nodiscard]] auto PopFunctions(_Fnames... function_names) & noexcept
            -> Result<bool> {
            return this->PopFunctions({ function_names... });
        }

        /**
         * @brief Sets a previously added function's description.
         *
         * @param target      The name of the function to set the description of.
         * @param description The description to set for the function.
         * @return True/False denoting whether the description was set successfully.
         */
        [[nodiscard]]
        auto SetDescription(std::string_view target, std::string_view description) & noexcept
            -> Result<bool>;

        /**
         * @brief Pops a previously added function's description.
         *
         * @param target The name of the function to pop the description of.
         * @return True/False denoting whether the description was popped successfully.
         */
        [[nodiscard]]
        auto PopDescription(std::string_view target) & noexcept -> Result<bool>;

        /**
         * @brief Sets which set function parameters are required.
         *
         * @param target The name of the function to set the required parameters of.
         * @param params A series of parameter names to set as required.
         * @return True/False denoting whether the required parameters were set successfully.
         */
        [[nodiscard]]
        auto SetRequired(
            std::string_view target,
            std::initializer_list<std::string_view> params
        ) & noexcept -> Result<bool>;

        /**
         * @brief Sets which set function parameters are required.
         *
         * @param target The name of the function to set the required parameters of.
         * @param params A series of parameter names to set as required.
         * @return True/False denoting whether the required parameters were set successfully.
         */
        [[nodiscard]]
        auto SetRequired(std::string_view target, std::vector<std::string> params) & noexcept
            -> Result<bool>;

        /**
         * @brief Sets which set function parameters are required.
         *
         * @param target The name of the function to set the required parameters of.
         * @param params A series of parameter names to set as required.
         * @return True/False denoting whether the required parameters were set successfully.
         */
        template <
            class... _Rp,
            std::enable_if_t<
                std::conjunction_v<std::is_convertible<_Rp, std::string_view>...>,
                int> = 0>
        [[nodiscard]] auto SetRequired(std::string_view target, _Rp... params) & noexcept
            -> Result<bool> {
            return SetRequired(target, { params... });
        }

        /**
         * @brief Pops previously set required function parameters.
         *
         * @param target The name of the function to pop the required parameters of.
         * @return True/False denoting whether the required parameters were popped successfully.
         */
        [[nodiscard]]
        auto PopRequired(std::string_view target) & noexcept -> Result<bool>;

        /**
         * @brief Appends a parameter to a previously set series of required function parameters.
         *
         * This function should only be called if required parameters have already been
         * set for 'target' via SetRequired().
         *
         * @param target The name of the function to append the required parameter to.
         * @param param  The name of the parameter to append to the required parameters.
         * @return True/False denoting whether the required parameter was appended successfully.
         */
        [[nodiscard]]
        auto AppendRequired(std::string_view target, std::string_view param) & noexcept
            -> Result<bool>;

        /**
         * @brief Appends multiple parameters to a previously set series of required
         *        function parameters.
         *
         * This function should only be called if required parameters have already
         * been set for 'target' via SetRequired().
         *
         * @param target The name of the function to append the required parameter to.
         * @param params The name of the parameters to append to the required parameters.
         * @return True/False denoting whether the required parameter was appended successfully.
         */
        [[nodiscard]]
        auto AppendRequired(
            std::string_view target,
            std::initializer_list<std::string_view> params
        ) & noexcept -> Result<bool>;

        /**
         * @brief Appends multiple parameters to a previously set series of required
         *        function parameters.
         *
         * This function should only be called if required parameters have already
         * been set for 'target' via SetRequired().
         *
         * @param target The name of the function to append the required parameter to.
         * @param params The name of the parameters to append to the required parameters.
         * @return True/False denoting whether the required parameter was appended successfully.
         */
        [[nodiscard]]
        auto AppendRequired(std::string_view target, std::vector<std::string> params) & noexcept
            -> Result<bool>;

        /**
         * @brief Appends multiple parameters to a previously set series of required
         *        function parameters.
         *
         * This function should only be called if required parameters have already
         * been set for 'target' via SetRequired().
         *
         * @param target The name of the function to append the required parameter to.
         * @param params The name of the parameters to append to the required parameters.
         * @return True/False denoting whether the required parameter was appended successfully.
         */
        template <
            class... _Rp,
            std::enable_if_t<
                std::conjunction_v<std::is_convertible<_Rp, std::string_view>...>,
                int> = 0>
        [[nodiscard]] auto AppendRequired(std::string_view target, _Rp... params) & noexcept
            -> Result<bool> {
            return AppendRequired(target, { params... });
        }

        /**
         * @brief Adds a single parameter to an added function.
         *
         * @param target    The name of the function to add the parameter to.
         * @param parameter The parameter to add to the function.
         * @return True/False denoting whether the parameter was added successfully.
         */
        [[nodiscard]]
        auto SetParameter(std::string_view target, FunctionParameter parameter) & noexcept
            -> Result<bool>;

        /**
         * @brief Adds a series of parameters to an added function.
         *
         * @param target     The name of the function to add the parameters to.
         * @param parameters The parameters to add to the function.
         * @return True/False denoting whether the parameters were added successfully.
         */
        [[nodiscard]]
        auto SetParameters(
            std::string_view target,
            std::initializer_list<FunctionParameter> parameters
        ) & noexcept -> Result<bool>;

        /**
         * @brief Adds a series of parameters to an added function.
         *
         * @param target     The name of the function to add the parameters to.
         * @param parameters The parameters to add to the function.
         * @return True/False denoting whether the parameters were added successfully.
         */
        [[nodiscard]]
        auto
        SetParameters(std::string_view target, std::vector<FunctionParameter> parameters) & noexcept
            -> Result<bool>;

        /**
         * @brief Adds a series of parameters to an added function.
         *
         * @param target     The name of the function to add the parameters to.
         * @param parameters The parameters to add to the function.
         * @return True/False denoting whether the parameters were added successfully.
         */
        template <
            class... _Fp,
            std::enable_if_t<std::conjunction_v<std::is_same<_Fp, FunctionParameter>...>, int> = 0>
        [[nodiscard]] auto SetParameters(std::string_view target, _Fp... parameters) & noexcept
            -> Result<bool> {
            return SetParameters(target, { parameters... });
        }

        /**
         * @brief Pops all of a function's set parameters.
         *
         * This function removes set 'required' values and anything else that falls
         * under the category of 'parameters' as a result of removing the entire
         * 'parameters' section.
         *
         * @param target The name of the function to pop the parameters of.
         * @return True/False denoting whether the parameters were popped successfully.
         */
        [[nodiscard]]
        auto PopParameters(std::string_view target) & noexcept -> Result<bool>;

        /**
         * @brief Pops one or more of a function's set parameters.
         *
         * @param target      The name of the function to pop the parameters of.
         * @param param_names The names of the parameters to pop.
         * @return True/False denoting whether the parameters were popped successfully.
         */
        [[nodiscard]]
        auto PopParameters(
            std::string_view target,
            std::initializer_list<std::string_view> param_names
        ) & noexcept -> Result<bool>;

        /**
         * @brief Pops one or more of a function's set parameters.
         *
         * @param target      The name of the function to pop the parameters of.
         * @param param_names The names of the parameters to pop.
         * @return True/False denoting whether the parameters were popped successfully.
         */
        [[nodiscard]]
        auto PopParameters(std::string_view target, std::vector<std::string> param_names) & noexcept
            -> Result<bool>;

        /**
         * @brief Pops one or more of a function's set parameters.
         *
         * @param target      The name of the function to pop the parameters of.
         * @param param_names The names of the parameters to pop.
         * @return True/False denoting whether the parameters were popped successfully.
         */
        template <
            class... _Pname,
            std::enable_if_t<
                std::conjunction_v<std::is_convertible<_Pname, std::string_view>...>,
                int> = 0>
        [[nodiscard]] auto PopParameters(std::string_view target, _Pname... param_names) & noexcept
            -> Result<bool> {
            return PopParameters(target, { param_names... });
        }

        /**
         * @brief Appends a single parameter to a previously added function.
         *
         * @param target    The name of the function to append the parameter to.
         * @param parameter The parameter to append to the function.
         * @return True/False denoting whether the parameter was appended successfully.
         */
        [[nodiscard]]
        auto AppendParameter(std::string_view target, FunctionParameter parameter) & noexcept
            -> Result<bool>;

        /**
         * @brief Appends a series of parameters to a previously added function.
         *
         * @param target     The name of the function to append the parameters to.
         * @param parameters The parameters to append to the function.
         * @return True/False denoting whether the parameters were appended successfully.
         */
        [[nodiscard]]
        auto AppendParameters(
            std::string_view target,
            std::initializer_list<FunctionParameter> parameters
        ) & noexcept -> Result<bool>;

        /**
         * @brief Appends a series of parameters to a previously added function.
         *
         * @param target     The name of the function to append the parameters to.
         * @param parameters The parameters to append to the function.
         * @return True/False denoting whether the parameters were appended successfully.
         */
        [[nodiscard]]
        auto AppendParameters(
            std::string_view target,
            std::vector<FunctionParameter> parameters
        ) & noexcept -> Result<bool>;

        /**
         * @brief Appends a series of parameters to a previously added function.
         *
         * @param target     The name of the function to append the parameters to.
         * @param parameters The parameters to append to the function.
         * @return True/False denoting whether the parameters were appended successfully.
         */
        template <
            class... _Fp,
            std::enable_if_t<std::conjunction_v<std::is_same<_Fp, FunctionParameter>...>, int> = 0>
        [[nodiscard]] auto AppendParameters(std::string_view target, _Fp... parameters) & noexcept
            -> Result<bool> {
            return AppendParameters(target, { parameters... });
        }

        /**
         * @brief Returns the JSON object of the internal conversation.
         */
        auto GetJSON() const& noexcept -> const nlohmann::json&;

    private:
        using index = std::size_t;
        [[nodiscard]] auto GetFunctionIndex(std::string_view function_name) const& noexcept
            -> Result<index>;

        nlohmann::json m_functions;
    };

    /**
     * @brief Class containing, and used for keeping track of, the chat history.
     *
     * An object of this class should be created, set with system and user data,
     * and provided to ChatCompletion::create (system is optional).
     *
     * The general usage of this class is as follows:
     * - 1. Create a ChatCompletion::Conversation object.
     * - 2. Set the user data, which is the user's input - such as a question or
     *      a command as well as optionally set the system data to guide how the
     *      assistant responds.
     * - 3. Provide the ChatCompletion::Conversation object to ChatCompletion::create.
     * - 4. Update the ChatCompletion::Conversation object with the response from
     *      the API - either the object or the response content can be used to
     *      update the object.
     * - 5. Retrieve the assistant's response from the ChatCompletion::Conversation
     *      object.
     * - 6. Repeat steps 2, 3, 4 and 5 until the conversation is complete.
     *
     * After providing the object to ChatCompletion::create, the object will be
     * updated with the 'assistant' response - this response is the assistant's
     * response to the user's input. A developer could then retrieve this response
     * and display it to the user, and then set the next user input in the object
     * and pass it back to ChatCompletion::create, if desired.
     */
    class Conversation final {
    public:
        Conversation();
        Conversation(const Conversation& other);
        Conversation(Conversation&& old) noexcept;
        explicit Conversation(std::string_view system_data);
        Conversation(std::string_view system_data, std::string_view user_data);
        Conversation(
            std::string_view system_data,
            std::initializer_list<std::string_view> user_data
        );
        explicit Conversation(std::initializer_list<std::string_view> user_data);
        explicit Conversation(const std::vector<std::string>& user_data);
        ~Conversation() = default;

        Conversation& operator=(const Conversation& other);
        Conversation& operator=(Conversation&& old) noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Conversation& conv);

        /**
         * @brief Changes the content of the first system message in the conversation.
         *
         * This method updates the content of the first system message in the
         * conversation, if it exists and is of type "system". If the first message
         * is not a system message or the conversation is empty, the method will
         * return false.
         *
         * @param new_data A string_view containing the new content for the system
         *                 message. Must be non-empty.
         * @return True/False denoting whether the first system message was changed
         *         successfully.
         */
        [[nodiscard]]
        auto ChangeFirstSystemMessage(std::string_view new_data) & noexcept
            -> Result<bool>;

        /**
         * @brief Sets the system data for the conversation.
         *
         * This method sets the system data for the conversation. The system data
         * is the data that helps set the behavior of the assistant so it knows
         * how to respond.
         *
         * @param data The system data to set.
         * @return True/False denoting whether the system data was set successfully.
         */
        [[nodiscard]]
        auto SetSystemData(std::string_view data) & noexcept -> Result<bool>;

        /**
         * @brief Removes the set system data from the top of the conversation.
         *
         * The system data must be the first data set, if used, in order to be
         * removed. If the system data is not the first data set, this method
         * will return false.
         *
         * @return True/False denoting whether the system data was removed successfully.
         */
        [[nodiscard]]
        auto PopSystemData() & noexcept -> Result<bool>;

        /**
         * @brief Adds user input to the conversation.
         *
         * This method adds user input to the conversation. The user input is the
         * user's input - such as a question or a command.
         *
         * If using a system prompt, the user input should be provided after the
         * system prompt is set - i.e. after SetSystemData() is called.
         *
         * @param data The user input to add.
         * @return True/False denoting whether the user input was added successfully.
         */
        [[nodiscard]]
        auto AddUserData(std::string_view data) & noexcept -> Result<bool>;

        /**
         * @brief Adds user input to the conversation.
         *
         * This method adds user input to the conversation. The user input is the
         * user's input - such as a question or a command.
         *
         * If using a system prompt, the user input should be provided after the
         * system prompt is set - i.e. after SetSystemData() is called.
         *
         * @param data The user input to add.
         * @param name The name of the author of this message. Name is required if
         *             role is function, and it should be the name of the function
         *             whose response is in the content.
         * @return True/False denoting whether the user input was added successfully.
         */
        [[nodiscard]]
        auto AddUserData(std::string_view data, std::string_view name) & noexcept
            -> Result<bool>;

        /**
         * @brief Removes the last added user data.
         *
         * @return True/False denoting whether the user data was removed.
         */
        [[nodiscard]]
        auto PopUserData() & noexcept -> Result<bool>;

        /**
         * @brief Gets the last response from the assistant.
         *
         * This method gets the last response from the assistant. The response is
         * the assistant's response to the user's input.
         */
        auto GetLastResponse() const& noexcept -> Result<std::string>;

        /**
         * @brief Returns whether the most recent response contains a function_call.
         *
         * This follows a call to Update. It is important to note that, when making
         * use of functions, a developer must call this method to determine whether
         * the response contains a function call or if it contains a regular response.
         *
         * @return True/False denoting whether the most recent response contains a
         *         function_call or not.
         */
        [[nodiscard]]
        auto LastResponseIsFunctionCall() const& noexcept -> Result<bool>;

        /**
         * @brief Returns the name of the function_call in the most recent response.
         *
         * This should only be called if LastResponseIsFunctionCall() returns true.
         */
        [[nodiscard]]
        auto GetLastFunctionCallName() const& noexcept -> Result<std::string>;

        /**
         * @brief Returns the arguments of the function_call in the most recent response.
         *
         * Returns them in their raw JSON form. This should only be called if
         * LastResponseIsFunctionCall() returns true.
         */
        [[nodiscard]]
        auto GetLastFunctionCallArguments() const& noexcept -> Result<std::string>;

        /**
         * @brief Removes the last assistant response.
         *
         * @return True/False denoting whether the last response was removed.
         */
        [[nodiscard]]
        auto PopLastResponse() & noexcept -> Result<bool>;

        /**
         * @brief Updates the conversation given JSON data.
         *
         * This method updates the conversation given JSON data. The JSON data
         * should be the JSON 'messages' data returned from the OpenAI API.
         *
         * This method should only be used if AppendStreamData was NOT used
         * immediately before it.
         *
         * For instance, if we made a call to create*(), and provided a callback
         * function to stream and, within this callback, we used AppendStreamData
         * to update the conversation per message, we would NOT want to use this
         * method. In this scenario, the AppendStreamData method would have already
         * updated the conversation, so this method would be a bad idea to call
         * afterwards.
         *
         * @param history The JSON data to update the conversation with. This should
         *                be the 'messages' array of data returned from a call to
         *                ChatCompletion::create.
         * @return True/False denoting whether the conversation was updated.
         */
        [[nodiscard]]
        auto Update(std::string_view history) & noexcept -> Result<bool>;

        /**
         * @brief Updates the conversation given a Response object.
         *
         * This method updates the conversation given a Response object. This method
         * should only be used if AppendStreamData was NOT used immediately before it.
         *
         * For instance, if we made a call to create*(), and provided a callback
         * function to stream and, within this callback, we used AppendStreamData
         * to update the conversation per message, we would NOT want to use this
         * method. In this scenario, the AppendStreamData method would have already
         * updated the conversation, so this method would be a bad idea to call
         * afterwards.
         *
         * @param response The Response to update the conversation with. This should
         *                 be the Response returned from a call to ChatCompletion::create.
         * @return True/False denoting whether the update was successful.
         */
        [[nodiscard]]
        auto Update(const Response& response) & noexcept -> Result<bool>;

        /**
         * @brief Exports the entire conversation to a JSON string.
         *
         * This method exports the conversation to a JSON string. The JSON string
         * can be used to save the conversation to a file. The exported string
         * contains both the conversation and included functions, if any.
         *
         * @return The JSON string representing the conversation.
         */
        [[nodiscard]]
        auto Export() const& noexcept -> Result<std::string>;

        /**
         * @brief Imports a conversation from a JSON string.
         *
         * This method imports a conversation from a JSON string. The JSON string
         * should be the JSON string returned from a call to Export().
         *
         * @param json The JSON string to import the conversation from.
         * @return True/False denoting whether the conversation was imported.
         */
        [[nodiscard]]
        auto Import(std::string_view json) & noexcept -> Result<bool>;

        /**
         * @brief Appends stream data (SSEs) from streamed methods.
         *
         * This method updates the conversation given a token from a streamed method.
         * This method should be used when using streamed methods such as
         * ChatCompletion::create or create_async with a callback supplied. This
         * function should be called from within the stream's callback function
         * receiving the SSEs.
         *
         * @param token Streamed token (data) to update the conversation with.
         */
        [[nodiscard]]
        auto AppendStreamData(const std::string& data) & noexcept -> Result<bool>;

        /**
         * @brief Appends stream data (SSEs) from streamed methods.
         *
         * This method updates the conversation given a token from a streamed method.
         * This method should be used when using streamed methods such as
         * ChatCompletion::create or create_async with a callback supplied. This
         * function should be called from within the stream's callback function
         * receiving the SSEs.
         *
         * @param token    Streamed token (data) to update the conversation with.
         * @param delta    Output parameter. The delta to append to the conversation.
         * @param completed Output parameter. Whether the stream is completed.
         */
        [[nodiscard]]
        auto
        AppendStreamData(const std::string& data, std::string& delta, bool& completed) & noexcept
            -> Result<bool>;

        /**
         * @brief Sets the functions to be used for the conversation.
         *
         * This method sets the functions to be used for the conversation.
         *
         * @param functions The functions to set.
         * @return True/False denoting whether the functions were set.
         */
        [[nodiscard]]
        auto SetFunctions(const Functions& functions) & noexcept -> Result<bool>;

        /**
         * @brief Pops any previously set functions.
         */
        auto PopFunctions() & noexcept -> void;

        /**
         * @brief Returns the raw JSON dump of the internal conversation object
         *        in string format.
         */
        [[nodiscard]]
        auto GetRawConversation() const& noexcept -> Result<std::string>;

        /**
         * @brief Returns the JSON object of the internal conversation.
         */
        [[nodiscard]]
        auto GetJSON() const& noexcept -> const nlohmann::json&;

        /**
         * @brief Returns the raw JSON dump of the internal functions object
         *        in string format - if one exists.
         */
        [[nodiscard]]
        auto GetRawFunctions() const& noexcept -> Result<std::string>;

        /**
         * @brief Returns the JSON object of the set functions.
         */
        [[nodiscard]]
        auto GetFunctionsJSON() const& noexcept -> const nlohmann::json&;

        /**
         * @brief Returns whether the conversation has functions or not.
         *
         *        This function call from ChatComplete.
         */
        [[nodiscard]] constexpr auto HasFunctions() const& noexcept -> bool {
            return this->m_functions ? true : false;
        }

        /**
         * @brief Sets the maximum history size for the conversation.
         *
         * @param size The maximum number of messages allowed in the conversation history.
         *             Older messages will be removed when the limit is exceeded.
         */
        auto SetMaxHistorySize(size_t size) noexcept -> void { m_max_history_size = size; }

    private:
        friend class ChatCompletion;
        friend class Azure;
        [[nodiscard]] auto SplitStreamedData(std::string data) const noexcept
            -> Result<std::vector<std::string>>;
        auto RemoveStrings(std::string& s, std::string_view p) const noexcept -> void;
        auto EraseExtra() -> void;
        /**
         * @brief Split full stream data that read from remote server.
         *
         * @return vector of string that contains the split data that will contains
         *         the last termination string(data: "DONE").
         */
        [[nodiscard]] auto SplitFullStreamedData(const std::string& data) const noexcept
            -> Result<std::vector<std::string>>;
        auto ParseStreamData(std::string data, std::string& delta, bool& completed) noexcept
            -> Result<bool>;

        nlohmann::json m_conversation;
        std::optional<nlohmann::json> m_functions = std::nullopt;
        bool m_last_resp_is_fc = false;
        std::string m_last_incomplete_buffer;
        size_t m_max_history_size = std::numeric_limits<size_t>::max();
    };

    class ChatCompletion final : private Network {
    public:
        explicit ChatCompletion(const std::string& root) : Network(root) {}

        ChatCompletion(const ChatCompletion&) = delete;
        ChatCompletion& operator=(const ChatCompletion&) = delete;
        ChatCompletion(ChatCompletion&&) = delete;
        ChatCompletion& operator=(ChatCompletion&&) = delete;
        ~ChatCompletion() = default;

        using ChatStreamCallback = std::function<bool(std::string, intptr_t, Conversation&)>;

        /**
         * @brief Creates a completion for the chat message.
         *
         * @param model            ID of the model to use. Currently, only gpt-3.5-turbo
         *                         and gpt-3.5-turbo-0301 are supported.
         * @param conversation     A Conversation object containing the conversation data.
         * @param function_call    Controls how the model responds to function calls.
         *                         "none" means the model does not call a function, and
         *                         responds to the end-user. "auto" means the model can
         *                         pick between an end-user or calling a function.
         * @param temperature      What sampling temperature to use, between 0 and 2.
         *                         Higher values like 0.8 will make the output more random,
         *                         while lower values like 0.2 will make it more focused
         *                         and deterministic.
         * @param top_p            An alternative to sampling with temperature, called
         *                         nucleus sampling, where the model considers the results
         *                         of the tokens with top_p probability mass. So 0.1 means
         *                         only the tokens comprising the top 10% probability mass
         *                         are considered.
         * @param n                How many chat completion choices to generate for each
         *                         input message.
         * @param stream           If set, partial message deltas will be sent, like in
         *                         ChatGPT. Tokens will be sent as data-only server-sent
         *                         events as they become available, with the stream
         *                         terminated by a data: [DONE] message.
         * @param stop             Up to 4 sequences where the API will stop generating
         *                         further tokens.
         * @param max_tokens       The maximum number of tokens allowed for the generated
         *                         answer. By default, the number of tokens the model can
         *                         return will be (4096 - prompt tokens).
         * @param presence_penalty Number between -2.0 and 2.0. Positive values penalize
         *                         new tokens based on whether they appear in the text so
         *                         far, increasing the model's likelihood to talk about
         *                         new topics.
         * @param frequency_penalty Number between -2.0 and 2.0. Positive values penalize
         *                          new tokens based on their existing frequency in the
         *                          text so far, decreasing the model's likelihood to
         *                          repeat the same line verbatim.
         * @param logit_bias       Modify the likelihood of specified tokens appearing
         *                         in the completion.
         * @param user             The user ID to associate with the request. This is
         *                         used to prevent abuse of the API.
         *
         * @return A liboai::Response object containing the data in JSON format.
         */
        [[nodiscard]]
        auto Create(
            const std::string& model,
            Conversation& conversation,
            std::optional<std::string> function_call = std::nullopt,
            std::optional<float> temperature = std::nullopt,
            std::optional<float> top_p = std::nullopt,
            std::optional<uint16_t> n = std::nullopt,
            std::optional<ChatStreamCallback> stream = std::nullopt,
            std::optional<std::vector<std::string>> stop = std::nullopt,
            std::optional<uint16_t> max_tokens = std::nullopt,
            std::optional<float> presence_penalty = std::nullopt,
            std::optional<float> frequency_penalty = std::nullopt,
            std::optional<std::unordered_map<std::string, int8_t>> logit_bias = std::nullopt,
            std::optional<std::string> user = std::nullopt
        ) const& noexcept -> Result<Response>;

        /**
         * @brief Asynchronously creates a completion for the chat message.
         *
         * @param model            ID of the model to use. Currently, only gpt-3.5-turbo
         *                         and gpt-3.5-turbo-0301 are supported.
         * @param conversation     A Conversation object containing the conversation data.
         * @param function_call    Controls how the model responds to function calls.
         *                         "none" means the model does not call a function, and
         *                         responds to the end-user. "auto" means the model can
         *                         pick between an end-user or calling a function.
         * @param temperature      What sampling temperature to use, between 0 and 2.
         *                         Higher values like 0.8 will make the output more random,
         *                         while lower values like 0.2 will make it more focused
         *                         and deterministic.
         * @param top_p            An alternative to sampling with temperature, called
         *                         nucleus sampling, where the model considers the results
         *                         of the tokens with top_p probability mass. So 0.1 means
         *                         only the tokens comprising the top 10% probability mass
         *                         are considered.
         * @param n                How many chat completion choices to generate for each
         *                         input message.
         * @param stream           If set, partial message deltas will be sent, like in
         *                         ChatGPT. Tokens will be sent as data-only server-sent
         *                         events as they become available, with the stream
         *                         terminated by a data: [DONE] message.
         * @param stop             Up to 4 sequences where the API will stop generating
         *                         further tokens.
         * @param max_tokens       The maximum number of tokens allowed for the generated
         *                         answer. By default, the number of tokens the model can
         *                         return will be (4096 - prompt tokens).
         * @param presence_penalty Number between -2.0 and 2.0. Positive values penalize
         *                         new tokens based on whether they appear in the text so
         *                         far, increasing the model's likelihood to talk about
         *                         new topics.
         * @param frequency_penalty Number between -2.0 and 2.0. Positive values penalize
         *                          new tokens based on their existing frequency in the
         *                          text so far, decreasing the model's likelihood to
         *                          repeat the same line verbatim.
         * @param logit_bias       Modify the likelihood of specified tokens appearing
         *                         in the completion.
         * @param user             The user ID to associate with the request. This is
         *                         used to prevent abuse of the API.
         *
         * @return A liboai::Response future containing the data in JSON format.
         */
        [[nodiscard]]
        auto CreateAsync(
            const std::string& model,
            Conversation& conversation,
            std::optional<std::string> function_call = std::nullopt,
            std::optional<float> temperature = std::nullopt,
            std::optional<float> top_p = std::nullopt,
            std::optional<uint16_t> n = std::nullopt,
            std::optional<ChatStreamCallback> stream = std::nullopt,
            std::optional<std::vector<std::string>> stop = std::nullopt,
            std::optional<uint16_t> max_tokens = std::nullopt,
            std::optional<float> presence_penalty = std::nullopt,
            std::optional<float> frequency_penalty = std::nullopt,
            std::optional<std::unordered_map<std::string, int8_t>> logit_bias = std::nullopt,
            std::optional<std::string> user = std::nullopt
        ) const& noexcept -> FutureExpected<Response>;

    private:
        Authorization& m_auth = Authorization::Authorizer();
        using StrippedStreamCallback = std::function<bool(std::string, intptr_t)>;
    };

    // Conversation method implementations
    inline Conversation::Conversation(const Conversation& other)
        : m_conversation(other.m_conversation),
          m_functions(other.m_functions),
          m_last_resp_is_fc(other.m_last_resp_is_fc) {}

    inline Conversation::Conversation()
        : m_conversation(nlohmann::json::object()),
          m_functions(nlohmann::json::object()),
          m_last_resp_is_fc(false) {}

    Conversation::Conversation(Conversation&& old) noexcept
        : m_conversation(std::move(old.m_conversation)),
          m_functions(std::move(old.m_functions)),
          m_last_resp_is_fc(old.m_last_resp_is_fc) {
        old.m_conversation = nlohmann::json::object();
        old.m_functions = nlohmann::json::object();
    }

    Conversation::Conversation(std::string_view system_data) {
        this->m_conversation["messages"] = nlohmann::json::array();
        auto result = this->SetSystemData(system_data);
    }

    Conversation::Conversation(std::string_view system_data, std::string_view user_data) {
        this->m_conversation["messages"] = nlohmann::json::array();
        auto result = this->SetSystemData(system_data);
        result = this->AddUserData(user_data);
    }

    Conversation::Conversation(
        std::string_view system_data,
        std::initializer_list<std::string_view> user_data
    ) {
        this->m_conversation["messages"] = nlohmann::json::array();
        auto result = this->SetSystemData(system_data);

        for (const auto& data : user_data) {
            auto result = this->AddUserData(data);
        }
    }

    Conversation::Conversation(std::initializer_list<std::string_view> user_data) {
        this->m_conversation["messages"] = nlohmann::json::array();

        for (const auto& data : user_data) {
            auto result = this->AddUserData(data);
        }
    }

    Conversation::Conversation(const std::vector<std::string>& user_data) {
        this->m_conversation["messages"] = nlohmann::json::array();

        for (const auto& data : user_data) {
            auto result = this->AddUserData(data);
        }
    }

    auto Conversation::operator=(const Conversation& other) -> Conversation& {
        if (this != &other) {
            this->m_conversation = other.m_conversation;
            this->m_functions = other.m_functions;
            this->m_last_resp_is_fc = other.m_last_resp_is_fc;
        }
        return *this;
    }

    auto Conversation::operator=(Conversation&& old) noexcept -> Conversation& {
        this->m_conversation = std::move(old.m_conversation);
        this->m_functions = std::move(old.m_functions);
        this->m_last_resp_is_fc = old.m_last_resp_is_fc;

        old.m_conversation = nlohmann::json::object();
        old.m_functions = nlohmann::json::object();

        return *this;
    }

    auto Conversation::ChangeFirstSystemMessage(std::string_view new_data) & noexcept
        -> Result<bool> {
        if (!new_data.empty() && !this->m_conversation["messages"].empty()) {
            if (this->m_conversation["messages"][0]["role"].get<std::string>() == "system") {
                this->m_conversation["messages"][0]["content"] = new_data;
                return true; // System message changed successfuly
            }
            return false;    // First message is not a system message
        }
        return false;        // New data is empty or conversation is empty
    }

    auto Conversation::SetSystemData(std::string_view data) & noexcept -> Result<bool> {
        // if data provided is non-empty
        if (!data.empty()) {
            // if system is not set already - only one system message shall exist in any
            // conversation
            for (const auto& message : this->m_conversation["messages"].items()) {
                if (message.value()["role"].get<std::string>() == "system") {
                    return false; // system already set
                }
            }
            this->m_conversation["messages"].push_back(
                {
                    {    "role", "system" },
                    { "content",     data }
            }
            );
            return true; // system set successfully
        }
        return false;    // data is empty
    }

    auto Conversation::PopSystemData() & noexcept -> Result<bool> {
        // if conversation is non-empty
        if (!this->m_conversation["messages"].empty()) {
            // if first message is system
            if (this->m_conversation["messages"][0]["role"].get<std::string>() == "system") {
                this->m_conversation["messages"].erase(0);
                return true; // system message popped successfully
            }
            return false;    // first message is not system
        }
        return false;        // conversation is empty
    }

    auto Conversation::EraseExtra() -> void {
        if (m_conversation["messages"].size() > m_max_history_size) {
            // Ensure the system message is preserved
            auto first_msg = m_conversation["messages"].begin();
            if (first_msg != m_conversation["messages"].end() &&
                (*first_msg)["role"].get<std::string>() == "system") {
                m_conversation["messages"].erase(first_msg + 1);
            } else {
                m_conversation["messages"].erase(first_msg);
            }
        }
    }

    auto Conversation::AddUserData(std::string_view data) & noexcept -> Result<bool> {
        // if data provided is non-empty
        if (!data.empty()) {
            EraseExtra();
            this->m_conversation["messages"].push_back(
                {
                    {    "role", "user" },
                    { "content",   data }
            }
            );
            return true; // user data added successfully
        }
        return false;    // data is empty
    }

    auto Conversation::AddUserData(std::string_view data, std::string_view name) & noexcept
        -> Result<bool> {
        // if data provided is non-empty
        if (!data.empty()) {
            EraseExtra();
            this->m_conversation["messages"].push_back(
                {
                    {    "role", "user" },
                    { "content",   data },
                    {    "name",   name }
            }
            );
            return true; // user data added successfully
        }
        return false;    // data is empty
    }

    auto Conversation::PopUserData() & noexcept -> Result<bool> {
        // if conversation is not empty
        if (!this->m_conversation["messages"].empty()) {
            // if last message is user message
            if (this->m_conversation["messages"].back()["role"].get<std::string>() == "user") {
                this->m_conversation["messages"].erase(this->m_conversation["messages"].end() - 1);
                return true; // user data popped successfully
            }
            return false;    // last message is not user message
        }
        return false;        // conversation is empty
    }

    auto Conversation::GetLastResponse() const& noexcept -> Result<std::string> {
        // if conversation is not empty
        if (!this->m_conversation["messages"].empty()) {
            // if last message is from assistant
            if (this->m_conversation["messages"].back()["role"].get<std::string>() == "assistant") {
                return this->m_conversation["messages"].back()["content"].get<std::string>();
            }
        }
        return ""; // no response found
    }

    auto Conversation::LastResponseIsFunctionCall() const& noexcept -> Result<bool> {
        return this->m_last_resp_is_fc;
    }

    auto Conversation::GetLastFunctionCallName() const& noexcept -> Result<std::string> {
        if (this->m_conversation.contains("function_call")) {
            if (this->m_conversation["function_call"].contains("name")) {
                return this->m_conversation["function_call"]["name"].get<std::string>();
            }
        }

        return "";
    }

    auto Conversation::GetLastFunctionCallArguments() const& noexcept -> Result<std::string> {
        if (this->m_conversation.contains("function_call")) {
            if (this->m_conversation["function_call"].contains("arguments")) {
                return this->m_conversation["function_call"]["arguments"].get<std::string>();
            }
        }

        return "";
    }

    auto Conversation::PopLastResponse() & noexcept -> Result<bool> {
        // if conversation is not empty
        if (!this->m_conversation["messages"].empty()) {
            // if last message is assistant message
            if (this->m_conversation["messages"].back()["role"].get<std::string>() == "assistant") {
                this->m_conversation["messages"].erase(this->m_conversation["messages"].end() - 1);
                return true; // assistant data popped successfully
            }
            return false;    // last message is not assistant message
        }
        return false;        // conversation is empty
    }

    auto Conversation::Update(std::string_view history) & noexcept -> Result<bool> {
        // reset "last response is function call" flag
        if (this->m_last_resp_is_fc) {
            if (this->m_conversation.contains("function_call")) {
                this->m_conversation.erase("function_call");
            }
            this->m_last_resp_is_fc = false;
        }

        // if history is non-empty
        if (!history.empty()) {
            nlohmann::json j;
            try {
                j = nlohmann::json::parse(history);
            } catch (const nlohmann::json::parse_error& e) {
                return std::unexpected(OpenAIError::parse_error(e.what()));
            } catch (const std::exception& e) {
                return std::unexpected(OpenAIError::parse_error(e.what()));
            } catch (...) {
                return std::unexpected(OpenAIError::parse_error("Unknown JSON parse error"));
            }
            if (j.contains("choices")) { // top level, several messages
                for (const auto& choice : j["choices"].items()) {
                    if (choice.value().contains("message")) {
                        if (choice.value()["message"].contains("role") &&
                            choice.value()["message"].contains("content")) {
                            if (!choice.value()["message"]["content"].is_null()) {
                                EraseExtra();
                                this->m_conversation["messages"].push_back(
                                    {
                                        {    "role",    choice.value()["message"]["role"] },
                                        { "content", choice.value()["message"]["content"] }
                                }
                                );
                            } else {
                                EraseExtra();
                                this->m_conversation["messages"].push_back(
                                    {
                                        {    "role", choice.value()["message"]["role"] },
                                        { "content",                                "" }
                                }
                                );
                            }

                            if (choice.value()["message"].contains("function_call")) {
                                // if a function_call is present in the response, the
                                // conversation is not updated as there is no assistant
                                // response to be added. However, we do add the function
                                // information

                                this->m_conversation["function_call"] = nlohmann::json::object();
                                if (choice.value()["message"]["function_call"].contains("name")) {
                                    this->m_conversation["function_call"]["name"] =
                                        choice.value()["message"]["function_call"]["name"];
                                }
                                if (choice.value()["message"]["function_call"].contains(
                                        "arguments"
                                    )) {
                                    this->m_conversation["function_call"]["arguments"] =
                                        choice.value()["message"]["function_call"]["arguments"];
                                }

                                this->m_last_resp_is_fc = true;
                            }

                            return true;        // conversation updated successfully
                        }
                        return false;           // response is not valid
                    }
                    return false;               // no response found
                }
            } else if (j.contains("message")) { // mid level, single message
                if (j["message"].contains("role") && j["message"].contains("content")) {
                    if (!j["message"]["content"].is_null()) {
                        EraseExtra();
                        this->m_conversation["messages"].push_back(
                            {
                                {    "role",    j["message"]["role"] },
                                { "content", j["message"]["content"] }
                        }
                        );
                    } else {
                        EraseExtra();
                        this->m_conversation["messages"].push_back(
                            {
                                {    "role", j["message"]["role"] },
                                { "content",                   "" }
                        }
                        );
                    }

                    if (j["message"].contains("function_call")) {
                        // if a function_call is present in the response, the
                        // conversation is not updated as there is no assistant
                        // response to be added. However, we do add the function
                        // information

                        this->m_conversation["function_call"] = nlohmann::json::object();
                        if (j["message"]["function_call"].contains("name")) {
                            this->m_conversation["function_call"]["name"] =
                                j["message"]["function_call"]["name"];
                        }
                        if (j["message"]["function_call"].contains("arguments")) {
                            this->m_conversation["function_call"]["arguments"] =
                                j["message"]["function_call"]["arguments"];
                        }

                        this->m_last_resp_is_fc = true;
                    }

                    return true;                                      // conversation updated successfully
                }
                return false;                                         // response is not valid
            } else if (j.contains("role") && j.contains("content")) { // low level, single message
                if (!j["content"].is_null()) {
                    EraseExtra();
                    this->m_conversation["messages"].push_back(
                        {
                            {    "role",    j["role"] },
                            { "content", j["content"] }
                    }
                    );
                } else {
                    EraseExtra();
                    this->m_conversation["messages"].push_back(
                        {
                            {    "role", j["role"] },
                            { "content",                   "" }
                    }
                    );
                }

                if (j.contains("function_call")) {
                    // if a function_call is present in the response, the
                    // conversation is not updated as there is no assistant
                    // response to be added. However, we do add the function
                    // information
                    this->m_conversation["function_call"] = nlohmann::json::object();
                    if (j["function_call"].contains("name")) {
                        this->m_conversation["function_call"]["name"] =
                            j["function_call"]["name"];
                    }
                    if (j["function_call"].contains("arguments")) {
                        this->m_conversation["function_call"]["arguments"] =
                            j["function_call"]["arguments"];
                    }

                    this->m_last_resp_is_fc = true;
                }

                return true;  // conversation updated successfully
            } else {
                return false; // invalid response
            }
        }
        return false; // response is empty
    }

    auto Conversation::Update(const Response& response) & noexcept -> Result<bool> {
        return this->Update(response.content);
    }

    auto Conversation::Export() const& noexcept -> Result<std::string> {
        nlohmann::json j;

        if (!this->m_conversation.empty()) {
            j["messages"] = this->m_conversation["messages"];

            if (this->m_functions) {
                j["functions"] = this->m_functions.value()["functions"];
            }

            return j.dump(4); // conversation exported successfully
        }

        return ""; // conversation is empty
    }

    auto Conversation::Import(std::string_view json) & noexcept -> Result<bool> {
        if (!json.empty()) {
            nlohmann::json j = nlohmann::json::parse(json);

            if (j.contains("messages")) {
                this->m_conversation["messages"] = j["messages"];

                if (j.contains("functions")) {
                    this->m_functions = nlohmann::json();
                    this->m_functions.value()["functions"] = j["functions"];
                }

                return true; // conversation imported successfully
            }

            return false; // no messages found
        }

        return false; // json is empty
    }

    auto Conversation::AppendStreamData(const std::string& data) & noexcept -> Result<bool> {
        if (!data.empty()) {
            std::string delta;
            bool completed = false;
            return this->ParseStreamData(data, delta, completed);
        }

        return false; // data is empty
    }

    auto Conversation::AppendStreamData(
        const std::string& data,
        std::string& delta,
        bool& completed
    ) & noexcept -> Result<bool> {
        if (!data.empty()) {
            return this->ParseStreamData(data, delta, completed);
        }

        return false;
    }

    auto Conversation::SetFunctions(const Functions& functions) & noexcept -> Result<bool> {
        nlohmann::json j = functions.GetJSON();

        if (!j.empty() && j.contains("functions") && j["functions"].size() > 0) {
            this->m_functions = std::move(j);
            return true; // functions set successfully
        }

        return false; // functions are empty
    }

    auto Conversation::PopFunctions() & noexcept -> void {
        this->m_functions = std::nullopt;
    }

    auto Conversation::GetRawConversation() const& noexcept -> Result<std::string> {
        return this->m_conversation.dump(4);
    }

    auto Conversation::GetJSON() const& noexcept -> const nlohmann::json& {
        return this->m_conversation;
    }

    auto Conversation::GetRawFunctions() const& noexcept -> Result<std::string> {
        return this->HasFunctions() ? this->m_functions.value().dump(4) : "";
    }

    auto Conversation::GetFunctionsJSON() const& noexcept -> const nlohmann::json& {
        return this->m_functions.value();
    }

    auto Conversation::SplitStreamedData(std::string data) const noexcept
        -> Result<std::vector<std::string>> {
        // remove all instances of the string "data: " from the string
        this->RemoveStrings(data, "data: ");

        /*
            Splits the streamed data into a vector of strings
            via delimiter of two newlines.

            For instance, a string of "Hello\n\nWorld" would
            be split into a vector of {"Hello", "World"}, and
            a string of "Hello World" would be split into
            a vector of {"Hello World"}.
        */
        if (!data.empty()) {
            return SplitNonEmptyLines(data);
        }

        return {};
    }

    auto Conversation::RemoveStrings(std::string& s, std::string_view p) const noexcept -> void {
        std::string::size_type i = s.find(p);
        while (i != std::string::npos) {
            s.erase(i, p.length());
            i = s.find(p, i);
        }
    }

    auto Conversation::SplitFullStreamedData(const std::string& data) const noexcept
        -> Result<std::vector<std::string>> {
        if (data.empty()) {
            return {};
        }

        return SplitNonEmptyLines(data);
    }

    auto Conversation::ParseStreamData(
        std::string data,
        std::string& delta_content,
        bool& completed
    ) noexcept -> Result<bool> {
        if (!m_last_incomplete_buffer.empty()) {
            data = m_last_incomplete_buffer + data;
            m_last_incomplete_buffer.clear();
        }

        auto lines_result = SplitFullStreamedData(data);
        if (!lines_result) {
            return std::unexpected(lines_result.error());
        }
        std::vector<std::string> data_lines = std::move(*lines_result);

        if (data_lines.empty()) {
            return false;
        }

        // create an empty message at the end of the conversation,
        // marked as "pending" to indicate that the response is
        // still being processed. This flag will be removed once
        // the response is processed. If the marking already
        // exists, keep appending to the same message.
        if (this->m_conversation["messages"].empty() ||
            !this->m_conversation["messages"].back().contains("pending")) {
            this->m_conversation["messages"].push_back(
                {
                    {    "role",   "" },
                    { "content",   "" },
                    { "pending", true }
            }
            );
        }

        for (auto& line : data_lines) {
            if (line.find("data: [DONE]") == std::string::npos) {
                /*
                    j should have content in the form of:
                        {"id":"chatcmpl-7SKOck29emvbBbDS6cHg5xwnRrsLO","object":"chat.completion.chunk","created":1686985942,"model":"gpt-3.5-turbo-0613","choices":[{"index":0,"delta":{"content":"."},"finish_reason":null}]}
                    where "delta" may be empty
                */
                this->RemoveStrings(line, "data: ");

                nlohmann::json j;
                try {
                    j = nlohmann::json::parse(line);
                } catch (const std::exception& e) {
                    m_last_incomplete_buffer = line;
                    continue;
                }

                if (j.contains("choices")) {
                    if (j["choices"][0].contains("delta")) {
                        if (!j["choices"][0]["delta"].empty() &&
                            !j["choices"][0]["delta"].is_null()) {
                            if (j["choices"][0]["delta"].contains("role")) {
                                this->m_conversation["messages"].back()["role"] =
                                    j["choices"][0]["delta"]["role"];
                            }

                            if (j["choices"][0]["delta"].contains("content")) {
                                if (!j["choices"][0]["delta"]["content"].empty() &&
                                    !j["choices"][0]["delta"]["content"].is_null()) {
                                    std::string stream_content =
                                        j["choices"][0]["delta"]["content"].get<std::string>();
                                    this->m_conversation["messages"].back()["content"] =
                                        this->m_conversation["messages"]
                                            .back()["content"]
                                            .get<std::string>() +
                                        stream_content;
                                    delta_content += stream_content;
                                }

                                // function calls do not have a content field,
                                // set m_last_resp_is_fc to false and remove any
                                // previously set function_call field in the
                                // conversation
                                if (this->m_last_resp_is_fc) {
                                    if (this->m_conversation.contains("function_call")) {
                                        this->m_conversation.erase("function_call");
                                    }
                                    this->m_last_resp_is_fc = false;
                                }
                            }

                            if (j["choices"][0]["delta"].contains("function_call")) {
                                if (!j["choices"][0]["delta"]["function_call"].empty() &&
                                    !j["choices"][0]["delta"]["function_call"].is_null()) {
                                    if (j["choices"][0]["delta"]["function_call"].contains(
                                            "name"
                                        )) {
                                        if (!j["choices"][0]["delta"]["function_call"]["name"]
                                                 .empty() &&
                                            !j["choices"][0]["delta"]["function_call"]["name"]
                                                 .is_null()) {
                                            if (!this->m_conversation["messages"].back().contains(
                                                    "function_call"
                                                )) {
                                                this->m_conversation["function_call"] = {
                                                    { "name",
                                                     j["choices"][0]["delta"]["function_call"]
                                                       ["name"] }
                                                };
                                                this->m_last_resp_is_fc = true;
                                            }
                                        }
                                    } else if (j["choices"][0]["delta"]["function_call"].contains(
                                                   "arguments"
                                               )) {
                                        if (!j["choices"][0]["delta"]["function_call"]["arguments"]
                                                 .empty() &&
                                            !j["choices"][0]["delta"]["function_call"]["arguments"]
                                                 .is_null()) {
                                            if (!this->m_conversation["function_call"].contains(
                                                    "arguments"
                                                )) {
                                                this->m_conversation["function_call"].push_back(
                                                    { "arguments",
                                                      j["choices"][0]["delta"]["function_call"]
                                                       ["arguments"] }
                                                );
                                            } else {
                                                this->m_conversation["function_call"]["arguments"] =
                                                    this->m_conversation["function_call"]
                                                                        ["arguments"]
                                                                            .get<std::string>() +
                                                    j["choices"][0]["delta"]["function_call"]
                                                     ["arguments"]
                                                         .get<std::string>();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    return false; // no "choices" found - invalid
                }
            } else {
                // the response is complete, erase the "pending" flag
                this->m_conversation["messages"].back().erase("pending");
                completed = true;
            }
        }

        return true; // last message received
    }

    auto ChatCompletion::Create(
        const std::string& model,
        Conversation& conversation,
        std::optional<std::string> function_call,
        std::optional<float> temperature,
        std::optional<float> top_p,
        std::optional<uint16_t> n,
        std::optional<ChatStreamCallback> stream,
        std::optional<std::vector<std::string>> stop,
        std::optional<uint16_t> max_tokens,
        std::optional<float> presence_penalty,
        std::optional<float> frequency_penalty,
        std::optional<std::unordered_map<std::string, int8_t>> logit_bias,
        std::optional<std::string> user
    ) const& noexcept -> Result<Response> {
        JsonConstructor jcon;
        jcon.push_back("model", model);
        jcon.push_back("temperature", temperature);
        jcon.push_back("top_p", top_p);
        jcon.push_back("n", n);
        jcon.push_back("stop", std::move(stop));
        jcon.push_back("max_tokens", max_tokens);
        jcon.push_back("presence_penalty", presence_penalty);
        jcon.push_back("frequency_penalty", frequency_penalty);
        jcon.push_back("logit_bias", std::move(logit_bias));
        jcon.push_back("user", std::move(user));

        if (function_call) {
            if (function_call.value() == "none" || function_call.value() == "auto") {
                nlohmann::json j;
                j["function_call"] = function_call.value();
                jcon.push_back("function_call", j["function_call"]);
            } else {
                nlohmann::json j;
                j["function_call"] = {
                    { "name", function_call.value() }
                };
                jcon.push_back("function_call", j["function_call"]);
            }
        }

        StrippedStreamCallback _sscb = nullptr;
        if (stream) {
            _sscb = [stream, &conversation](std::string data, intptr_t userdata) -> bool {
                ChatStreamCallback _stream = stream.value();
                return _stream(data, userdata, conversation);
            };

            jcon.push_back("stream", _sscb);
        }

        if (conversation.GetJSON().contains("messages")) {
            jcon.push_back("messages", conversation.GetJSON()["messages"]);
        }

        if (conversation.HasFunctions()) {
            jcon.push_back("functions", conversation.GetFunctionsJSON()["functions"]);
        }

        return this->Request(
            Method::HTTP_POST,
            this->GetOpenAIRoot(),
            "/chat/completions",
            "application/json",
            this->m_auth.GetAuthorizationHeaders(),
            cpr::Body{ jcon.dump() },
            _sscb ?
                cpr::WriteCallback{
                    [cb = std::move(_sscb)](std::string_view data, intptr_t userdata) -> bool {
                        return cb(std::string(data), userdata);
                    } } :
                cpr::WriteCallback{},
            this->m_auth.GetProxies(),
            this->m_auth.GetProxyAuth(),
            this->m_auth.GetMaxTimeout()
        );
    }

    auto ChatCompletion::CreateAsync(
        const std::string& model,
        Conversation& conversation,
        std::optional<std::string> function_call,
        std::optional<float> temperature,
        std::optional<float> top_p,
        std::optional<uint16_t> n,
        std::optional<ChatStreamCallback> stream,
        std::optional<std::vector<std::string>> stop,
        std::optional<uint16_t> max_tokens,
        std::optional<float> presence_penalty,
        std::optional<float> frequency_penalty,
        std::optional<std::unordered_map<std::string, int8_t>> logit_bias,
        std::optional<std::string> user
    ) const& noexcept -> FutureExpected<Response> {
        return std::async(
            std::launch::async,
            &ChatCompletion::Create,
            this,
            model,
            std::ref(conversation),
            function_call,
            temperature,
            top_p,
            n,
            stream,
            stop,
            max_tokens,
            presence_penalty,
            frequency_penalty,
            logit_bias,
            user
        );
    }

    auto operator<<(std::ostream& os, const Conversation& conv) -> std::ostream& {
        auto raw_conv = conv.GetRawConversation();
        os << (raw_conv ? *raw_conv : "") << std::endl;

        if (conv.HasFunctions()) {
            auto raw_funcs = conv.GetRawFunctions();
            os << (raw_funcs ? *raw_funcs : "");
        }

        return os;
    }

    Functions::Functions() {
        this->m_functions["functions"] = nlohmann::json::array();
    }

    Functions::Functions(const Functions& other) {
        this->m_functions = other.m_functions;
    }

    Functions::Functions(Functions&& old) noexcept {
        this->m_functions = std::move(old.m_functions);
        old.m_functions = nlohmann::json::object();
    }

    auto Functions::operator=(const Functions& other) -> Functions& {
        if (this != &other) {
            this->m_functions = other.m_functions;
        }
        return *this;
    }

    auto Functions::operator=(Functions&& old) noexcept -> Functions& {
        this->m_functions = std::move(old.m_functions);
        old.m_functions = nlohmann::json::object();
        return *this;
    }

    auto Functions::AddFunction(std::string_view function_name) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(function_name);
        if (!idx_result) {
            this->m_functions["functions"].push_back(
                {
                    { "name", function_name }
            }
            );
            return true; // function added
        }
        return false;    // function already exists
    }

    auto Functions::AddFunctions(std::initializer_list<std::string_view> function_names) & noexcept
        -> Result<bool> {
        if (function_names.size() > 0) {
            for (const auto& function_name : function_names) {
                auto idx_result = this->GetFunctionIndex(function_name);
                if (!idx_result) {
                    this->m_functions["functions"].push_back(
                        {
                            { "name", function_name }
                    }
                    );
                }
            }
            return true; // functions added
        }
        return false;    // functions not added (size 0)
    }

    auto Functions::AddFunctions(std::vector<std::string> function_names) & noexcept
        -> Result<bool> {
        if (function_names.size() > 0) {
            for (auto& function_name : function_names) {
                auto idx_result = this->GetFunctionIndex(function_name);
                if (!idx_result) {
                    this->m_functions["functions"].push_back(
                        {
                            { "name", std::move(function_name) }
                    }
                    );
                }
            }
            return true; // functions added
        }
        return false;    // functions not added (size 0)
    }

    auto Functions::PopFunction(std::string_view function_name) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(function_name);

        if (idx_result) {
            this->m_functions["functions"].erase(
                this->m_functions["functions"].begin() + static_cast<std::ptrdiff_t>(*idx_result)
            );
            return true; // function removed
        }

        return false; // function not removed
    }

    auto Functions::PopFunctions(std::initializer_list<std::string_view> function_names) & noexcept
        -> Result<bool> {
        if (function_names.size() > 0) {
            for (const auto& function_name : function_names) {
                auto idx_result = this->GetFunctionIndex(function_name);

                if (idx_result) {
                    this->m_functions["functions"].erase(
                        this->m_functions["functions"].begin() +
                        static_cast<std::ptrdiff_t>(*idx_result)
                    );
                }
            }

            return true; // functions removed
        }

        return false; // functions not removed (size 0)
    }

    auto Functions::PopFunctions(std::vector<std::string> function_names) & noexcept
        -> Result<bool> {
        if (function_names.size() > 0) {
            for (const auto& function_name : function_names) {
                auto idx_result = this->GetFunctionIndex(function_name);

                if (idx_result) {
                    this->m_functions["functions"].erase(
                        this->m_functions["functions"].begin() +
                        static_cast<std::ptrdiff_t>(*idx_result)
                    );
                }
            }
            return true; // functions removed
        }

        return false; // functions not removed (size 0)
    }

    auto Functions::SetDescription(std::string_view target, std::string_view description) & noexcept
        -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (!this->m_functions["functions"][i].contains("description")) {
                this->m_functions["functions"][i]["description"] = description;
                return true; // description set successfully
            }
            return false;    // already has a description
        }

        return false; // function does not exist
    }

    auto Functions::PopDescription(std::string_view target) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("description")) {
                this->m_functions["functions"][i].erase("description");
                return true; // description removed successfully
            }
            return false;    // does not have a description
        }

        return false; // function does not exist
    }

    auto Functions::SetRequired(
        std::string_view target,
        std::initializer_list<std::string_view> params
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result && params.size() > 0) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                for (const auto& parameter : params) {
                    this->m_functions["functions"][i]["parameters"]["required"] = params;
                    return true; // required parameters set successfully
                }
            }
        }

        return false; // required parameters not set
    }

    auto Functions::SetRequired(std::string_view target, std::vector<std::string> params) & noexcept
        -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result && params.size() > 0) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                for (const auto& parameter : params) {
                    this->m_functions["functions"][i]["parameters"]["required"] = std::move(params);
                    return true; // required parameters set successfully
                }
            }
        }

        return false; // required parameters not set
    }

    auto Functions::PopRequired(std::string_view target) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                if (this->m_functions["functions"][i]["parameters"].contains("required")) {
                    this->m_functions["functions"][i]["parameters"].erase("required");
                    return true; // required parameters removed successfully
                }
            }
        }

        return false; // required parameters not removed
    }

    auto Functions::AppendRequired(std::string_view target, std::string_view param) & noexcept
        -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                if (this->m_functions["functions"][i]["parameters"].contains("required")) {
                    this->m_functions["functions"][i]["parameters"]["required"].push_back(param);
                    return true; // required parameter appended successfully
                }
            }
        }

        return false; // required parameter not appended
    }

    auto Functions::AppendRequired(
        std::string_view target,
        std::initializer_list<std::string_view> params
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result && params.size() > 0) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                if (this->m_functions["functions"][i]["parameters"].contains("required")) {
                    for (const auto& param : params) {
                        this->m_functions["functions"][i]["parameters"]["required"].push_back(
                            param
                        );
                    }

                    return true; // required parameters appended successfully
                }
            }
        }

        return false; // required parameters not appended
    }

    auto
    Functions::AppendRequired(std::string_view target, std::vector<std::string> params) & noexcept
        -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result && params.size() > 0) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                if (this->m_functions["functions"][i]["parameters"].contains("required")) {
                    for (auto& param : params) {
                        this->m_functions["functions"][i]["parameters"]["required"].push_back(
                            std::move(param)
                        );
                    }

                    return true; // required parameters appended successfully
                }
            }
        }

        return false; // required parameters not appended
    }

    auto Functions::SetParameter(std::string_view target, FunctionParameter parameter) & noexcept
        -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (!this->m_functions["functions"][i].contains("parameters")) {
                this->m_functions["functions"][i]["parameters"] = nlohmann::json::object();
                this->m_functions["functions"][i]["parameters"]["properties"] =
                    nlohmann::json::object();
                this->m_functions["functions"][i]["parameters"]["type"] = "object";

                this->m_functions["functions"][i]["parameters"]["properties"].push_back(
                    {
                        parameter.name,
                        { { "type", std::move(parameter.type) },
                               { "description", std::move(parameter.description) } }
                }
                );

                if (parameter.enumeration) {
                    this->m_functions["functions"][i]["parameters"]["properties"][parameter.name]
                                     ["enum"] = std::move(parameter.enumeration.value());
                }

                return true; // parameter set successfully
            }
        }

        return false; // function non-existent, or parameters already set (use AppendParameter(s))
    }

    auto Functions::SetParameters(
        std::string_view target,
        std::initializer_list<FunctionParameter> parameters
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (!this->m_functions["functions"][i].contains("parameters") &&
                parameters.size() > 0) {
                this->m_functions["functions"][i]["parameters"] = nlohmann::json::object();
                this->m_functions["functions"][i]["parameters"]["properties"] =
                    nlohmann::json::object();
                this->m_functions["functions"][i]["parameters"]["type"] = "object";

                for (const auto& parameter : parameters) {
                    if (!this->m_functions["functions"][i]["parameters"]["properties"].contains(
                            parameter.name
                        )) {
                        this->m_functions["functions"][i]["parameters"]["properties"].push_back(
                            {
                                parameter.name,
                                { { "type", parameter.type },
                                       { "description", parameter.description } }
                        }
                        );

                        if (parameter.enumeration) {
                            this->m_functions["functions"][i]["parameters"]["properties"]
                                             [parameter.name]["enum"] =
                                parameter.enumeration.value();
                        }
                    }
                }

                return true; // parameter set successfully
            }
        }

        return false; // function non-existent, or parameters already set (use AppendParameter(s))
    }

    auto Functions::SetParameters(
        std::string_view target,
        std::vector<FunctionParameter> parameters
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (!this->m_functions["functions"][i].contains("parameters") &&
                parameters.size() > 0) {
                this->m_functions["functions"][i]["parameters"] = nlohmann::json::object();
                this->m_functions["functions"][i]["parameters"]["properties"] =
                    nlohmann::json::object();
                this->m_functions["functions"][i]["parameters"]["type"] = "object";

                for (auto& parameter : parameters) {
                    if (!this->m_functions["functions"][i]["parameters"]["properties"].contains(
                            parameter.name
                        )) {
                        this->m_functions["functions"][i]["parameters"]["properties"].push_back(
                            {
                                parameter.name,
                                { { "type", std::move(parameter.type) },
                                       { "description", std::move(parameter.description) } }
                        }
                        );

                        if (parameter.enumeration) {
                            this->m_functions["functions"][i]["parameters"]["properties"]
                                             [parameter.name]["enum"] =
                                std::move(parameter.enumeration.value());
                        }
                    }
                }

                return true; // parameter set successfully
            }
        }

        return false; // function non-existent, or parameters already set (use AppendParameter(s))
    }

    auto Functions::PopParameters(std::string_view target) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                this->m_functions["functions"][i].erase("parameters");
                return true; // parameters removed successfully
            }
        }

        return false; // parameters not removed
    }

    auto Functions::PopParameters(
        std::string_view target,
        std::initializer_list<std::string_view> param_names
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                for (const auto& param_name : param_names) {
                    if (this->m_functions["functions"][i]["parameters"]["properties"].contains(
                            param_name
                        )) {
                        this->m_functions["functions"][i]["parameters"]["properties"].erase(
                            param_name
                        );
                    }
                }

                return true; // parameters removed successfully
            }
        }

        return false; // parameters not removed
    }

    auto Functions::PopParameters(
        std::string_view target,
        std::vector<std::string> param_names
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                for (const auto& param_name : param_names) {
                    if (this->m_functions["functions"][i]["parameters"]["properties"].contains(
                            param_name
                        )) {
                        this->m_functions["functions"][i]["parameters"]["properties"].erase(
                            param_name
                        );
                    }
                }

                return true; // parameters removed successfully
            }
        }

        return false; // parameters not removed
    }

    auto Functions::AppendParameter(std::string_view target, FunctionParameter parameter) & noexcept
        -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                if (!this->m_functions["functions"][i]["parameters"]["properties"].contains(
                        parameter.name
                    )) {
                    this->m_functions["functions"][i]["parameters"]["properties"].push_back(
                        {
                            parameter.name,
                            { { "type", std::move(parameter.type) },
                                   { "description", std::move(parameter.description) } }
                    }
                    );

                    if (parameter.enumeration) {
                        this->m_functions["functions"][i]["parameters"]["properties"]
                                         [parameter.name]["enum"] =
                            std::move(parameter.enumeration.value());
                    }

                    return true; // parameter appended successfully
                }
            }
        }

        return false; // parameter not appended
    }

    auto Functions::AppendParameters(
        std::string_view target,
        std::initializer_list<FunctionParameter> parameters
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                for (const auto& parameter : parameters) {
                    if (!this->m_functions["functions"][i]["parameters"]["properties"].contains(
                            parameter.name
                        )) {
                        this->m_functions["functions"][i]["parameters"]["properties"].push_back(
                            {
                                parameter.name,
                                { { "type", parameter.type },
                                       { "description", parameter.description } }
                        }
                        );

                        if (parameter.enumeration) {
                            this->m_functions["functions"][i]["parameters"]["properties"]
                                             [parameter.name]["enum"] =
                                parameter.enumeration.value();
                        }
                    }
                }

                return true; // parameters appended successfully
            }
        }

        return false; // parameters not appended
    }

    auto Functions::AppendParameters(
        std::string_view target,
        std::vector<FunctionParameter> parameters
    ) & noexcept -> Result<bool> {
        auto idx_result = this->GetFunctionIndex(target);

        if (idx_result) {
            index i = *idx_result;
            if (this->m_functions["functions"][i].contains("parameters")) {
                for (auto& parameter : parameters) {
                    if (!this->m_functions["functions"][i]["parameters"]["properties"].contains(
                            parameter.name
                        )) {
                        this->m_functions["functions"][i]["parameters"]["properties"].push_back(
                            {
                                parameter.name,
                                { { "type", std::move(parameter.type) },
                                       { "description", std::move(parameter.description) } }
                        }
                        );

                        if (parameter.enumeration) {
                            this->m_functions["functions"][i]["parameters"]["properties"]
                                             [parameter.name]["enum"] =
                                std::move(parameter.enumeration.value());
                        }
                    }
                }

                return true; // parameters appended successfully
            }
        }

        return false; // parameters not appended
    }

    auto Functions::GetJSON() const& noexcept -> const nlohmann::json& {
        return this->m_functions;
    }

    auto Functions::GetFunctionIndex(std::string_view function_name) const& noexcept
        -> Result<index> {
        index i = 0;

        if (!this->m_functions.empty()) {
            for (const auto& [key, value] : this->m_functions["functions"].items()) {
                if (value.contains("name")) {
                    if (value["name"].get<std::string>() == function_name) {
                        return i;
                    }
                }
                i++;
            }
        }

        return std::unexpected(OpenAIError::parse_error("Function not found"));
    }

} // namespace liboai
