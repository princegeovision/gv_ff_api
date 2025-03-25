# Example of vcpkg.json

## Intro

- Collect what works for me previously
- name = vcpkg.json

### JSON File

```json
{
  "name": "gv_ff_api",
  "version-string": "0.1",
  "dependencies": [
    "ffmpeg",
    "fmt"
  ],
  "overrides": [
    {
      "name": "fmt",
      "version": "11.0.2"
    },
    {
      "name": "ffmpeg",
      "version": "4.4.3#3"
    }
  ]
}
```

#### Ex-JSON-riesling

```json
{
    "name": "riesling",
    "version-string": "0.1",
    "dependencies": [
        "args",
        "catch2",
        "eigen3",
        "fmt",
        "itk",
        {
            "name": "fftw3",
            "features": [ "threads", "avx2" ]
        }
    ]
}
```