const VERSION = "0.2.0"
const mousetrap_commit = "ce80b9ac063170419ffd989921b7d975b74fda72"
const mousetrap_julia_binding_commit = "f1e9b350873591c31e258c89fbe962a032e0c031"

const linux_repo = "mousetrap_linux_jll"
const windows_repo = "mousetrap_windows_jll"
const apple_repo = "mousetrap_apple_jll"

const deploy_linux = false
const deploy_windows = true
const deploy_apple = false

const deploy_local = true
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
    run(`julia -t 8 build_tarballs.jl --debug --verbose --deploy=$repo`)
end

cd("./linux")

if deploy_linux
    if deploy_local
        run_deploy("local")
    else
        run_deploy("Clemapfel/$linux_repo")
    end
end

cd("..")
cd("./windows")

if deploy_windows
    if deploy_local
        run_deploy("local")
    else
        run_deploy("Clemapfel/$windows_repo")
    end
end

cd("..")
cd("./apple")

if deploy_apple
    if deploy_local
        run_deploy("local")
    else
        run_deploy("Clemapfel/$apple_repo")
    end
end

cd("..")
