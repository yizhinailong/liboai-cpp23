<h1>Completions</h1>
<p>The <code>Completions</code> class is exported from the <code>liboai</code> module at <code>liboai::Completions</code>, and its interface can ideally be accessed through a <code>liboai::OpenAI</code> object.

This class and its associated <code>liboai::OpenAI</code> interface allow access to the <a href="https://platform.openai.com/docs/api-reference/completions">Completions</a> endpoint of the OpenAI API; this endpoint's functionality can be found below.</p>
- Given a prompt, the model will return one or more predicted completions, and can also return the probabilities of alternative tokens at each position.

<br>
<h2>Methods</h2>
<p>This document covers the method(s) exported from the <code>liboai</code> module. You can find their function signature(s) below.</p>

<h3>Create a Completion</h3>
<p>Creates a completion for the provided prompt and parameters. Returns a <code>Result&lt;liboai::Response&gt;</code> containing response data or error information.</p>

```cpp
Result<Response> Create(
  const std::string& model_id,
  std::optional<std::string> prompt = std::nullopt,
  std::optional<std::string> suffix = std::nullopt,
  std::optional<uint16_t> max_tokens = std::nullopt,
  std::optional<float> temperature = std::nullopt,
  std::optional<float> top_p = std::nullopt,
  std::optional<uint16_t> n = std::nullopt,
  std::optional<StreamCallback> stream = std::nullopt,
  std::optional<uint8_t> logprobs = std::nullopt,
  std::optional<bool> echo = std::nullopt,
  std::optional<std::vector<std::string>> stop = std::nullopt,
  std::optional<float> presence_penalty = std::nullopt,
  std::optional<float> frequency_penalty = std::nullopt,
  std::optional<uint16_t> best_of = std::nullopt,
  std::optional<std::unordered_map<std::string, int8_t>> logit_bias = std::nullopt,
  std::optional<std::string> user = std::nullopt
) const& noexcept;
```

<h3>Create a Completion (async)</h3>
<p>Asynchronously creates a completion for the provided prompt and parameters. Returns a <code>FutureExpected&lt;Response&gt;</code> containing future response data or error information.</p>

```cpp
FutureExpected<Response> CreateAsync(
  const std::string& model_id,
  std::optional<std::string> prompt = std::nullopt,
  std::optional<std::string> suffix = std::nullopt,
  std::optional<uint16_t> max_tokens = std::nullopt,
  std::optional<float> temperature = std::nullopt,
  std::optional<float> top_p = std::nullopt,
  std::optional<uint16_t> n = std::nullopt,
  std::optional<StreamCallback> stream = std::nullopt,
  std::optional<uint8_t> logprobs = std::nullopt,
  std::optional<bool> echo = std::nullopt,
  std::optional<std::vector<std::string>> stop = std::nullopt,
  std::optional<float> presence_penalty = std::nullopt,
  std::optional<float> frequency_penalty = std::nullopt,
  std::optional<uint16_t> best_of = std::nullopt,
  std::optional<std::unordered_map<std::string, int8_t>> logit_bias = std::nullopt,
  std::optional<std::string> user = std::nullopt
) const& noexcept;
```

<p>All function parameters marked <code>optional</code> are not required and are resolved on OpenAI's end if not supplied.</p>

<br>
<h2>Example Usage</h2>
<p>For example usage of the above function(s), please refer to the <a href="./examples">examples</a> folder.
