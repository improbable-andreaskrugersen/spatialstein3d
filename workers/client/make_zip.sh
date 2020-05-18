#!/bin/bash
cd ../..
zip -j build/assembly/worker/client@Linux bazel-bin/workers/client/src/spatialstein3d
zip build/assembly/worker/client@Linux assets/*.png assets/*.ttf
