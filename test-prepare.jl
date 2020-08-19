using Pkg
using polymake_jll
using libcxxwrap_julia_jll

# we need to adjust the test-driver to running from the callable library
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

# FIXME: we need to fix polymake-config in polymake_jll
# to allow building against that artifact
# for now we copy and patch this here
open(joinpath("test","polymake-config"), write=true) do out
    open(joinpath(polymake_jll.artifact_dir,"bin","polymake-config")) do in
        for line in eachline(in, keep=true) # keep so the new line isn't chomped
            write(out, line)
            if startswith(line,"my \$root=")
                write(out,"""
                          \$root = "\$ENV{POLYMAKE_DEPS_TREE}/share/polymake";
                          \$InstallArch = "\$ENV{POLYMAKE_DEPS_TREE}/lib/polymake";
                          """)
            end
        end
    end
end
chmod(joinpath("test","polymake-config"),0o755);

# configure libpolymake-julia with artifact dirs
run(`cmake \
     -DPolymake_Config_EXECUTABLE=$(pwd())/test/polymake-config \
     -DJlCxx_DIR=$(libcxxwrap_julia_jll.artifact_dir)/lib/cmake/JlCxx \
     -DCMAKE_INSTALL_PREFIX=$(pwd())/test/install \
     -DCMAKE_BUILD_TYPE=Release \
     -S . -B build`);

# add override
open("$(joinpath(Pkg.depots1(),"artifacts","Overrides.toml"))", "a") do io
    project = Base.active_project()
    uuid = Base.project_deps_get(project, "libpolymake_julia_jll")
    write(io, """
              [$(uuid.uuid)]
              libpolymake_julia = "$(joinpath(pwd(),"test","install"))"
              """)
end;

