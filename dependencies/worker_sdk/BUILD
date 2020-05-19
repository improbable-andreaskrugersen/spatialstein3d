cc_library(
  name = "headers",
  hdrs = glob(["headers/include/improbable/**"]),
  includes = ["headers/include"], # SDK headers are available with #include <improbable/...>
)

cc_library(
  name = "dependencies",
  srcs = select({
    "@bazel_tools//src/conditions:linux_x86_64": [
      "lib/libRakNetLibStatic.a",
      "lib/libssl.a",
      "lib/libz.a"
    ],
    "@bazel_tools//src/conditions:windows": [
      "lib/RakNetLibStatic.lib",
      "lib/ssl.lib",
      "lib/zlibstatic.lib"
    ]
  }),
)

cc_library(
  name = "worker_sdk",
  srcs = select({
    "@bazel_tools//src/conditions:linux_x86_64": [
      "lib/libimprobable_worker.a"
    ],
    "@bazel_tools//src/conditions:windows": [
      "lib/improbable_worker.lib"
    ]
  }),
  linkopts = select({
    "@bazel_tools//src/conditions:linux_x86_64": [
      "-ldl",
      "-lm",
      "-lpthread"
    ],
    "@bazel_tools//src/conditions:windows": [
      "-DEFAULTLIB:user32.lib",
      "-DEFAULTLIB:advapi32.lib"
    ],
    "//conditions:default": []
  }),
  deps = [":headers", ":dependencies"],
  visibility = ["//visibility:public"]
)
