<h1>Models</h1>
<p>The <code>Models</code> class is defined in <code>models.hpp</code> at <code>liboai::Models</code>, and its interface can ideally be accessed through a <code>liboai::OpenAI</code> object.

This class and its associated <code>liboai::OpenAI</code> interface allow access to the <a href="https://platform.openai.com/docs/api-reference/models">Models</a> endpoint of the OpenAI API; this endpoint's functionality can be found below.</p>
- List and describe the various models available in the API. 

<br>
<h2>Methods</h2>
<p>This document covers the method(s) located in <code>models.hpp</code>. You can find their function signature(s) below.</p>

<h3>List Models</h3>
<p>Lists the currently available models, and provides basic information about each one such as the owner and availability. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>

```cpp
std::expected<liboai::Response, liboai::OpenAIError> List() const & noexcept(false);
```

<h3>List Models (async)</h3>
<p>Asynchronously lists the currently available models, and provides basic information about each one such as the owner and availability. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>

```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> ListAsync() const & noexcept(false);
```

<h3>Retrieve Model</h3>
<p>Retrieves a model instance, providing basic information about the model such as the owner and permissioning. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>

```cpp
std::expected<liboai::Response, liboai::OpenAIError> Retrieve(
  const std::string& model
) const & noexcept(false);
```

<h3>Retrieve Model (async)</h3>
<p>Asynchronously retrieves a model instance, providing basic information about the model such as the owner and permissioning. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>

```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> RetrieveAsync(
  const std::string& model
) const & noexcept(false);
```

<br>
<h2>Example Usage</h2>

```cpp
import std;
import liboai;

using namespace liboai;

int main() {
    OpenAI oai;
    if (oai.auth.SetKeyEnv("OPENAI_API_KEY")) {
        auto response = oai.Model->List();
        if (response) {
            std::cout << response.value()["data"] << std::endl;
        } else {
            std::cout << response.error().message << std::endl;
        }
    }
}
```

<p>For more example usage of the above function(s), please refer to the <a href="./examples">examples</a> folder.
