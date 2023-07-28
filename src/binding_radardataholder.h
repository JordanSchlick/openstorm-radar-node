#include <node_api.h>
#include "./helpers.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/RadarDataHolder.h"

#include <string>

void radarDataHolderDeallocateCallback(napi_env env, void* finalize_data, void* finalize_hint){
	delete (RadarDataHolder*)finalize_data;
}

static napi_value radarDataHolderAllocate(napi_env env, napi_callback_info info) {
	RadarDataHolder* radarDataHolder = new RadarDataHolder();
	
	napi_value objPointer;
	napi_create_object(env, &objPointer);
	napi_set_named_property(env, objPointer, "type", make_string_napi(env, "RadarDataHolder Pointer"));
	napi_wrap(env, objPointer, radarDataHolder, radarDataHolderDeallocateCallback, NULL, NULL);
	
	return objPointer;
}


static napi_value radarDataHolderDeallocate(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "RadarDataHolder pointer not defined");
		return make_undefined(env);
	}
	
	RadarDataHolder* radarDataHolder;
	napi_remove_wrap(env, argv[0], (void**)&radarDataHolder);
	if(radarDataHolder != NULL){
		delete radarDataHolder;
		return make_bool(env, 1);
	}else{
		return make_bool(env, 0);
	}
}



static napi_value radarDataHolderLoad(napi_env env, napi_callback_info info) {
	size_t argc = 3;
	napi_value argv[3];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc < 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder pointer, optional file name]");
		return make_undefined(env);
	}
	// radar data holder
	RadarDataHolder* radarDataHolder = (RadarDataHolder*)unwrap_pointer(env, argv[0]);
	if(radarDataHolder == NULL){
		return make_undefined(env);
	}
	
	if(argc >= 2){
		// filename
		char filename[1024];
		size_t filenameLength = 0;
		napi_get_value_string_utf8(env, argv[1], filename, 1024, &filenameLength);
		
		RadarFile file = {};
		file.path = std::string(filename);
		radarDataHolder->Load(file);
	}else{
		radarDataHolder->Load();
	}
	
	return make_bool(env, 1);
}

static napi_value radarDataHolderUnload(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder pointer]");
		return make_undefined(env);
	}
	// radar data holder
	RadarDataHolder* radarDataHolder = (RadarDataHolder*)unwrap_pointer(env, argv[0]);
	if(radarDataHolder == NULL){
		return make_undefined(env);
	}
	radarDataHolder->Unload();
	return make_bool(env, 1);
}

static napi_value radarDataHolderGetState(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder pointer]");
		return make_undefined(env);
	}
	// radar data holder
	RadarDataHolder* radarDataHolder = (RadarDataHolder*)unwrap_pointer(env, argv[0]);
	if(radarDataHolder == NULL){
		return make_undefined(env);
	}
	return make_int32(env, (int)radarDataHolder->state);
}

void radarDataHolderProductDeallocateCallback(napi_env env, void* data, void* hint){
	RadarDataHolder::ProductHolder* radarDataHolderProduct = (RadarDataHolder::ProductHolder*)data;
	radarDataHolderProduct->StopUsing();
}

static napi_value radarDataHolderGetProduct(napi_env env, napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 2)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder pointer, volume type]");
		return make_undefined(env);
	}
	// radar data holder
	RadarDataHolder* radarDataHolder = (RadarDataHolder*)unwrap_pointer(env, argv[0]);
	if(radarDataHolder == NULL){
		return make_undefined(env);
	}
	RadarDataHolder::ProductHolder* radarDataHolderProduct = radarDataHolder->GetProduct((RadarData::VolumeType)get_int32(env, argv[1]));
	radarDataHolderProduct->isFinal = true;
	radarDataHolderProduct->StartUsing();
	
	napi_value objPointer;
	napi_create_object(env, &objPointer);
	napi_set_named_property(env, objPointer, "type", make_string_napi(env, "RadarDataHolder::ProductHolder Pointer"));
	napi_wrap(env, objPointer, radarDataHolderProduct, radarDataHolderProductDeallocateCallback, NULL, NULL);
	
	return objPointer;
}

static napi_value radarDataHolderProductGetRadarData(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder::ProductHolder pointer]");
		return make_undefined(env);
	}
	// product holder
	RadarDataHolder::ProductHolder* radarDataHolderProduct = (RadarDataHolder::ProductHolder*)unwrap_pointer(env, argv[0]);
	if(radarDataHolderProduct == NULL){
		return make_undefined(env);
	}
	if(radarDataHolderProduct->radarData != NULL){
		radarDataHolderProduct->radarData->Decompress();
	}else{
		return make_bool(env, 0);
	}
	napi_value objPointer;
	napi_create_object(env, &objPointer);
	napi_set_named_property(env, objPointer, "type", make_string_napi(env, "RadarData Pointer"));
	// radar data is managed by the product holder and will be deleted upon gc
	// therefor link the product holder to prevent gc
	napi_set_named_property(env, objPointer, "parent", argv[0]);
	napi_wrap(env, objPointer, radarDataHolderProduct->radarData, NULL, NULL, NULL);
	return objPointer;
	return make_undefined(env);
}

static napi_value radarDataHolderProductIsLoaded(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder::ProductHolder pointer]");
		return make_undefined(env);
	}
	// product holder
	RadarDataHolder::ProductHolder* radarDataHolderProduct = (RadarDataHolder::ProductHolder*)unwrap_pointer(env, argv[0]);
	if(radarDataHolderProduct == NULL){
		return make_undefined(env);
	}
	return make_bool(env, radarDataHolderProduct->isLoaded);
}


static napi_value radarDataHolderProductStopUsing(napi_env env, napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarDataHolder::ProductHolder pointer]");
		return make_undefined(env);
	}
	// product holder
	RadarDataHolder::ProductHolder* radarDataHolderProduct;
	napi_remove_wrap(env, argv[0], (void**)&radarDataHolderProduct);
	if(radarDataHolderProduct != NULL){
		radarDataHolderProduct->StopUsing();
		return make_bool(env, 1);
	}else{
		return make_bool(env, 0);
	}
}