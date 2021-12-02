# libpolymake-julia

This is the C++ library accompanying [Polymake.jl](https://github.com/oscar-system/Polymake.jl).
It implements the C++ interface from julia to polymake using [CxxWrap.jl](https://github.com/JuliaInterop/CxxWrap.jl) and [libcxxwrap-julia](https://github.com/JuliaInterop/libcxxwrap-julia).

For Polymake.jl versions before 0.5 this was included in Polymake.jl but since version 0.5 this library is installed as a separate artifact
via [libpolymake_julia_jll](https://github.com/JuliaBinaryWrappers/libpolymake_julia_jll.jl).

## Building

Compiling `libpolymake-julia` from source requires a C++17 enabled compiler, a `libcxxwrap-julia` installation and a polymake installation.

```bash
git clone https://github.com/oscar-system/libpolymake-julia
cmake -DJulia_PREFIX=/home/user/path/to/julia \
      -DPolymake_PREFIX=/home/user/path/to/polymake \
      -DJlCxx_DIR=/home/user/path/to/libcxxwrap-julia/lib/cmake/JlCxx \
      -DCMAKE_INSTALL_PREFIX=/home/user/prefix/for/libpolymake-julia \
      -DCMAKE_BUILD_TYPE=Release \
      -S libpolymake-julia -B build
cmake --build build --config Release --target install -- -j${nproc}
```

### Overriding the default artifacts for Polymake.jl

Put the following into `~/.julia/artifacts/Overrides.toml` to replace the `libpolymake-julia` artifact:

```toml
[4d8266f6-2b3b-57e3-ad7a-d431eaaac945]
libpolymake_julia = "/home/user/prefix/for/libpolymake-julia"
```

Overrides for `polymake` and `libcxxwrap-julia` with the directories used during the build need to be added as well, e.g.:

```toml
[7c209550-9012-526c-9264-55ba7a78ba2c]
polymake = "/home/user/path/to/polymake"

[3eaa8342-bff7-56a5-9981-c04077f7cee7]
libcxxwrap_julia = "/home/user/path/to/libcxxwrap-julia"
```

### Convenience script

For convenience we provide `test-prepare.jl` script which can be used to compile and place overrides at once.
First execute
```julia
julia --project=test/project test-prepare.jl --build
```
If the compilation succeeds and the displayed overrides seem correct run
```julia
julia --project=test/project test-prepare.jl --build --override
```
To ignore the current compat bounds for `polymake_jll` run
```julia
julia --project=test/project test-prepare.jl --build --override --ignore-compat
```


### Wrapping a new type (TYPE)

In general you will need to complete the following steps:

  - In `src/` add a new file `type_TYPE.cpp`
  - Setup your type in `type_setup.pl`
  - Add your `add_TYPE` to both `include/jlpolymake/type_modules.h` and `src/jlpolymake.cpp`

Please use the existing types for a first reference on the exact details.
