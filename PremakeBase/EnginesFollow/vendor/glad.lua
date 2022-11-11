project "GLAD"
    location "glad"
    kind "StaticLib"
    language "C"

    targetdir (wsbin .."/" .. outputdir .. "/%{prj.name}")
    objdir (wsbinint .. "/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "glad/include"
    }

    files {
        "glad/include/glad/glad.h",
        "glad/include/KHR/khrplatform.h",
        "glad/src/glad.c"
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
        defines {"_CRT_SECURE_NO_WARNINGS=1"}