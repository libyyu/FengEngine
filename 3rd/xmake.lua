
add_subdirs("luasocket", "lua-cjson")

rule("3rd")
    -- if has_config("luajit") then
    --     before_build(function (target)
    --         --target:add("defines", "DEBUG", "TEST=\"hello\"")
    --         --target:add("linkdirs", "/usr/lib", "/usr/local/lib")
    --         target:add({includedirs = "luajit/src", links = "luajit"})
    --     end)
    -- else
    --     before_build(function (target)
    --         target:add({includedirs = "lua-5.1.5/src", links = "lua51"})
    --     end)
    -- end
    -- add_imports("core.project.project")
    -- before_build(function (target)    
    --     for _, dep in ipairs(target:get("deps")) do
    --         local dep_target = project.target(dep)
    --         target:add({files = dep_target:targetfile()})
    --         --print("add ", dep_target:targetfile())
    --     end
    -- end)
rule_end()

-- add target
target("thirdpart")
    set_options("luajit", "genproj", "FengEngineBundle")
    set_kind("static")
    add_rules("3rd")
    add_deps("luasocket", "cjson")
    add_files("lpeg/*.c")
    add_files("pb.c")
	add_files("slua.c")
    add_files("i64lib/*.c")
    if has_config("luajit") then
        add_includedirs("../luajit/src")
    else
        add_includedirs("../lua-5.1.5/src")
    end

    if is_plat("windows") then
        set_languages("c89")
    else
        set_languages("gnu99")
    end