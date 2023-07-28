#include <node_api.h>
#include "./helpers.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/RadarData.h"
#include "../../OpenStorm/Source/OpenStorm/Radar/Globe.h"





static napi_value test(napi_env env, const napi_callback_info info) {
	printf("Get Beaned\n");
	return make_bool(env, 1);
}


void radarDataDeallocateCallback(napi_env env, void* finalize_data, void* finalize_hint){
	delete (RadarData*)finalize_data;
}

static napi_value radarDataAllocate(napi_env env, const napi_callback_info info) {
	size_t argc = 3;
	napi_value argv[3];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	
	// max sweeps
	int maxSweeps = 0;
	if (argc >= 1){
		napi_get_value_int32(env, argv[0], &maxSweeps);
	}
	// max thetas
	int maxThetas = 0;
	if (argc >= 2){
		napi_get_value_int32(env, argv[1], &maxThetas);
	}
	// max radius
	int maxRadius = 0;
	if (argc >= 3){
		napi_get_value_int32(env, argv[2], &maxRadius);
	}
	
	RadarData* radarData = new RadarData();
	radarData->sweepBufferCount = maxSweeps;
	radarData->thetaBufferCount = maxThetas;
	radarData->radiusBufferCount = maxRadius;
	
	napi_value objPointer;
	napi_create_object(env, &objPointer);
	napi_set_named_property(env, objPointer, "type", make_string_napi(env, "RadarData Pointer"));
	napi_wrap(env, objPointer, radarData, radarDataDeallocateCallback, NULL, NULL);
	return objPointer;
}



static napi_value radarDataDeallocate(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "RadarData pointer not defined");
		return make_undefined(env);
	}
	
	RadarData* radarData;
	napi_remove_wrap(env, argv[0], (void**)&radarData);
	if(radarData != NULL){
		delete radarData;
	}
	return make_bool(env, 1);
}

static napi_value radarDataLoadFile(napi_env env, const napi_callback_info info) {
	size_t argc = 3;
	napi_value argv[3];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 3)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer, file name, volume type]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	// filename
	char filename[1024];
	size_t filenameLength = 0;
	napi_get_value_string_utf8(env, argv[1], filename, 1024, &filenameLength);
	// volume type
	int volumeType = get_int32(env, argv[2]);
	
	void* nexradData = RadarData::ReadNexradData(filename);
	radarData->LoadNexradVolume(nexradData, (RadarData::VolumeType)volumeType);
	RadarData::FreeNexradData(nexradData);
	return make_bool(env, 1);
}

static napi_value radarDataUpdateProperties(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 2)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer, out object]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	// javascript class
	napi_value outObj = argv[1];
	
	
	napi_set_named_property(env, outObj, "sweepBufferCount", make_int32(env, radarData->sweepBufferCount));
	napi_set_named_property(env, outObj, "thetaBufferCount", make_int32(env, radarData->thetaBufferCount));
	napi_set_named_property(env, outObj, "radiusBufferCount", make_int32(env, radarData->radiusBufferCount));
	napi_set_named_property(env, outObj, "fullBufferSize", make_int32(env, radarData->fullBufferSize));
	napi_set_named_property(env, outObj, "sweepBufferSize", make_int32(env, radarData->sweepBufferSize));
	napi_set_named_property(env, outObj, "thetaBufferSize", make_int32(env, radarData->thetaBufferSize));
	if(radarData->buffer != NULL){
		napi_value arrayBuffer;
		napi_create_external_arraybuffer(env, radarData->buffer, radarData->fullBufferSize * 4, NULL, NULL, &arrayBuffer);
		// napi_set_named_property(env, outObj, "rawBuffer", arrayBuffer);
		// attach radar data pointer to the buffer so that it won't get garbage collected while the buffer is in use
		napi_set_named_property(env, arrayBuffer, "pointer", argv[0]);
		napi_value floatBuffer;
		napi_create_typedarray(env, napi_float32_array, radarData->fullBufferSize, arrayBuffer, 0, &floatBuffer);
		napi_set_named_property(env, outObj, "buffer", floatBuffer);
	}else{
		napi_set_named_property(env, outObj, "buffer", make_undefined(env));
	}
	
	return make_bool(env, 1);
}

