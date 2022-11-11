project "DearIMGui"
    location "imgui"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir (wsbin .."/" .. outputdir .. "/%{prj.name}")
    objdir (wsbinint .. "/" .. outputdir .. "/%{prj.name}")

    includedirs {
        "imgui",
        "glfw/include"
    }

    files {
        "imgui/imconfig.h",
        "imgui/imgui_demo.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_internal.h",
        "imgui/imgui_tables.cpp",
        "imgui/imgui_widgets.cpp",
        "imgui/imgui.cpp",
        "imgui/imgui.h",
        "imgui/imstb_rectpack.h",
        "imgui/imstb_textedit.h",
        "imgui/imstb_truetype.h",
        "imgui/backends/imgui_impl_glfw.h",
        "imgui/backends/imgui_impl_glfw.cpp",
        "imgui/backends/imgui_impl_opengl3.cpp",
        "imgui/backends/imgui_impl_opengl3.h",
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