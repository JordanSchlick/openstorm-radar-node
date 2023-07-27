#include <node_api.h>

napi_value make_bool(napi_env env, int input) {
	napi_value out;
	napi_create_int32(env, input, &out);
	napi_coerce_to_bool(env, out, &out);
	return out;
}
napi_value make_string_napi(napi_env env, char* input) {
	napi_value out;
	napi_create_string_utf8(env, input, NAPI_AUTO_LENGTH, &out);
	return out;
}
napi_value make_int32(napi_env env, int input) {
	napi_value out;
	napi_create_int32(env, input, &out);
	return out;
}
napi_value make_double(napi_env env, double input) {
	napi_value out;
	napi_create_double(env, input, &out);
	return out;
}
napi_value make_undefined(napi_env env) {
	napi_value out;
	napi_get_undefined(env, &out);
	return out;
}
void* unwrap_pointer(napi_env env, napi_value object){
	void* ptr = NULL;
	napi_unwrap(env, object, &ptr);
	if(ptr == NULL){
		napi_throw_error(env, nullptr, "Wrapped pointer is null");
	}
	return ptr;
}

int get_int32(napi_env env, napi_value numberObject){
	int value;
	napi_get_value_int32(env, numberObject, &value);
	return value;
}

double get_double(napi_env env, napi_value numberObject){
	double value;
	napi_get_value_double(env, numberObject, &value);
	return value;
}