static napi_value radarDataGetStats(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	
	napi_value obj;
	napi_create_object(env, &obj);
	napi_set_named_property(env, obj, "innerDistance", make_double(env, radarData->stats.innerDistance));
	napi_set_named_property(env, obj, "minValue", make_double(env, radarData->stats.minValue));
	napi_set_named_property(env, obj, "maxValue", make_double(env, radarData->stats.maxValue));
	napi_set_named_property(env, obj, "pixelSize", make_double(env, radarData->stats.pixelSize));
	napi_set_named_property(env, obj, "beginTime", make_double(env, radarData->stats.beginTime));
	napi_set_named_property(env, obj, "endTime", make_double(env, radarData->stats.endTime));
	napi_set_named_property(env, obj, "noDataValue", make_double(env, radarData->stats.noDataValue));
	napi_set_named_property(env, obj, "boundRadius", make_double(env, radarData->stats.boundRadius));
	napi_set_named_property(env, obj, "boundUpper", make_double(env, radarData->stats.boundUpper));
	napi_set_named_property(env, obj, "boundLower", make_double(env, radarData->stats.boundLower));
	napi_set_named_property(env, obj, "latitude", make_double(env, radarData->stats.latitude));
	napi_set_named_property(env, obj, "longitude", make_double(env, radarData->stats.longitude));
	napi_set_named_property(env, obj, "altitude", make_double(env, radarData->stats.altitude));
	napi_set_named_property(env, obj, "volumeType", make_int32(env, radarData->stats.volumeType));
	
	return obj;
}


static napi_value radarDataGetSweepInfo(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	
	napi_value sweepArray;
	napi_create_array_with_length(env, radarData->sweepBufferCount, &sweepArray);
	if(radarData->sweepInfo != NULL){
		for(int i = 0; i < radarData->sweepBufferCount; i++){
			napi_value info;
			napi_create_object(env, &info);
			napi_set_named_property(env, info, "elevation", make_double(env, radarData->sweepInfo[i].elevationAngle));
			napi_set_named_property(env, info, "id", make_int32(env, radarData->sweepInfo[i].id));
			napi_set_named_property(env, info, "actual_ray_count", make_int32(env, radarData->sweepInfo[i].actualRayCount));
			napi_set_element(env, sweepArray, i, info);
		}
	}
	return sweepArray;
}

static napi_value radarDataGetRayInfo(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 1)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	
	napi_value rayArray;
	int rayCount = radarData->sweepBufferCount * (radarData->thetaBufferCount + 2);
	napi_create_array_with_length(env, rayCount, &rayArray);
	if(radarData->sweepInfo != NULL){
		for(int i = 0; i < rayCount; i++){
			RadarData::RayInfo* rayInfo = &radarData->rayInfo[i];
			napi_value info;
			napi_create_object(env, &info);
			// these should be replaced with napi_define_properties to improve performance
			napi_set_named_property(env, info, "interpolated", make_bool(env, rayInfo->interpolated));
			napi_set_named_property(env, info, "actualAngle", make_double(env, rayInfo->actualAngle));
			napi_set_named_property(env, info, "closestTheta", make_int32(env, rayInfo->closestTheta));
			napi_set_named_property(env, info, "nextTheta", make_int32(env, rayInfo->nextTheta));
			napi_set_named_property(env, info, "previousTheta", make_int32(env, rayInfo->previousTheta));
			napi_set_named_property(env, info, "sweep", make_int32(env, rayInfo->sweep));
			napi_set_named_property(env, info, "theta", make_int32(env, rayInfo->theta));
			napi_set_element(env, rayArray, i, info);
		}
	}
	return rayArray;
}

static napi_value radarDataRadarSpaceForLocation(napi_env env, const napi_callback_info info) {
	size_t argc = 4;
	napi_value argv[4];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 4)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer, latitude, longitude, altitude]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	// latitude
	double latitude = get_double(env, argv[1]);
	// longitude
	double longitude = get_double(env, argv[2]);
	// altitude
	double altitude = get_double(env, argv[3]);
	
	// radar data not loaded
	if(radarData->sweepInfo == NULL){
		return make_undefined(env);
	}
	
	
	
	
	
	Globe globe = {};
	globe.SetCenter(0, 0, -globe.surfaceRadius - radarData->stats.altitude);
	globe.SetTopCoordinates(radarData->stats.latitude, radarData->stats.longitude);
	
	auto radarSpaceLocation = globe.GetPointDegrees(latitude, longitude, altitude);
	
	napi_value outObject;
	napi_create_object(env, &outObject);
	napi_set_named_property(env, outObject, "x", make_double(env, radarSpaceLocation.x));
	napi_set_named_property(env, outObject, "y", make_double(env, radarSpaceLocation.y));
	napi_set_named_property(env, outObject, "z", make_double(env, radarSpaceLocation.z));
	return outObject;
}

