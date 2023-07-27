# OpenStorm Radar Node JS Library
This library provides OpenStorm's radar processing capabilities to Node JS.
It uses the c++ files in the Radar source directory of OpenStorm.
The RadarData and RadarDataHolder classes are exposed to javascript. RadarData is the class that contains the actual data.
The data can be accessed by through radarData.buffer or radarData.bufferArray.
The rays are automatically interpolated to provide a uniform dense array of radar data.
The first dimension is the sweep the second is the rotation and the third is the distance away from the radar.
You will see the dimensions refereed to as sweep, theta, and radius in code.
There are 2 duplicated rays in each sweep that are used for padding for interpolation. In most cases they should be ignored or removed.
RadarDataHolder is a more advanced container for RadarData objects and supports loading multiple products as well as computing derived products. 
RadarDataHolder is multi threaded and asynchronous.

## Documentation
See [libtest.js](./libtest.js) for some examples.  
See [index.js](./__init__.py) for documentation of classes and methods.  
The javascript api mirrors the c++ one closely.
The [OpenStorm .h files](https://github.com/JordanSchlick/OpenStorm/tree/main/Source/OpenStorm/Radar) are well documented. [RadarData.h](https://github.com/JordanSchlick/OpenStorm/tree/main/Source/OpenStorm/Radar/RadarData.h) is especially helpfully.

## Building
The OpenStorm source code is a dependency and is required for building. The OpenStorm repository needs to be placed in the same directory as this module is located.  
Ex:  
```
Folder  
  ├OpenStorm  
  └openstorm-radar-node 
```
Run `git clone https://github.com/JordanSchlick/openstorm-radar-node` and `git clone https://github.com/JordanSchlick/OpenStorm` in the same directory do download them both.  
Your system needs to be have an environment capable of building native node js modules. See https://www.npmjs.com/package/node-gyp for more details.
  
Run `node-gyp clean configure build` inside openstorm-radar-node to build the module. Quick rebuilds can be done with `node-gyp build`

If the build completes successfully you should be able to import and use the module in node with `require("./openstorm-radar-node")`  

You may need to change the path in the require statement before you can import it depending on where it is located.
