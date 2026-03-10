<h1>Moderations</h1>
<p>The <code>Moderations</code> class is defined in <code>moderations.hpp</code> at <code>liboai::Moderations</code>, and its interface can ideally be accessed through a <code>liboai::OpenAI</code> object.

This class and its associated <code>liboai::OpenAI</code> interface allow access to the <a href="https://platform.openai.com/docs/api-reference/moderations">Moderations</a> endpoint of the OpenAI API; this endpoint's functionality can be found below.</p>
- Given a input text, outputs if the model classifies it as violating OpenAI's content policy.

<br>
<h2>Methods</h2>
<p>This document covers the method(s) located in <code>moderations.hpp</code>. You can find their function signature(s) below.</p>

<h3>Create a Moderation</h3>
<p>Classifies if text violates OpenAI's Content Policy. Returns a <code>std::expected&lt;liboai::Response, liboai::Error&gt;</code> containing response data or an error.</p>

```cpp
auto Create(
  const std::string& input,
  std::optional<std::string> model = std::nullopt
) const & noexcept(false);
```

<h3>Create a Moderation (async)</h3>
<p>Asynchronously classifies if text violates OpenAI's Content Policy. Returns a <code>std::future&lt;std::expected&lt;liboai::Response, liboai::Error&gt;&gt;</code> containing future response data.</p>

```cpp
auto CreateAsync(
  const std::string& input,
  std::optional<std::string> model = std::nullopt
) const & noexcept(false);
```

<p>All function parameters marked <code>optional</code> are not required and are resolved on OpenAI's end if not supplied.</p>

<br>
<h2>Example Usage</h2>

<h3>Synchronous</h3>

```cpp
import std;
import liboai;

using namespace liboai;

int main() {
    OpenAI oai;
    if (oai.auth.SetKeyEnv("OPENAI_API_KEY")) {
        auto response = oai.Moderation->Create("I want to kill them.");
        if (response) {
            std::cout << response.value()["results"][0]["flagged"].get<bool>() << std::endl;
        } else {
            std::cout << response.error().message << std::endl;
        }
    }
}
```

<h3>Asynchronous</h3>

```cpp
import std;
import liboai;

using namespace liboai;

int main() {
    OpenAI oai;
    if (oai.auth.SetKeyEnv("OPENAI_API_KEY")) {
        // call async method; returns a future
        auto fut = oai.Moderation->CreateAsync("I want to kill them.");

        // do other work...

        // check if the future is ready
        fut.wait();

        // get the contained response
        auto response = fut.get();

        // print some response data
        if (response) {
            std::cout << response.value()["results"][0]["flagged"].get<bool>() << std::endl;
        } else {
            std::cout << response.error().message << std::endl;
        }
    }
}
```

<p>For additional example usage, please refer to the <a href="./examples">examples</a> folder.
