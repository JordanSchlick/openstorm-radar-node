#include <node_api.h>
#include "./helpers.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/Nexrad.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/Deps/hdf5/highfive/H5File.hpp"
#include "../../OpenStorm/Source/OpenStorm/Radar/Deps/hdf5/highfive/H5DataSet.hpp"
#include "../../OpenStorm/Source/OpenStorm/Radar/Deps/hdf5/highfive/H5DataSpace.hpp"
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


static std::string listAttributes(HighFive::DataSet object, std::string prefix){
	std::string out = "";
	for(auto attributeName : object.listAttributeNames()){
		out += prefix + "(" + attributeName;
		auto attribute = object.getAttribute(attributeName);
	}
}

static std::string walkH5(HighFive::Group group, int indentation){
	std::string out = "";
	char indentationString[101] = "";
	for(int i = 0; i < indentation * 2; i++){
		indentationString[i] = ' ';
	}
	indentationString[indentation * 2] = '\x00';
	auto keys = group.listObjectNames(HighFive::IndexType::NAME);
	for(auto key : keys){
		auto type = group.getObjectType(key);
		out += indentationString;
		out += key;
		if(type == HighFive::ObjectType::Group){
			out += "Group\n";
			out += walkH5(group.getGroup(key), indentation + 1);
		}else if(type == HighFive::ObjectType::Dataset){
			out += "Dataset\n";
		}else if(type == HighFive::ObjectType::Attribute){
			out += "Attribute\n";
		}else if(type == HighFive::ObjectType::DataSpace){
			out += "DataSpace\n";
		}else if(type == HighFive::ObjectType::File){
			out += "File\n";
		}else{
			out += "¯\\_(ツ)_/¯\n";
		}
	}
	return out;
}

static napi_value h5List(napi_env env, napi_callback_info info) {
	
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [file name]");
		return make_undefined(env);
	}
	// filename
	char filename[1024];
	size_t filenameLength = 0;
	napi_get_value_string_utf8(env, argv[0], filename, 1024, &filenameLength);
	
	
	HighFive::File file(filename, HighFive::File::ReadOnly);
	
	std::string list = walkH5(file.getGroup("/"), 0);
	return make_string_napi(env, list.c_str());
}