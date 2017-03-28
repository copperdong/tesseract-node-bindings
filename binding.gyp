{
    "variables": {
        "major_version": "<!(node -pe 'v=process.versions.node.split(\".\"); v[0];')",
        "minor_version": "<!(node -pe 'v=process.versions.node.split(\".\"); v[1];')",
        "micro_version": "<!(node -pe 'v=process.versions.node.split(\".\"); v[2];')"
    },
    "targets": [
        {
            "target_name": "tesseract",
            "sources": [
                "src/tesseract.cc",
                "src/api.cc",
                "src/pix.cc"
            ],
            "libraries": [
                "<!@(pkg-config --libs tesseract)"
            ],
            "cflags": [
                "<!@(pkg-config --cflags tesseract)"
            ],
            "defines": [
                "NODE_VERSION_MAJOR=<(major_version)",
                "NODE_VERSION_MINOR=<(minor_version)",
                "NODE_VERSION_MICRO=<(micro_version)"
            ],
            "xcode_settings": {
                "OTHER_CFLAGS": [
                    "<!@(pkg-config --cflags tesseract)"
                ],
                "OTHER_LDFLAGS": []
            },
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "configurations": {
                "Debug": {
                  "conditions": [
                    ["OS==\"linux\"", {
                      "cflags": ["-coverage"],
                      "ldflags": ["-coverage"]
                    }],
                    ["OS==\"mac\"", {
                      "xcode_settings": {
                        "OTHER_CFLAGS": [
                          "-fprofile-arcs -ftest-coverage"
                        ],
                        "OTHER_LDFLAGS": [
                          "--coverage"
                        ]
                      }
                    }]
                  ]
                }
            }
        }
    ]
}
