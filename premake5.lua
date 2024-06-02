workspace "randword"
	architecture "x64"

    configurations { 
        "Debug", 
        "Release" 
    }

    startproject "randword"


project "randword"
    location "randword"
    -- kind "ConsoleApp" 
    -- defines { "_CONSOLE" }
    kind "WindowedApp"
    language "C++"
    warnings "Everything"
    targetdir ("%{wks.location}/target/dis.%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}")
    objdir ("%{wks.location}/target/build.%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}")
    files {
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.ixx",
        "%{prj.name}/src/**.cppm",
        -- "premake5.lua",
		-- "%{wks.location}/target/**.*"
    }
    includedirs {
    }
    cppdialect "C++latest"
    staticruntime "On"
    systemversion "latest"
    conformancemode "Yes"
    externalanglebrackets "On"
    filter { "options:cc=clang" }
        buildoptions {
            "-Wpadded", "-Wconversion", "-Wsign-conversion", "-Wmisleading-indentation",  "-Wnull-dereference",  "-Wdouble-promotion", "-Wimplicit-fallthrough", "-Wunused",
            "-Wno-pre-c23-compat",
            "-Wno-c++98-compat-pedantic",
            "-Wno-pre-c++20-compat-pedantic",
            "-Wno-declaration-after-statement",
            -- "unsafe-buffer-usage",
            "-Wno-unsafe-buffer-usage",
            "-Wno-unused-macros",
            "-Wno-c++98-compat",
            "-fdiagnostics-absolute-paths",
            "-fdata-sections",
            "-ffunction-sections",
            -- "-std=c2x",
        }
    filter { "options:cc=clang", "configurations:Debug" }
        linkoptions {
            "-g"
        }
    filter { "toolset:msc" }
        buildoptions {
            "/Zc:__cplusplus", "/utf-8", "/interface", "/w14242", "/w14254", "/w14263", "/w14265", "/w14287", "/we4289", "/w14296", "/w14311", "/w14545", "/w14546", "/w14547", "/w14549", "/w14555", "/w14619", "/w14640", "/w14826", "/w14905", "/w14906", "/w14928"
        }
        disablewarnings {
            "4464" --  warning C4464: relative include path contains '..'
        }
    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        -- editandcontinue "Off" -- for /Zi
        -- buildoptions "/fsanitize=address"
        links { 
                "user32", "kernel32"
        }
        libdirs { 
        }
    filter { "configurations:Release" }
        exceptionhandling ("Off")
        runtime "Release"
        flags { "LinkTimeOptimization" }
        -- floatingpoint "Fast"
        optimize "Speed"
        links { 
            "user32", "kernel32"
        }
        libdirs { 
        }

