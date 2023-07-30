{
	'targets': [
		{
			'target_name': 'openstorm-radar',
			
			'cflags': [
				
			],
			"defines": [
				"<!@(node node-gyp-defines.js HDF5 exists ../OpenStorm/Plugins/UnrealHDF5/Source/UnrealHDF5/hdf5/)"
			],
			
			'sources': [
				'src/binding.cpp',
				"<!@(node node-gyp-sources.js ../OpenStorm/Source/OpenStorm/Radar/)",
				"<!@(node node-gyp-sources.js ../OpenStorm/Plugins/UnrealHDF5/Source/UnrealHDF5/hdf5/)"
			]
		},
	]
}
