# Note that this script can accept some limited command-line arguments, run
# `julia build_tarballs.jl --help` to see a usage message.
using BinaryBuilder, Pkg

name = "libpolymake_julia"
version = v"0.0.1"

# Collection of sources required to complete build
sources = [
    GitSource("https://github.com/oscar-system/libpolymake-julia", "408f7090b6d9943a8017a75066208e82d693abba")
]

# Bash recipe for building across all platforms
script = raw"""

# work around weird LD_LIBRARY_PATH for linux targets:
# remove $libdir
if [[ -n "$LD_LIBRARY_PATH" ]]; then
LD_LIBRARY_PATH=$(echo -n $LD_LIBRARY_PATH | sed -e "s|[:^]$libdir\w*|:|g")
fi

# FIXME: to be moved to polymake_jll
sed -i -e 's#\$^O eq "darwin"#$ConfigFlags{Arch} =~ /^darwin/#g' $bindir/polymake-config

mkdir build
cd build
cmake -DJulia_PREFIX=$prefix -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_BUILD_TYPE=Release -DCMAKE_FIND_ROOT_PATH=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} -DCMAKE_BUILD_TYPE=Release ../libpolymake-j*/
VERBOSE=ON cmake --build . --config Release --target install -- -j${nproc}
install_license $WORKSPACE/srcdir/libpolymake-j*/LICENSE.md

rm $bindir/polymake-config
"""

# These are the platforms we will build for by default, unless further
# platforms are passed in on the command line
platforms = [
    MacOS(:x86_64),
	 Linux(:x86_64, libc=:glibc, compiler_abi=CompilerABI(libstdcxx_version = v"3.4.23", cxxstring_abi=:cxx11)),
	 Linux(:x86_64, libc=:glibc, compiler_abi=CompilerABI(libstdcxx_version = v"3.4.25", cxxstring_abi=:cxx11)),
	 Linux(:x86_64, libc=:glibc, compiler_abi=CompilerABI(libstdcxx_version = v"3.4.26", cxxstring_abi=:cxx11))
]

# The products that we will ensure are always built
products = [
    LibraryProduct("libpolymake_julia", :libpolymake_julia; dlopen_flags=[:RTLD_GLOBAL]),
    FileProduct(joinpath("share","libpolymake_julia","type_translator.jl"),:type_translator)
]

# Dependencies that must be installed before this package can be built
dependencies = [
    Dependency(PackageSpec(name="libcxxwrap_julia_jll",version=v"0.8")),
    Dependency(PackageSpec(name="polymake_jll",uuid="7c209550-9012-526c-9264-55ba7a78ba2c",url="https://github.com/benlorenz/polymake_jll.jl")),
    Dependency(PackageSpec(name="Perl_jll",uuid="83958c19-0796-5285-893e-a1267f8ec499",url="https://github.com/benlorenz/Perl_jll.jl")),
    Dependency(PackageSpec(name="CompilerSupportLibraries_jll", uuid="e66e0078-7015-5450-92f7-15fbd957f2ae")),
    BuildDependency(PackageSpec(name="Julia_jll",version=v"1.4.1"))
]
#,version=v"4.1"))

# Build the tarballs, and possibly a `build.jl` as well.
build_tarballs(ARGS, name, version, sources, script, platforms, products, dependencies; preferred_gcc_version = v"7")
