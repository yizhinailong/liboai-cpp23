set_project("liboai")
set_version("2.0.0")

add_rules("mode.debug", "mode.release")

add_requires("nlohmann_json", "cpr")

option("build_examples")
    set_default(true)
    set_showmenu(true)
    set_description("Build example programs")
option_end()

target("oai", function()
    set_kind("static")
    set_languages("c++23")
    set_policy("build.c++.modules", true)
    set_policy("build.c++.modules.std", true)
    set_policy("build.c++.modules.reuse", true)

    add_files("src/**.cppm", {public = true})

    add_packages("nlohmann_json", "cpr", {public = true})

    -- Linux requires Clang for C++23 modules (GCC has TU-local entity issues with header-only libs)
    on_config(function (target)
        if is_plat("linux") then
            local tc = get_config("toolchain") or ""
            local cxx = target:tool("cxx") or ""
            if tc == "gcc" or (tc == "" and cxx:find("g++", 1, true)) then
                raise([[
liboai C++23 modules require Clang on Linux.

Please run:
    xmake f --toolchain=clang -c -y
]])
            end
        end
    end)
end)


if get_config("build_examples") then
    includes("examples")
end
