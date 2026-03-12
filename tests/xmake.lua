function regression_target(name, source)
    target(name, function()
        set_kind("binary")
        set_default(false)
        set_languages("c++23")
        set_warnings("all", "error")
        add_files(source)
    end)
end

regression_target("test_authorization", "test_authorization.cpp")
regression_target("test_chat_update", "test_chat_update.cpp")
regression_target("test_files_download", "test_files_download.cpp")
regression_target("test_network_headers", "test_network_headers.cpp")

target("regression", function()
    set_kind("phony")
    add_deps("test_authorization", "test_chat_update", "test_files_download", "test_network_headers")
end)
