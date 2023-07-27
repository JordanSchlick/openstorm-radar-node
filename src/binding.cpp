#include <node_api.h>
#define _USE_MATH_DEFINES 
#include "math.h"
#include <algorithm>


#include "binding_radardata.h"
// #include "binding_radardataholder.h"
// #include "binding_other.h"





// static PyMethodDef pyMethods[] = {

	
// 	{NULL, NULL, 0, NULL}        /* Sentinel */
// };

// static struct PyModuleDef pyModule = {
// 	PyModuleDef_HEAD_INIT,
// 	"openstorm_radar_native",   /* name of module */
// 	"", /* module documentation, may be NULL */
// 	-1,       /* size of per-interpreter state of the module,
// 				 or -1 if the module keeps state in global variables. */
// 	pyMethods
// };



// PyMODINIT_FUNC
// PyInit_openstorm_radar_native(void)
// {
// 	//printf("Initialized\n");
// 	Py_Initialize();
	
	
	
	
// 	PyObject* m;
// 	m = PyModule_Create(&pyModule);
// 	if (m == NULL)
// 		return NULL;
	
	
// 	RadarBufferObject_Type.tp_name = "openstorm_radar_native.RadarBufferType";
// 	RadarBufferObject_Type.tp_basicsize = sizeof(RadarBufferObject);
// 	RadarBufferObject_Type.tp_doc = PyDoc_STR("Radar Buffer Data");
// 	RadarBufferObject_Type.tp_new = PyType_GenericNew,
// 	RadarBufferObject_Type.tp_as_buffer = &RadarBuffer_BufferProcs;
//     RadarBufferObject_Type.tp_itemsize = 0;
// 	//RadarBufferObject_Type.tp_dealloc = (destructor)myobj_dealloc,
// 	//RadarBufferObject_Type.tp_repr = (reprfunc)myobj_repr,
// 	RadarBufferObject_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	
//     if (PyType_Ready(&RadarBufferObject_Type) < 0)
//         return NULL;
	
// 	Py_INCREF(&RadarBufferObject_Type);
//     if (PyModule_AddObject(m, "RadarBufferType", (PyObject *) &RadarBufferObject_Type) < 0) {
//         Py_DECREF(&RadarBufferObject_Type);
//         Py_DECREF(m);
//         return NULL;
//     }
	
	
// 	return m;
// }


napi_value Init(napi_env env, napi_value exports) {
	
	napi_value func1;
	
	// 	{"test",  test, METH_VARARGS,"test"},
	napi_create_function(env, "test", NAPI_AUTO_LENGTH, test, nullptr, &func1);
	napi_set_named_property(env, exports, "test", func1);
	
	// 	{"radarDataAllocate",  radarDataAllocate, METH_VARARGS, ""},
	napi_create_function(env, "radarDataAllocate", NAPI_AUTO_LENGTH, radarDataAllocate, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataAllocate", func1);
	
	// 	{"radarDataDeallocate",  radarDataDeallocate, METH_VARARGS, ""},
	napi_create_function(env, "radarDataDeallocate", NAPI_AUTO_LENGTH, radarDataDeallocate, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataDeallocate", func1);
	
	// 	{"radarDataLoadFile",  radarDataLoadFile, METH_VARARGS, ""},
	napi_create_function(env, "radarDataLoadFile", NAPI_AUTO_LENGTH, radarDataLoadFile, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataLoadFile", func1);
	
	// 	{"radarDataUpdateProperties",  radarDataUpdateProperties, METH_VARARGS, ""},
	napi_create_function(env, "radarDataUpdateProperties", NAPI_AUTO_LENGTH, radarDataUpdateProperties, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataUpdateProperties", func1);
	
	// 	{"radarDataGetStats",  radarDataGetStats, METH_VARARGS, ""},
	napi_create_function(env, "radarDataGetStats", NAPI_AUTO_LENGTH, radarDataGetStats, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataGetStats", func1);
	
	// 	{"radarDataGetSweepInfo",  radarDataGetSweepInfo, METH_VARARGS, ""},
	napi_create_function(env, "radarDataGetSweepInfo", NAPI_AUTO_LENGTH, radarDataGetSweepInfo, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataGetSweepInfo", func1);
	
	// 	{"radarDataRadarSpaceForLocation",  radarDataRadarSpaceForLocation, METH_VARARGS, ""},
	napi_create_function(env, "radarDataRadarSpaceForLocation", NAPI_AUTO_LENGTH, radarDataRadarSpaceForLocation, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataRadarSpaceForLocation", func1);
	
	// 	{"radarDataGetPixelForRadarSpace",  radarDataGetPixelForRadarSpace, METH_VARARGS, ""},
	napi_create_function(env, "radarDataGetPixelForRadarSpace", NAPI_AUTO_LENGTH, radarDataGetPixelForRadarSpace, nullptr, &func1);
	napi_set_named_property(env, exports, "radarDataGetPixelForRadarSpace", func1);
	
		
		
	// 	{"radarDataHolderAllocate",  radarDataHolderAllocate, METH_VARARGS, ""},
	
	// 	{"radarDataHolderDeallocate",  radarDataHolderDeallocate, METH_VARARGS, ""},
	
	// 	{"radarDataHolderLoad",  radarDataHolderLoad, METH_VARARGS, ""},
	
	// 	{"radarDataHolderUnload",  radarDataHolderUnload, METH_VARARGS, ""},
	
	// 	{"radarDataHolderGetState",  radarDataHolderGetState, METH_VARARGS, ""},
	
	// 	{"radarDataHolderGetProduct",  radarDataHolderGetProduct, METH_VARARGS, ""},
	
	// 	{"radarDataHolderProductGetRadarData",  radarDataHolderProductGetRadarData, METH_VARARGS, ""},
	
	// 	{"radarDataHolderProductIsLoaded",  radarDataHolderProductIsLoaded, METH_VARARGS, ""},
	
	// 	{"radarDataHolderProductStartUsing",  radarDataHolderProductStartUsing, METH_VARARGS, ""},
	
	// 	{"radarDataHolderProductStopUsing",  radarDataHolderProductStopUsing, METH_VARARGS, ""},
	
		
	// 	{"recompressNexradArchive",  recompressNexradArchive, METH_VARARGS, ""},
	
	return exports;
}

NAPI_MODULE(openstorm_radar, Init)