using Pkg

Pkg.add("polymake_jll")
Pkg.add("libcxxwrap_julia_jll")

using polymake_jll
using libcxxwrap_julia_jll

include(polymake_jll.generate_deps_tree)

const polymake_deps_tree = prepare_deps_tree(mktempdir(;cleanup=false))

extraldflags=""
extralibs=""

if isdefined(polymake_jll.FLINT_jll,:OpenBLAS32_jll)
    blasdepsdir = joinpath(polymake_deps_tree,"deps","OpenBLAS32_jll")
    force_symlink(polymake_jll.FLINT_jll.OpenBLAS32_jll.artifact_dir,blasdepsdir)
    extraldflags="-L$(joinpath(blasdepsdir,"lib"))"
    extralibs="-lopenblas"
end

# we need to adjust the test-driver to running from the callable library
let file = joinpath("test","run_testcases")
    if !isfile(file)
        mkpath(dirname(file))
        touch(file)
    end
end

open(joinpath("test","run_testcases"), write=true) do out
    open(joinpath(polymake_jll.artifact_dir,"share","polymake","scripts","run_testcases")) do in
        for line in eachline(in, keep=true) # keep so the new line isn't chomped
            if startswith(line,"use Polymake")
                write(out,"use Getopt::Long;\n")
            end
            write(out, line)
        end
    end
end

installdir = joinpath(pwd(),"test","install")

# configure libpolymake-julia with artifact dirs
run(`cmake \
     -DPolymake_PREFIX=$(polymake_deps_tree) \
     -DJulia_PREFIX=$(joinpath(Sys.BINDIR,"..")) \
     -DJlCxx_DIR=$(libcxxwrap_julia_jll.artifact_dir)/lib/cmake/JlCxx \
     -DCMAKE_INSTALL_PREFIX=$(installdir) \
     -DCMAKE_EXE_LINKER_FLAGS="$(extraldflags)" \
     -DEXTRA_LIBRARIES="$(extralibs)" \
     -DCMAKE_BUILD_TYPE=Release \
     -S . -B build`);

if "--build" in ARGS
    run(`cmake --build build --config Release --target install -- -j2`)
end

# add override
let file = joinpath(Pkg.depots1(),"artifacts","Overrides.toml")
    if !isfile(file)
        mkpath(dirname(file))
        touch(file)
    end
    lines = readlines(file)
    # id for libpolymake_julia_jll
    pkgid = "4d8266f6-2b3b-57e3-ad7a-d431eaaac945"
    k = findfirst(==("[$(pkgid)]"), lines)
    if !isnothing(k)
        @assert k < length(lines) "Overrides.toml seem to be ill formatted"
        lines[k+1] = "libpolymake_julia = \"$(installdir)\""
    else
        append!(lines, ["[$(pkgid)]", "libpolymake_julia = \"$(installdir)\""])
    end
    if !("--override" in ARGS)
        @info "$file to be written:\n$(join(lines, "\n"))\nTo actually write the file run julia --project test-prepare.jl --override"
    else
        write(file, join(lines, "\n"))
        @info "$file written."

        # make sure our override is already active for this session
        Artifacts.load_overrides(;force=true)
    end
end

if "--ignore-compat" in ARGS
    jlldir = joinpath(pwd(),"test","jll")
    if isdir(jlldir)
        run(`git -C $jlldir pull --autostash`)
    else
        run(`git clone --depth 1 https://github.com/JuliaBinaryWrappers/libpolymake_julia_jll.jl $(jlldir)`)
    end
    run(`sed -i -e '/^polymake_jll = "~/d' $(jlldir)/Project.toml`)
    Pkg.develop(path="$(jlldir)")
else
    Pkg.add("libpolymake_julia_jll")
end

if "--build" in ARGS
    # we need the binary wrappers for the dependencies (topcom+4ti2)
    # for the relevant functions to appear in the json
    Pkg.add(["TOPCOM_jll", "lib4ti2_jll", "BinaryWrappers"])
    using TOPCOM_jll
    using lib4ti2_jll
    using BinaryWrappers
    binpaths = [
             BinaryWrappers.generate_wrappers(lib4ti2_jll, nothing),
             BinaryWrappers.generate_wrappers(TOPCOM_jll, nothing),
           ]

    ENV["PATH"] = join([binpaths...,ENV["PATH"]], ":")
    using libpolymake_julia_jll
    mktempdir() do userpath
        jsondir = joinpath(installdir,"share","libpolymake_julia","appsjson")
        mkpath(jsondir)
        polymake_run_script() do exe
            run(`$exe --config=user=$userpath "$(pwd())/src/polymake/apptojson.pl" "$(jsondir)"`)
        end
    end
end
