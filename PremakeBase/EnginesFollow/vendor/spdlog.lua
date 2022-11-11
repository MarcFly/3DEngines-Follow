project "SPDLOG"
    location "spdlog"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

    targetdir (wsbin .."/" .. outputdir .. "/%{prj.name}")
    objdir (wsbinint .. "/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "spdlog/include"
    }

    files {
        "spdlog/include/**.h",
        "spdlog/include/**.cpp",
        "spdlog/include/**.hpp",
        "spdlog/include/**.c",
        "spdlog/src/**.h",
        "spdlog/src/**.cpp",
        "spdlog/src/**.hpp",
        "spdlog/src/**.c"
    }
    

    filter {"system:windows"}
        defines {"WIN32", "_CRT_SECURE_NO_WARNINGS"}
    
    defines "SPDLOG_COMPILED_LIB"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
    
    filter "configurations:Distribution"
        runtime "Release"
        optimize "On"