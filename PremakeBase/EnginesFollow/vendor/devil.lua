
--All of this is windows only for now...

os.executef("cmake ./devIL/DevIL -A x64 -S ./devIL/DevIL -B %s/devIL", wsbin)
os.executef("cmake --build %s/devIL --config debug", wsbin)
os.executef("cmake --build %s/devIL --config release", wsbin)

-- TODO: actually properly build devIL, manage its dependencies too

os.executef("echo Copying Devil prebuild dlls %s", DevILPrebuiltBin)
os.executef("{COPYFILE} %s/../libwin/x64/Release/DevIL.dll %s/Debug-windows-x86_64/%s", IncludeDirs.DevILPrebuilt, wsbin, AppName)
os.executef("{COPYFILE} %s/../libwin/x64/Release/ILU.dll %s/Debug-windows-x86_64/%s", IncludeDirs.DevILPrebuilt, wsbin, AppName)
os.executef("{COPYFILE} %s/../libwin/x64/Release/ILUT.dll %s/Debug-windows-x86_64/%s", IncludeDirs.DevILPrebuilt, wsbin, AppName)



-- Copy results
--os.executef("echo Copying DevIL generated dlls")
--os.executef("{COPYFILE} %s/assimp/bin/Debug/assimp-vc142-mtd.dll %s/Debug-windows-x86_64/%s", wsbin, wsbin, AppName)
--os.executef("{COPYFILE} %s/assimp/bin/Release/assimp-vc142-mt.dll %s/Release-windows-x86_64/%s", wsbin, wsbin, AppName)
--
--assimp_debugdlldir = "/assimp/bin/Debug/assimp-vc142-mtd.dll"
--assimp_releasedlldir = "/assimp/bin/Release/assimp-vc142-mt.dll"
--
--assimp_debuglibdir = "%{wsbin}/assimp/lib/Debug/assimp-vc142-mtd.lib"
--assimp_releaselibdir = "%{wsbin}/assimp/lib/Release/assimp-vc142-mt.lib"
--
--os.executef("echo Assimp dlls %s %s", assimp_debugdlldir, assimp_releasedlldir)