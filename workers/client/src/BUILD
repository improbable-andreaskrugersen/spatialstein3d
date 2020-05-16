SHARED_DEPS = ["//dependencies/eigen:eigen"]

cc_binary(
    name = "spatialstein3d",
    srcs = glob(["*.hpp", "*.cpp"]),
    linkopts = select({
        "@bazel_tools//src/conditions:linux_x86_64": [
            "-lSDL2", "-lSDL2_image", "-lSDL2_ttf"
        ],
        "//conditions:default": [],
    }),
    copts = select({
        "@bazel_tools//src/conditions:windows": [
            "/W4", "/WX"
        ],
        "@bazel_tools//src/conditions:linux_x86_64": [
            "-Wall", "-Werror", "-g"
        ]
    }),
    deps = select({
        "@bazel_tools//src/conditions:windows": SHARED_DEPS + [
            "@SDL_win//:headers",
            "@SDL_win//:SDL_lib",
            "@SDL_image_win//:headers",
            "@SDL_image_win//:SDL_image_lib",
            "@SDL_image_win//:libpng",
            "@SDL_ttf_win//:headers",
            "@SDL_ttf_win//:SDL_ttf_lib",
            "@SDL_ttf_win//:libfreetype",
        ],
        "@bazel_tools//src/conditions:linux_x86_64": SHARED_DEPS
    }),
    data = ["//assets:textures", "//assets:fonts"],
)
