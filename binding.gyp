{
    "targets": [{
        "target_name": "nlua",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "csrc/main.cpp"
        ],
        'include_dirs': [
			"<!@(node -p \"require('node-addon-api').include\")",
			"include"
        ],
		"library_dirs": [
			"lib"
		],
		"libraries": [
			"<(module_root_dir)/lib/liblua-<!(node -e \"console.log(process.platform)\")-<!(node -e \"console.log(process.arch)\").a"
		],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}