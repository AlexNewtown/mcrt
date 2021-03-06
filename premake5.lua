--- premake5.lua
name = "mcrt"
workspace (name)
    language "C++"
    location "build"
    warnings "Extra"
    cppdialect "C++14"
    configurations {"Debug", "Release"}

    filter {"configurations:Debug"}
        defines {"DEBUG"}
        optimize "Off"
        symbols "On"

    filter {"configurations:Release"}
        defines {"RELEASE"}
        optimize "Speed"
        symbols "Off"

    filter {"system:macosx"}
        defines {"MACOS"}
    filter {"system:windows"}
        defines {"WINDOWS"}
    filter {"system:linux or system:bsd"}
        defines {"LINUX_OR_BSD"}

------ Program
project (name)
    targetdir "bin"
    kind "WindowedApp"
    files {"src/main.cc"}
    files {"src/foreign/**.cc"}
    files {"src/"..name.."/**.cc"}
    includedirs {"include/foreign"}
    includedirs {"include"}

    filter {"system:macosx"}
        linkoptions  {"-fopenmp"}
        buildoptions {"-fopenmp"}
    filter {"system:windows"}
        linkoptions  {"-fopenmp"}
        buildoptions {" -static -static-libgcc -static-libstdc++",
                      "-mwindows", "-mconsole", "-fopenmp"}
    filter {"system:linux or system:bsd"}
        linkoptions  {"-fopenmp"}
        buildoptions {"-fopenmp"}
