const VERSION = "0.2.1"

const author = "HyperSphereStudio"
const mousetrap_commit = "74507a0bffcfa29d11bd2b5e68268651f36afe7a"
const mousetrap_julia_binding_commit = "7a9dc111ae1c0dde187e8d0386082af9b46d0e1d"

const linux_repo = "hypmousetrap_linux_jll"
const windows_repo = "hypmousetrap_windows_jll"
const apple_repo = "hypmousetrap_apple_jll"

const deploy_linux = true
const deploy_windows = true
const deploy_apple = true

const deploy_local = false

# if local, files will be written to ~/.julia/dev/mousetrap_[linux,windows,apple]_jll

println("deploy linux   : $deploy_linux")
println("deploy windows : $deploy_windows")
println("deploy apple : $deploy_apple")
println("local : $deploy_local")

## Configure

function configure_file(path_in::String, path_out::String)
    file_in = open(path_in, "r")
    file_out = open(path_out, "w+")

    for line in eachline(file_in)
        write(file_out, replace(line,
			"@MOUSETRAP_AUTHOR@" => author,
            "@MOUSETRAP_COMMIT@" => mousetrap_commit,
            "@MOUSETRAP_JULIA_BINDING_COMMIT@" => mousetrap_julia_binding_commit,
            "@VERSION@" => VERSION
        ) * "\n")
    end

    close(file_in)
    close(file_out)
end

if deploy_linux
    @info "Configuring `linux/build_tarballs.jl`"
    configure_file("./linux/build_tarballs.jl.in", "./linux/build_tarballs.jl")

    path = joinpath(pwd(), "$linux_repo")
    if isfile(path)
        run(`rm -r $path`)
    end
end

if deploy_windows
    @info "Configuring `windows/build_tarballs.jl`"
    configure_file("./windows/build_tarballs.jl.in", "./windows/build_tarballs.jl")

    path = joinpath(pwd(), "$windows_repo")
    if isfile(path)
        run(`rm -r $path`)
    end
end

if deploy_apple
    @info "Configuring `apple/build_tarballs.jl`"
    configure_file("./apple/build_tarballs.jl.in", "./apple/build_tarballs.jl")

    path = joinpath(pwd(), "$apple_repo")
    if isfile(path)
        run(`rm -r $path`)
    end
end

## Build

function run_deploy(repo::String)
    run(Cmd(`sudo /workspace/julia/bin/julia -t 8 build_tarballs.jl --debug --verbose --deploy=$repo`; env= ("BINARYBUILDER_RUNNER" => "privileged", )))
end

cd("./linux")

if deploy_linux
    if deploy_local
        run_deploy("local")
    else
        run_deploy("$author/$linux_repo")
    end
end

cd("..")
cd("./windows")

if deploy_windows
    if deploy_local
        run_deploy("local")
    else
        run_deploy("$author/$windows_repo")
    end
end

cd("..")
cd("./apple")

if deploy_apple
    if deploy_local
        run_deploy("local")
    else
        run_deploy("$author/$apple_repo")
    end
end

cd("..")

end

