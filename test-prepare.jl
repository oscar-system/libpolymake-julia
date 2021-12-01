using Pkg
using polymake_jll
using libcxxwrap_julia_jll

include(polymake_jll.generate_deps_tree)

const polymake_deps_tree = prepare_deps_tree(mktempdir(;cleanup=false))

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

# configure libpolymake-julia with artifact dirs
run(`cmake \
     -DPolymake_PREFIX=$(polymake_deps_tree) \
     -DJulia_PREFIX=$(joinpath(Sys.BINDIR,"..")) \
     -DJlCxx_DIR=$(libcxxwrap_julia_jll.artifact_dir)/lib/cmake/JlCxx \
     -DCMAKE_INSTALL_PREFIX=$(pwd())/test/install \
     -DCMAKE_BUILD_TYPE=Release \
     -S . -B build`);

# add override

let file = joinpath(Pkg.depots1(),"artifacts","Overrides.toml")
    if !isfile(file)
        mkpath(dirname(file))
        touch(file)
    end
    lines = readlines(file)
    pkgid = Base.identify_package("libpolymake_julia_jll")
    k = findfirst(==("[$(pkgid.uuid)]"), lines)
    if !isnothing(k)
        @assert k < length(lines) "Overrides.toml seem to be ill formatted"
        lines[k+1] = "libpolymake_julia = \"$(joinpath(pwd(),"test","install"))\""
    else
        append!(lines, ["[$(pkgid.uuid)]", "libpolymake_julia = \"$(joinpath(pwd(),"test","install"))\""])
    end
    if !("--override" in ARGS)
        @info "$file to be written:\n$(join(lines, "\n"))\nTo actually write the file run julia --project test-prepare.jl --override"
    else
        write(file, join(lines, "\n"))
        @info "$file written."
    end
end

if "--build" in ARGS
    run(`cmake --build build --config Release --target install -- -j2`)
    run(`$(polymake()) --iscript $(pwd())/src/polymake/apptojson.pl $(pwd())/test/install/share/libpolymake_julia/appsjson/`)
end