static napi_value radarDataGetPixelForRadarSpace(napi_env env, const napi_callback_info info) {
	size_t argc = 4;
	napi_value argv[4];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
	if (argc != 4)
	{
		napi_throw_error(env, nullptr, "Invalid number of arguments. [RadarData pointer, x, y, z]");
		return make_undefined(env);
	}
	// radar data
	RadarData* radarData = (RadarData*)unwrap_pointer(env, argv[0]);
	if(radarData == NULL){
		return make_undefined(env);
	}
	
	// radar space x
	double x = get_double(env, argv[1]);
	// radar space y
	double y = get_double(env, argv[2]);
	// radar space z
	double z = get_double(env, argv[3]);
	
	// radar data not loaded
	if(radarData->sweepInfo == NULL){
		return make_undefined(env);
	}
	
	
	
	float radius = sqrt(x * x + y * y + z * z);
	float theta = atan2(y, x);
	float phi = acos(z / radius);
	
	// normalize
	radius = radius;
	theta = (theta / (M_PI * 2.0)) + 0.25;
	theta = theta < 0 ? theta + 1 : theta;
	phi = (0.5 - phi / M_PI);
	
	// convert to volume texture coordinates
	radius = radius / radarData->stats.pixelSize - radarData->stats.innerDistance;
	theta = theta * radarData->thetaBufferCount/* + 1*/;
	// convert phi to volume sweep index
	phi = phi * 180;
	int firstIndex = radarData->sweepBufferCount;
	int lastIndex = -1;
	for (int sweepIndex = 0; sweepIndex < radarData->sweepBufferCount; sweepIndex++) {
		RadarData::SweepInfo &info = radarData->sweepInfo[sweepIndex];
		if(info.id != -1){
			firstIndex = std::min(firstIndex, sweepIndex);
			lastIndex = std::max(lastIndex, sweepIndex);
		}
	}
	//fprintf(stderr, "phi %f %f %f\n",phi, radarData->sweepInfo[firstIndex].elevationAngle, radarData->sweepInfo[lastIndex].elevationAngle);
	if(phi < radarData->sweepInfo[firstIndex].elevationAngle){
		phi = -INFINITY;
	}else if (phi > radarData->sweepInfo[lastIndex].elevationAngle){
		phi = INFINITY;
	}else{
		for (int sweepIndex = firstIndex + 1; sweepIndex < radarData->sweepBufferCount; sweepIndex++) {
			RadarData::SweepInfo &info2 = radarData->sweepInfo[sweepIndex];
			if(phi <= info2.elevationAngle){
				RadarData::SweepInfo &info1 = radarData->sweepInfo[sweepIndex - 1];
				// set phi to sweep index
				phi = (phi - info1.elevationAngle) / (info2.elevationAngle - info1.elevationAngle) + 0.5f + sweepIndex;
				break;
			}
		}
	}
	bool isInVolume = radius >= 0 && radius < radarData->radiusBufferCount && isfinite(phi);
	float pixelThetaWidth = (radius + radarData->stats.innerDistance) * radarData->stats.pixelSize * 2 * M_PI / (float)radarData->thetaBufferCount;
	
	
	napi_value outObject;
    napi_create_object(env, &outObject);
    napi_set_named_property(env, outObject, "isInVolume", make_bool(env, isInVolume));
    napi_set_named_property(env, outObject, "sweep", make_double(env, phi));
    napi_set_named_property(env, outObject, "theta", make_double(env, theta));
    napi_set_named_property(env, outObject, "radius", make_double(env, radius));
    napi_set_named_property(env, outObject, "pixelRadiusLength", make_double(env, radarData->stats.pixelSize));
    napi_set_named_property(env, outObject, "pixelThetaWidth", make_double(env,  pixelThetaWidth));
	
	if(isInVolume && radarData->buffer != NULL){
		float value = radarData->buffer[(int)phi * radarData->sweepBufferSize + ((int)theta + 1) * radarData->thetaBufferSize + (int)radius];
		// value of pixel
		napi_set_named_property(env, outObject, "value", make_double(env,  value));
	}
    
    return outObject;
}

