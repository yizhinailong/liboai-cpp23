<p align="center">
  <img src="/images/_logo.png">
</p>

<br>
<h1>Introduction</h1>
<p><code>liboai</code> is a simple, <b>unofficial</b> C++23 library for the OpenAI API. It allows developers to access OpenAI endpoints through a simple collection of methods and classes. The library can most effectively be thought of as a <b>spiritual port</b> of OpenAI's Python library, simply called <code>openai</code>, due to its similar structure - with few exceptions.

<h3>Features</h3>

- [x] [ChatGPT](https://github.com/yizhinailong/liboai/tree/main/examples/chat)
- [X] [Audio](https://github.com/yizhinailong/liboai/tree/main/examples/audio)
- [X] [Azure](https://github.com/yizhinailong/liboai/tree/main/examples/azure)
- [X] [Functions](https://platform.openai.com/docs/api-reference/chat/create)
- [x] [Image DALL·E](https://github.com/yizhinailong/liboai/tree/main/examples/images)
- [x] [Models](https://github.com/yizhinailong/liboai/tree/main/examples/models)
- [x] [Completions](https://github.com/yizhinailong/liboai/tree/main/examples/completions) 
- [x] [Edit](https://github.com/yizhinailong/liboai/tree/main/examples/edits) 
- [x] [Embeddings](https://github.com/yizhinailong/liboai/tree/main/examples/embeddings) 
- [x] [Files](https://github.com/yizhinailong/liboai/tree/main/examples/files) 
- [x] [Fine-tunes](https://github.com/yizhinailong/liboai/tree/main/examples/fine-tunes) 
- [x] [Moderation](https://github.com/yizhinailong/liboai/tree/main/examples/moderations)
- [X] Asynchronous Support
- [X] C++23 Modules

<h1>Usage</h1>
See below for just how similar in style <code>liboai</code> and its Python alternative are when generating an image using DALL-E.</p>
<details open>
<summary>DALL-E Generation in Python.</summary>
<br>

```py
from openai import OpenAI
import os

client = OpenAI(api_key=os.getenv("OPENAI_API_KEY"))
response = client.images.generate(
    model="dall-e-3",
    prompt="A snake in the grass!",
    n=1,
    size="1024x1024"
)
print(response.data[0].url)
```
</details>

<details open>
<summary>DALL-E Generation in C++.</summary>
<br>

```cpp
import liboai;
import std;

auto main() -> int {
  liboai::OpenAI oai;
  oai.auth.SetKeyEnv("OPENAI_API_KEY");
	
  auto res = oai.Image->Create(
    "A snake in the grass!",
    1,
    "1024x1024"
  );

  if (res) {
    std::cout << res.value()["data"][0]["url"].get<std::string>() << std::endl;
  } else {
    std::cout << res.error().message << std::endl;
  }
}
```

</details>

<p>Running the above will print out the URL to the resulting generated image, which may or may not look similar to the one found below.</p>
<table>
<tr>
<th>Example Image</th>
</tr>
<td>

<img src="/images/snake.png">

</td>
</tr>
</table>

<p><i>Keep in mind the above C++ example is a minimal example. Please see <a href="/examples">the examples</a> for more detailed code snippets showing proper error handling with std::expected.</i></p>

<h1>Requirements</h1>

- **C++23** compatible compiler with `import std;` support
- **xmake** build system

<h3>Platform Support</h3>

This library uses C++23 standard library modules (`import std;`) and requires a compiler with full support.

| Platform | Toolchain | Minimum Version | Notes |
|----------|-----------|-----------------|-------|
| **Windows** | MSVC | VS 2022 17.5+ | `import std;` supported via `/std:c++20` + `/experimental:module` |
| **Windows** | clang-cl | LLVM 16+ | MSVC ABI compatibility, recommended for cross-platform builds |
| **macOS** | Clang | LLVM 16+ | Install via Homebrew: `brew install llvm` |
| **Linux** | Clang | LLVM 16+ | **Required** for C++23 modules support |
| **Linux** | GCC | 13+ | **Not supported** - GCC has issues with C++23 modules and header-only libraries |

<h4>macOS Setup</h4>

The default Apple Clang does not support `import std;`. Install LLVM via Homebrew:

```bash
brew install llvm
```

Then build with the Homebrew LLVM toolchain:

```bash
xmake f --toolchain=clang --sdk=$(brew --prefix llvm) -c -y
xmake -v
```

Build with a specific toolchain:
```bash
xmake --toolchain=clang
xmake --toolchain=msvc
xmake --toolchain=clang-cl
```

<h1>Dependencies</h1>
<p>For the library to work the way it does, it relies on two major dependencies. These dependencies can be found listed below.<p>

- <a href="https://github.com/nlohmann/json">nlohmann-json</a> - JSON parsing
- <a href="https://github.com/libcpr/cpr">cpr</a> - HTTP requests (C++ Requests)

*Dependencies are managed automatically by xmake.*

<h3>File Paths</h3>

The library uses `std::filesystem` internally and supports both Windows-style (`C:\path\to\file`) and Unix-style (`/path/to/file`) paths. For maximum portability, use forward slashes or `std::filesystem::path`:

```cpp
// Portable - works on all platforms
std::filesystem::path myPath = "data/file.jsonl";

// Also works on all platforms (forward slashes)
oai.File->Create("data/file.jsonl", "fine-tune");
```

<h3>TLS/HTTPS Certificates</h3>

The library uses [cpr](https://github.com/libcpr/cpr) (libcurl) for HTTPS requests. Certificate verification behavior depends on the platform:

| Platform | Certificate Store | Notes |
|----------|------------------|-------|
| **Windows** | Windows Certificate Store (Schannel) | Uses system certificates automatically |
| **macOS** | System Keychain | Uses system certificates automatically |
| **Linux** | File-based (`/etc/ssl/certs`) | Requires `ca-certificates` package |

**Troubleshooting SSL certificate errors on Linux:**

```bash
# Debian/Ubuntu
sudo apt-get install ca-certificates
sudo update-ca-certificates

# Set custom CA bundle (if needed)
export CURL_CA_BUNDLE=/path/to/ca-bundle.crt
```

<h1>Building</h1>

```bash
# Build library only
xmake

# Build with examples (default)
xmake -DBUILD_EXAMPLES=ON

# Build in release mode
xmake -r
```

**Linux builds require Clang:**

```bash
xmake f --toolchain=clang -c -y
xmake -v
```

**Troubleshooting build errors:**

If you see errors like "exposes TU-local entity" from nlohmann_json headers, you are using GCC. GCC has known issues with C++23 modules when importing header-only libraries. Switch to Clang to fix this.

<h3>Local Verification</h3>

<p>This repository no longer uses CI automation. All verification must be performed locally using the following commands.</p>

<details>
<summary><b>Verification Commands</b></summary>

```bash
# Configure build (Linux requires Clang)
xmake f --toolchain=clang -c -y

# Build the project
xmake -v

# Build regression test target
xmake build regression

# Run individual verification tests
xmake run test_authorization
xmake run test_chat_update
xmake run test_files_download
```

</details>
<h3>Installation</h3>

Install the library to a specific prefix:

```bash
xmake install -o /path/to/install/prefix
```

This installs:
- `lib/liboai.a` - static library
- `modules/` - C++23 module interface files

To use the installed library in another xmake project:

```lua
add_requires("nlohmann_json", "cpr")

target("myapp")
    set_kind("binary")
    set_languages("c++23")
    add_files("src/*.cpp")
    add_links("/path/to/install/prefix/lib/liboai.a")
    add_includedirs("/path/to/install/prefix/modules", {public = true})
```

<h1>Documentation</h1>
<p>For detailed documentation and additional code examples, see the library's examples <a href="/examples">here</a>.

<h1>Contributing</h1>
<p>Artificial intelligence is an exciting and quickly-changing field. 

If you'd like to partake in further placing the power of AI in the hands of everyday people, please consider contributing by submitting new code and features via a **Pull Request**. If you have any issues using the library, or just want to suggest new features, feel free to open an **Issue**.

<h1>License</h1>
<p>This project is licensed under the MIT License - see the LICENSE file for details.
