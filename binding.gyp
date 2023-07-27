{
	'targets': [
		{
			'target_name': 'openstorm-radar',
			
			'cflags': [
                
			],
			
			'sources': [
				'src/binding.cpp',
                "<!@(node node-gyp-sources.js ../OpenStorm/Source/OpenStorm/Radar/)"
			]
		},
	]
}
