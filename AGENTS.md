# PROJECT KNOWLEDGE BASE

**Generated:** 2026-02-16
**Commit:** e582105
**Branch:** main

## OVERVIEW

Unofficial C++17 library for OpenAI API. Spiritual port of Python's `openai` library with similar structure. Single-header include design (`#include "liboai/liboai.hpp"`).

## STRUCTURE

```
include/liboai/
├── liboai.hpp              # Main entry - include only this
├── components/             # API endpoint wrappers (audio, chat, images, etc.)
│   ├── audio.hpp
│   ├── azure.hpp
│   ├── chat.hpp            # Largest component (~40k lines)
│   ├── completions.hpp
│   ├── edits.hpp
│   ├── embeddings.hpp
│   ├── files.hpp
│   ├── fine_tunes.hpp
│   ├── images.hpp          # DALL-E generation
│   ├── models.hpp
│   └── moderations.hpp
└── core/                   # Infrastructure
    ├── authorization.hpp   # Singleton auth manager
    ├── exception.hpp       # OpenAIException, OpenAIRateLimited
    ├── network.hpp         # HTTP request interface
    ├── netimpl.hpp         # cURL wrapper (~47k lines)
    └── response.hpp        # Response, JsonConstructor

src/
├── components/             # Component implementations (.cpp)
└── core/                   # Core implementations (.cpp)

documentation/              # Examples per API feature
├── audio/
├── authorization/
├── chat/                   # Conversation helper class
├── embeddings/
├── images/
└── ...
```

## WHERE TO LOOK

| Task | Location | Notes |
|------|----------|-------|
| Add new API endpoint | `include/liboai/components/` + `src/components/` | Create .hpp/.cpp pair, add to xmake.lua, include in liboai.hpp |
| Modify auth behavior | `include/liboai/core/authorization.hpp` | Singleton pattern via `Authorization::Authorizer()` |
| Change network layer | `include/liboai/core/netimpl.hpp` | cURL wrapper, ~47k lines |
| Response handling | `include/liboai/core/response.hpp` | JSON via nlohmann::json |
| Usage examples | `documentation/{feature}/examples/` | Sync + async variants |
| Build configuration | `xmake.lua` | Primary build system (xmake) |

## CODE MAP

| Symbol | Type | Location | Role |
|--------|------|----------|------|
| `OpenAI` | Class | `include/liboai/liboai.hpp` | Facade - owns all component unique_ptrs |
| `Authorization` | Class | `include/liboai/core/authorization.hpp` | Singleton for API keys/proxies/timeout |
| `Response` | Class | `include/liboai/core/response.hpp` | JSON response wrapper (operator[] access) |
| `FutureResponse` | Type | `include/liboai/core/response.hpp` | `std::future<liboai::Response>` |
| `JsonConstructor` | Class | `include/liboai/core/response.hpp` | Build JSON payloads via `json.insert()` |
| `Audio` | Component | `include/liboai/components/audio.hpp` | Speech/transcription/translation |
| `ChatCompletion` | Component | `include/liboai/components/chat.hpp` | ChatGPT API, includes `Conversation` helper |
| `Azure` | Component | `include/liboai/components/azure.hpp` | Azure OpenAI endpoints |
| `Images` | Component | `include/liboai/components/images.hpp` | DALL-E generation |
| `Models` | Component | `include/liboai/components/models.hpp` | Model listing/retrieval |
| `OpenAIException` | Exception | `include/liboai/core/exception.hpp` | Base exception class |
| `OpenAIRateLimited` | Exception | `include/liboai/core/exception.hpp` | Rate limit specific exception |

## CONVENTIONS

- **C++17** required
- **Namespace**: `liboai`
- **Single header**: Users include only `liboai/liboai.hpp`
- **Singleton auth**: `Authorization::Authorizer()` for global auth state
- **Non-copyable/movable**: Core classes use `NON_COPYABLE(Class)` and `NON_MOVABLE(Class)` macros
- **Return type**: `Response` for sync, `FutureResponse` for async
- **Component pattern**: Each API area = component class with `create()`, `create_async()` methods
- **Error handling**: Throws `liboai::exception::OpenAIException`
- **LIBOAI_EXPORT**: Visibility macro for exported symbols

## ANTI-PATTERNS

- **DO NOT** include individual component headers - use `liboai/liboai.hpp`
- **DO NOT** copy/move `OpenAI`, `Authorization`, `Response` instances (deleted ops)
- **DO NOT** access `raw_json` directly before checking `status_code`
- **DO NOT** modify component classes directly - always use `OpenAI` facade
- **NEVER** call component methods before setting auth key

## DEPENDENCIES

- **nlohmann_json** - JSON parsing
- **cURL** - HTTP requests
- Install via **vcpkg** on Windows

## COMMANDS

#BV|```bash
#TH|# Build library only (xmake)
#ZN|xmake
#TS|
#RR|# Build with examples (xmake)
#VB|xmake -DBUILD_EXAMPLES=ON
#WY|
#JR|# Legacy CMake (if using)
#BB|cmake -B build && cmake --build build
#PP|
#VY|# Windows (vcpkg)
#QW|set VCPKG_ROOT=<path> && xmake
#TS|
#KR|# Local regression tests (CI workflow removed - use these commands locally)
#ZN|xmake build regression
#VY|xmake run test_authorization
#WY|xmake run test_chat_update
#BB|xmake run test_files_download
#HH|```

## NOTES

#YX|- CI workflow has been removed from this repository. Planners and executors should not rely on GitHub Actions for verification. Use the local regression commands above instead.
#YX|- Repository no longer actively maintained - refer to forks (e.g., jasonduncan/liboai)
- Azure OpenAI supported via separate `Azure` component
- Chat component includes `Conversation` helper class for multi-turn context management
- All async methods: `{method}_async()` suffix, returns `FutureResponse`
- Default timeout: 30000ms (set via `auth.SetMaxTimeout()`)
- All component pointers accessed via `OpenAI` facade: `oai.ChatCompletion->create(...)`
