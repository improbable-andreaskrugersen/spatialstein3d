cc_library(
  name = "headers",
  hdrs = glob(["headers/include/improbable/**"]),
  includes = ["headers/include"], # SDK headers are available with #include <improbable/...>
)

cc_library(
  name = "dependencies",
  srcs = [
    "linux/lib/libRakNetLibStatic.a",
    "linux/lib/libssl.a",
    "linux/lib/libz.a"
  ],
)

cc_library(
  name = "worker_sdk",
  srcs = ["linux/lib/libimprobable_worker.a"],
  linkopts = [
    "-ldl",
    "-lm",
    "-lpthread"
  ],
  deps = [":headers", ":dependencies"],
  visibility = ["//visibility:public"]
)
