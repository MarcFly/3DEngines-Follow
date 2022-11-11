project "MathGeoLib"
    location "mathgeolib"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    
    targetdir (wsbin .."/" .. outputdir .. "/%{prj.name}")
    objdir (wsbinint .. "/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "mathgeolib/src"
    }

    files {
        "mathgeolib/src/**.h",
        "mathgeolib/src/**.cpp",
        "mathgeolib/src/**.hpp",
        "mathgeolib/src/**.c"
    }

    filter {"system:windows"}
        defines "WIN32"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
    
    filter "configurations:Distribution"
        runtime "Release"
        optimize "On"

    filter {"system:windows", "configurations:Release"}
        buildoptions "/MT"

    filter {"system:windows", "configurations:Distribution"}
        buildoptions "/MT"

    filter {"system:windows", "configurations:Debug"}
        buildoptions "/MTd"
        
    defines {"_CRT_SECURE_NO_WARNINGS"}