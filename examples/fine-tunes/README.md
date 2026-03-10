<h1>Fine-Tunes</h1>
<p>The <code>FineTunes</code> class is defined in <code>fine_tunes.hpp</code> at <code>liboai::FineTunes</code>, and its interface can ideally be accessed through a <code>liboai::OpenAI</code> object.

This class and its associated <code>liboai::OpenAI</code> interface allow access to the <a href="https://platform.openai.com/docs/api-reference/fine-tunes">Fine-tunes</a> endpoint of the OpenAI API; this endpoint's functionality can be found below.</p>
- Manage fine-tuning jobs to tailor a model to your specific training data.

<br>
<h2>Methods</h2>
<p>This document covers the method(s) located in <code>fine_tunes.hpp</code>. You can find their function signature(s) below.</p>

<h3>Create a Fine-Tune</h3>
<p>Creates a job that fine-tunes a specified model from a given dataset. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>

```cpp
std::expected<liboai::Response, liboai::OpenAIError> Create(
  const std::string& training_file,
  std::optional<std::string> validation_file = std::nullopt,
  std::optional<std::string> model_id = std::nullopt,
  std::optional<uint8_t> n_epochs = std::nullopt,
  std::optional<uint16_t> batch_size = std::nullopt,
  std::optional<float> learning_rate_multiplier = std::nullopt,
  std::optional<float> prompt_loss_weight = std::nullopt,
  std::optional<bool> compute_classification_metrics = std::nullopt,
  std::optional<uint16_t> classification_n_classes = std::nullopt,
  std::optional<std::string> classification_positive_class = std::nullopt,
  std::optional<std::vector<float>> classification_betas = std::nullopt,
  std::optional<std::string> suffix = std::nullopt
) const & noexcept(false);
```

<h3>Create a Fine-Tune (async)</h3>
<p>Asynchronously creates a job that fine-tunes a specified model from a given dataset. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>

```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> CreateAsync(
  const std::string& training_file,
  std::optional<std::string> validation_file = std::nullopt,
  std::optional<std::string> model_id = std::nullopt,
  std::optional<uint8_t> n_epochs = std::nullopt,
  std::optional<uint16_t> batch_size = std::nullopt,
  std::optional<float> learning_rate_multiplier = std::nullopt,
  std::optional<float> prompt_loss_weight = std::nullopt,
  std::optional<bool> compute_classification_metrics = std::nullopt,
  std::optional<uint16_t> classification_n_classes = std::nullopt,
  std::optional<std::string> classification_positive_class = std::nullopt,
  std::optional<std::vector<float>> classification_betas = std::nullopt,
  std::optional<std::string> suffix = std::nullopt
) const & noexcept(false);
```

<h3>List Fine-Tunes</h3>
<p>List your organization's fine-tuning jobs. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>

```cpp
std::expected<liboai::Response, liboai::OpenAIError> List() const & noexcept(false);
```


<h3>List Fine-Tunes (async)</h3>
<p>Asynchronously list your organization's fine-tuning jobs. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>

```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> ListAsync() const & noexcept(false);
```

<h3>Retrieve Fine-Tune</h3>
<p>Gets info about the fine-tune job. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>
  
```cpp
std::expected<liboai::Response, liboai::OpenAIError> Retrieve(
  const std::string& fine_tune_id
) const & noexcept(false);
```

<h3>Retrieve Fine-Tune (async)</h3>
<p>Asynchronously gets info about the fine-tune job. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>
  
```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> RetrieveAsync(
  const std::string& fine_tune_id
) const & noexcept(false);
```

<h3>Cancel Fine-Tune</h3>
<p>Immediately cancel a fine-tune job. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>
  
```cpp
std::expected<liboai::Response, liboai::OpenAIError> Cancel(
  const std::string& fine_tune_id
) const & noexcept(false);
```

<h3>Cancel Fine-Tune (async)</h3>
<p>Asynchronously and immediately cancel a fine-tune job. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>
  
```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> CancelAsync(
  const std::string& fine_tune_id
) const & noexcept(false);
```

<h3>List Fine-Tune Events</h3>
<p>Get fine-grained status updates for a fine-tune job. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>
  
```cpp
std::expected<liboai::Response, liboai::OpenAIError> ListEvents(
  const std::string& fine_tune_id,
  std::optional<std::function<bool(std::string, intptr_t)>> stream = std::nullopt
) const & noexcept(false);
```

<h3>List Fine-Tune Events (async)</h3>
<p>Asynchronously get fine-grained status updates for a fine-tune job. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>
  
```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> ListEventsAsync(
  const std::string& fine_tune_id,
  std::optional<std::function<bool(std::string, intptr_t)>> stream = std::nullopt
) const & noexcept(false);
```

<h3>Delete Fine-Tune Model</h3>
<p>Delete a fine-tuned model. You must have the Owner role in your organization. Returns a <code>std::expected&lt;liboai::Response, liboai::OpenAIError&gt;</code> containing response data or error.</p>

```cpp
std::expected<liboai::Response, liboai::OpenAIError> Remove(
  const std::string& model
) const & noexcept(false);
```

<h3>Delete Fine-Tune Model (async)</h3>
<p>Asynchronously delete a fine-tuned model. You must have the Owner role in your organization. Returns a <code>std::expected&lt;liboai::FutureResponse, liboai::OpenAIError&gt;</code> containing future response data or error.</p>

```cpp
std::expected<liboai::FutureResponse, liboai::OpenAIError> RemoveAsync(
  const std::string& model
) const & noexcept(false);
```

<p>All function parameters marked <code>optional</code> are not required and are resolved on OpenAI's end if not supplied.</p>

<br>
<h2>Example Usage</h2>

```cpp
import std;
import liboai;

using namespace liboai;

int main() {
    OpenAI oai;
    if (oai.auth.SetKeyEnv("OPENAI_API_KEY")) {
        auto response = oai.FineTune->Create("file-XGinujblHPwGLSztz8cPS8XY");
        if (response) {
            std::cout << response.value()["events"][0]["message"] << std::endl;
        } else {
            std::cout << response.error().message << std::endl;
        }
    }
}
```

<p>For more example usage of the above function(s), please refer to the <a href="./examples">examples</a> folder.
