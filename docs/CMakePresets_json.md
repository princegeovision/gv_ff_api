# CMake Preset JSON File

## Intro

- EX

### JSON-Xcode

```json
{
    "version": 2,
    "configurePresets": [
      {
        "name": "vcpkg",
        "generator": "Xcode",
        "binaryDir": "${sourceDir}/build",
        "cacheVariables": {
          "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        }
      }
    ]
}
```

### JSON-Ninja
```json
{
    "version": 2,
    "configurePresets": [
      {
        "name": "vcpkg",
        "generator": "Ninja",
        "binaryDir": "${sourceDir}/build",
        "cacheVariables": {
          "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        }
      }
    ]
}
```
