
--All of this is windows only for now...

os.executef("cmake ./assimp -A x64 -S ./assimp -B %s/assimp", wsbin)
os.executef("cmake --build %s/assimp --config debug", wsbin)
os.executef("cmake --build %s/assimp --config release", wsbin)
-- copy config.h back to assimp dir
os.executef("{COPYFILE} %s/assimp/include/assimp/config.h ./assimp/include/assimp", wsbin)
-- Copy results
os.executef("echo Copying Assimp generated dlls")
os.executef("{COPYFILE} %s/assimp/bin/Debug/assimp-vc142-mtd.dll %s/Debug-windows-x86_64/%s", wsbin, wsbin, AppName)
os.executef("{COPYFILE} %s/assimp/bin/Release/assimp-vc142-mt.dll %s/Release-windows-x86_64/%s", wsbin, wsbin, AppName)

assimp_debugdlldir = "/assimp/bin/Debug/assimp-vc142-mtd.dll"
assimp_releasedlldir = "/assimp/bin/Release/assimp-vc142-mt.dll"

assimp_debuglibdir = "%{wsbin}/assimp/lib/Debug/assimp-vc142-mtd.lib"
assimp_releaselibdir = "%{wsbin}/assimp/lib/Release/assimp-vc142-mt.lib"

os.executef("echo Assimp dlls %s %s", assimp_debugdlldir, assimp_releasedlldir)