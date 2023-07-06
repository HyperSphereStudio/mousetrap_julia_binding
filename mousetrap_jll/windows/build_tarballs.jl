# Note that this script can accept some limited command-line arguments, run
# `julia build_tarballs.jl --help` to see a usage message.
using BinaryBuilder, Pkg

name = "mousetrap_windows"
version = v"0.1.0"

# Collection of sources required to complete build
sources = [
    GitSource("https://github.com/Clemapfel/mousetrap.git", "9f8a41bc3e420e23baa3a482121a0c143fb79487"),
    GitSource("https://github.com/Clemapfel/mousetrap_julia_binding.git", "bdcf6442410fe30a68a2b074bc42ca4352481614"),
    FileSource("https://dllfile.net/download/18762", "b8df8a508817b5f4d8cac6365e3fbf0e8952d50aa58ed9b5032ec5767e530aaa")
]

# Bash recipe for building across all platforms
script = raw"""
cd $WORKSPACE/srcdir
unzip 18762 
mv opengl32.dll $prefix/bin

cd mousetrap
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} -DOpenGL=$prefix/bin/opengl32.dll -DGLEW=$prefix/bin/glew32.dll 
make install -j 8
cd ..
mkdir ${prefix}/share/licenses/mousetrap_windows
cp LICENSE ${prefix}/share/licenses/mousetrap_windows/LICENSE
cd ..
cd mousetrap_julia_binding/
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} -DJulia_INCLUDE_DIRS=$prefix/include/julia -DJulia_LIBRARY=$prefix/bin/libjulia.dll
make install -j 8
cd ..

mv $prefix/lib/libmousetrap.dll $prefix/bin/libmousetrap.dll
exit
"""

# These are the platforms we will build for by default, unless further
# platforms are passed in on the command line
platforms = [
    Platform("x86_64", "windows"; )
]


# The products that we will ensure are always built
products = [
    LibraryProduct("libmousetrap", :mousetrap_windows),
    LibraryProduct("libmousetrap_julia_binding", :mousetrap_julia_binding_windows)
]

# Dependencies that must be installed before this package can be built
dependencies = [
    Dependency(PackageSpec(name="GLEW_jll", uuid="bde7f898-03f7-559e-8810-194d950ce600"))
    Dependency(PackageSpec(name="GTK4_jll", uuid="6ebb71f1-8434-552f-b6b1-dc18babcca63"))
    Dependency(PackageSpec(name="OpenGLMathematics_jll", uuid="cc7be9be-d298-5888-8f50-b85d5f9d6d73"))
    Dependency(PackageSpec(name="libcxxwrap_julia_jll", uuid="3eaa8342-bff7-56a5-9981-c04077f7cee7"))
    Dependency(PackageSpec(name="libjulia_jll", uuid="5ad3ddd2-0711-543a-b040-befd59781bbf"))
    Dependency(PackageSpec(name="X11_jll", uuid="546b0b6d-9ca3-5ba2-8705-1bc1841d8479"))
]

# Build the tarballs, and possibly a `build.jl` as well.
build_tarballs(ARGS, name, version, sources, script, platforms, products, dependencies; julia_compat="1.6", preferred_gcc_version = v"12.1.0")
