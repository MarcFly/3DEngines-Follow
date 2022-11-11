LibName = "EnginesFollow"
AppName = "Sandbox"
LibInclude = "%{LibName}/src"


workspace "Engines3DFollow"
    location "./"
    configurations { "Debug", "Release", "Distribution" }
    architecture "x64"
    startproject "%{AppName}"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
wsbin = path.join(_WORKING_DIR, "./bin/")
wsbinint = path.join(_WORKING_DIR, "./bin-int/")

IncludeDirs = {}
IncludeDirs["GLFW"] = "%{LibName}/vendor/glfw/include"
IncludeDirs["SPDLOG"] = "%{LibName}/vendor/spdlog/include"
IncludeDirs["GLAD"] = "%{LibName}/vendor/glad/include"
IncludeDirs["DearIMGui"] = "%{LibName}/vendor/imgui"
IncludeDirs["MathGeoLib"] = "%{LibName}/vendor/mathgeolib/src"
IncludeDirs["Parson"] = "%{LibName}/vendor/parson"
IncludeDirs["PCG"] = "%{LibName}/vendor/pcg"
IncludeDirs["Assimp"] = "%{LibName}/vendor/assimp/include"
IncludeDirs["DevILPrebuilt"] = path.join(_WORKING_DIR, "vendor/DevIL_Windows_SDK/include")
DevILPrebuiltBin = "%{IncludeDirs.DevILPrebuilt}/../libwin/x64/Release"

include "EnginesFollow/vendor/glfw.lua"
include "EnginesFollow/vendor/glad.lua"
include "EnginesFollow/vendor/imgui.lua"
include "EnginesFollow/vendor/mathgeolib.lua"
include "EnginesFollow/vendor/parson.lua"
include "EnginesFollow/vendor/pcg.lua"
include "EnginesFollow/vendor/assimp.lua"
include "EnginesFollow/vendor/spdlog.lua"
include "EnginesFollow/vendor/devil.lua"

project "EnginesFollow"
    location "%{prj.name}"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cxx",
        "%{prj.name}/src/**.c"
    }

    

    pchheader "EnginePCH.h"
    pchsource "%{prj.name}/src/EnginePCH.cpp"

    includedirs {
        "%{prj.name}/src",
        "%{IncludeDirs.SPDLOG}",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.DearIMGui}",
        "%{IncludeDirs.MathGeoLib}",
        "%{IncludeDirs.Parson}",
        "%{IncludeDirs.PCG}",
        "%{IncludeDirs.Assimp}",
        "%{IncludeDirs.DevILPrebuilt}"
    }

    filter "system:windows"
        links {
            "GLFW",
            "opengl32.lib"
        }

    libdirs{ ("bin/" .. outputdir .. "/%{AppName}"), "%{DevILPrebuiltBin}" }
    links {
        "GLAD",
        "DearIMGui",
        "MathGeoLib",
        "parson",
        "PCG",
        "SPDLOG",
        "%{DevILPrebuiltBin}/DevIL.lib",
        "%{DevILPrebuiltBin}/ILU.lib",
        "%{DevILPrebuiltBin}/ILUT.lib"
    }

    filter "system:windows"
        systemversion "latest"

    defines {
        "EF_BUILD_STATIC",
        "EF_WINDOWS",
        "GLFW_INCLUDE_NONE"
    }

    postbuildcommands {
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/%{AppName}")
    }

    filter "configurations:Debug"
        defines "EF_DEBUG"
        symbols "On"
        links {"%{assimp_debuglibdir}"}
        
    filter "configurations:Release"
        defines "EF_RELEASE"
        links {"%{assimp_releaselibdir}"}
        
    filter "configurations:Distribution"
        defines "EF_DISTRIBUTION"
        links {"%{assimp_releaselibdir}"}

    filter "configurations:Release, Distribution"
        optimize "On"

    filter {"system:windows", "configurations:Release"}
        buildoptions "/MT"

    filter {"system:windows", "configurations:Distribution"}
        buildoptions "/MT"

    filter {"system:windows", "configurations:Debug"}
        buildoptions "/MTd"


project "Sandbox"
    location "%{prj.name}"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.hpp",
        "%{prj.name}/**.cxx"
    }

    includedirs {
        "%{LibInclude}",
        "%{IncludeDirs.SPDLOG}",
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.DearIMGui}",
        "%{IncludeDirs.MathGeoLib}",
        "%{IncludeDirs.Parson}",
        "%{IncludeDirs.PCG}",
        "%{IncludeDirs.Assimp}"
    }

    libdirs{ ("bin/" .. outputdir .. "/%{AppName}") }
    links {
        "%{LibName}"
    }
    os.executef("echo Linking or not? %s", assimp_debugdlldir)
    filter "system:windows"
        systemversion "latest"

    defines {
        "EF_BUILD_STATIC",
        "EF_WINDOWS"
    }

    filter "configurations:Debug"
        defines "EF_DEBUG"
        symbols "On"

        
    filter "configurations:Release"
        defines "EF_RELEASE"
        optimize "On"

        
    filter "configurations:Distribution"
        defines "EF_DISTRIBUTION"
        optimize "On"

    filter {"system:windows", "configurations:Release"}
        buildoptions "/MT"

    
