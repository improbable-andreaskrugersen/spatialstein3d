cc_library(
    name = "headers",
    hdrs = glob(["include/*.h"]),
    includes = ["include"],
    strip_include_prefix = "include",
    include_prefix = "SDL2",
    visibility = ["//visibility:public"],
)

cc_import(
    name = "SDL_lib",
    interface_library = "lib/x64/SDL2.lib",
    shared_library = "lib/x64/SDL2.dll",
    visibility = ["//visibility:public"],
)

cc_import(
    name = "SDL_image_lib",
    interface_library = "lib/x64/SDL2_image.lib",
    shared_library = "lib/x64/SDL2_image.dll",
    visibility = ["//visibility:public"],
)

cc_import(
    name = "SDL_ttf_lib",
    interface_library = "lib/x64/SDL2_ttf.lib",
    shared_library = "lib/x64/SDL2_ttf.dll",
    visibility = ["//visibility:public"],
)

cc_import(
    name = "libfreetype",
    shared_library = "lib/x64/libfreetype-6.dll",
    visibility = ["//visibility:public"],
)

cc_import(
    name = "libpng",
    shared_library = "lib/x64/libpng16-16.dll",
    visibility = ["//visibility:public"],
)
