workspace(name = "spatialstein3d")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

http_archive(
    name = "SDL_win",
    urls = ["https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip"],
    sha256 = "00c55a597cebdb9a4eb2723f2ad2387a4d7fd605e222c69b46099b15d5d8b32d",
    strip_prefix = "SDL2-2.0.12",
    build_file = "//dependencies/SDL:BUILD",
)
http_archive(
    name = "SDL_image_win",
    urls = ["https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip"],
    sha256 = "a180f9b75c4d3fbafe02af42c42463cc7bc488e763cfd1ec2ffb75678b4387ac",
    strip_prefix = "SDL2_image-2.0.5",
    build_file = "//dependencies/SDL:BUILD",
)
http_archive(
    name = "SDL_ttf_win",
    urls = ["https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-VC.zip"],
    sha256 = "aab0d81f1aa6fe654be412efc85829f2b188165dca6c90eb4b12b673f93e054b",
    strip_prefix = "SDL2_ttf-2.0.15",
    build_file = "//dependencies/SDL:BUILD",
)
