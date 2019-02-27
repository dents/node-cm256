{
    "targets": [{
        "target_name": "cm256",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "cflags": [ "-msse4.1" ],
        "cflags_cc": [ "-msse4.1" ],
        "sources": [
			"cppsrc/cm256.h",
			"cppsrc/cm256.cpp",
			"cppsrc/gf256.h",
			"cppsrc/gf256.cpp",
            "cppsrc/main.h",
            "cppsrc/main.cpp"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'libraries': [],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}