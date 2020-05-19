cd ../..
New-Item -Force -ItemType Directory $env:TEMP/spatialstein3d-client
New-Item -Force -ItemType Directory $env:TEMP/spatialstein3d-client/assets
Copy-Item -Force bazel-bin/workers/client/src/spatialstein3d.exe $env:TEMP/spatialstein3d-client
Copy-Item -Force bazel-bin/workers/client/src/*.dll $env:TEMP/spatialstein3d-client
Copy-Item -Recurse -Force assets/*.png $env:TEMP/spatialstein3d-client/assets
Copy-Item -Recurse -Force assets/*.ttf $env:TEMP/spatialstein3d-client/assets

New-Item -Force -ItemType Directory build/assembly/worker
Compress-Archive -Force -Path $env:TEMP/spatialstein3d-client/* -DestinationPath build/assembly/worker/client@Windows.zip

Remove-Item -Force -Recurse -Path $env:TEMP/spatialstein3d-client
