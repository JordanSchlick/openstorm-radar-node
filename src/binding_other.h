#include <node_api.h>
#include "./helpers.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/Nexrad.h"

#include <string>

static napi_value recompressNexradArchive(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[2];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 2)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [filename, filename]");
		return make_undefined(env);
	}
	// input file
	char filename[1024];
	size_t filenameLength = 0;
	napi_get_value_string_utf8(env, argv[1], filename, 1024, &filenameLength);
	std::string inFile = std::string(filename);
	// output file
	napi_get_value_string_utf8(env, argv[1], filename, 1024, &filenameLength);
	std::string outFile = std::string(filename);
	int result = Nexrad::RecompressArchive(inFile, outFile);
	return make_bool(env, result == 0);
}