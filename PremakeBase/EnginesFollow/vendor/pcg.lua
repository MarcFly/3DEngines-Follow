project "PCG"
    location "pcg"
    kind "StaticLib"
    language "C"

    targetdir (wsbin .."/" .. outputdir .. "/%{prj.name}")
    objdir (wsbinint .. "/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "pcg"
    }

    files {
        "pcg/pcg_basic.h",
        "pcg/pcg_basic.c"
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
    
    filter {"system:windows"}
        defines {"_CRT_SECURE_NO_WARNINGS"}
    