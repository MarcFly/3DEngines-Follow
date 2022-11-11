project "parson"
    location "parson"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir (wsbin .."/" .. outputdir .. "/%{prj.name}")
    objdir (wsbinint .. "/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "parson"
    }

    files {
        "parson/parson.h",
        "parson/parson.c"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
    
    filter "configurations:Distribution"
        runtime "Release"
        optimize "On"
    filter "system:windows"
        defines {"_CRT_SECURE_NO_WARNINGS"}