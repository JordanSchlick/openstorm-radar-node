#include <node_api.h>
#include "./helpers.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/Nexrad.h"
#ifdef HDF5
#include "../../OpenStorm/Plugins/UnrealHDF5/Source/UnrealHDF5/hdf5/highfive/H5File.hpp"
#include "../../OpenStorm/Plugins/UnrealHDF5/Source/UnrealHDF5/hdf5/highfive/H5DataSet.hpp"
#include "../../OpenStorm/Plugins/UnrealHDF5/Source/UnrealHDF5/hdf5/highfive/H5DataSpace.hpp"
#endif
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

#ifdef HDF5

template <typename T>
static std::string listAttributes(T object, std::string prefix){
	std::string out = "";
	for(auto attributeName : object.listAttributeNames()){
		out += prefix + "(" + attributeName;
		auto attribute = object.getAttribute(attributeName);
		auto dataType = attribute.getDataType();
		auto dimensions = attribute.getSpace().getDimensions();
		out += ": " + dataType.string();
		auto dataTypeClass = dataType.getClass();
		try{
			if(dimensions.size() > 0){
				out += "Array[";
				for(int i = 0; i < dimensions.size(); i++){
					if(i > 0){
						out += ",";
					}
					out += std::to_string(dimensions[i]);
				}
				out += "]";
			}else{
				// out += " " + std::to_string(attribute.read<double>());
				if(dataTypeClass == HighFive::DataTypeClass::Integer){
					out += " " + std::to_string(attribute.read<int64_t>());
				}
				if(dataTypeClass == HighFive::DataTypeClass::Float){
					out += " " + std::to_string(attribute.read<double>());
				}
				if(dataTypeClass == HighFive::DataTypeClass::String){
					std::string str;
					if(dataType.isFixedLenStr()){
						// char* stringData = new char[dataType.getSize() + 1]{};
						// attribute.read(stringData, dataType);
						// str = std::string(stringData);
						// delete stringData
						str.resize(dataType.getSize());
						attribute.read(&str[0], dataType);
						str.resize(dataType.getSize() - 1);
					}else if(dataType.isVariableStr()){
						attribute.read(str);
					}
					out += " " + ('"' + str + '"');
				}
			}
		}catch(std::exception e){
			out += prefix + std::string("ERROR: ") + e.what() + "\n";
			fprintf(stderr, "%s %s\n", attributeName.c_str(), dataType.string().c_str());
			fprintf(stderr, "ERROR: %s\n", e.what());
		}
		out += ")\n";
	}
	return out;
}

static std::string walkH5(HighFive::Group group, int indentation){
	
	std::string out = "";
	char indentationString[101] = "";
	for(int i = 0; i < indentation * 2; i++){
		indentationString[i] = ' ';
	}
	indentationString[indentation * 2] = '\x00';
	
	out += listAttributes(group, indentationString);
	
	std::vector<std::string> keys = group.listObjectNames(HighFive::IndexType::NAME);
	for(std::string key : keys){
		HighFive::ObjectType type = group.getObjectType(key);
		out += indentationString;
		out += key;
		out += " ";
		if(type == HighFive::ObjectType::Group){
			out += "Group\n";
			// out += listAttributes(group.getGroup(key), indentationString + std::string("  "));
			out += walkH5(group.getGroup(key), indentation + 1);
		}else if(type == HighFive::ObjectType::Dataset){
			out += "Dataset\n";
			HighFive::DataSet dataset = group.getDataSet(key);
			
			out += indentationString + ("  datatype: " + dataset.getDataType().string() + "\n");
			
			std::vector<size_t> dimensions = dataset.getSpace().getDimensions();
			out += indentationString + std::string("  dimensions: [");
			for(int i = 0; i < dimensions.size(); i++){
				if(i > 0){
					out += ",";
				}
				out += std::to_string(dimensions[i]);
			}
			out += "]\n";
			
			auto dataTypeClass = dataset.getDataType().getClass();
			static size_t dataSize = 0;
			static int32_t* dataInt = NULL;
			static float* dataFloat = NULL;
			
			if(dataSize != dataset.getElementCount()){
				dataSize = dataset.getElementCount();
				if(dataInt != NULL){
					delete[] dataInt;
				}
				dataInt = new int32_t[dataSize];
				dataFloat = (float*)dataInt;
			}
			
			try{
				fprintf(stderr, "reading: %s\n", dataset.getPath().c_str());
				if(dataTypeClass == HighFive::DataTypeClass::Float){
					dataset.read(dataFloat);
				}else{
					dataset.read(dataInt);
				}
				
				out += indentationString + std::string("  data: [");
				for(size_t i = 0; i < std::min(dataSize, (size_t)10); i++){
					if(i > 0){
						out += ",";
					}
					if(dataTypeClass == HighFive::DataTypeClass::Float){
						out += std::to_string(dataFloat[i]);
					}else{
						out += std::to_string(dataInt[i]);
					}
				}
				if(dataSize > 10){
					out += ",...";
				}
				out += "]\n";
			}catch(std::exception e){
				out += indentationString + std::string("  ERROR: ") + e.what() + "\n";
				fprintf(stderr, "%s\n", dataset.getPath().c_str());
				fprintf(stderr, "ERROR: %s\n", e.what());
			}
			
			out += listAttributes(dataset, indentationString + std::string("  "));
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

#endif