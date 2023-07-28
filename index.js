//@ts-check

//@ts-ignore
var nativeModule = require('./build/Release/openstorm-radar.node');

class VolumeTypes{
	static VOLUME_NONE = -1
	static VOLUME_UNKNOWN = 0

	// raw volume types

	static VOLUME_REFLECTIVITY = 1
	static VOLUME_VELOCITY = 2
	static VOLUME_SPECTRUM_WIDTH = 3
	static VOLUME_CORELATION_COEFFICIENT = 4
	static VOLUME_DIFFERENTIAL_REFLECTIVITY = 5
	static VOLUME_DIFFERENTIAL_PHASE_SHIFT = 6

	// computed volume types

	static VOLUME_VELOCITY_DEALIASED = 102
	static VOLUME_ROTATION = 103
	static VOLUME_STORM_RELATIVE_VELOCITY = 104
}


class RadarData{
	/**
	 * stores radar data in a contiguous 3 dimensional buffer  
	 * See https://github.com/JordanSchlick/OpenStorm/blob/main/Source/OpenStorm/Radar/RadarData.h for more info  
	 * maxSweeps, maxSweeps, and maxRadius determine the size of the buffer used for the radar data  
	 * if left as 0 the size will be automatically determined based off of the first radar file loaded  
	 * @param {{maxSweeps?:number,maxThetas?:number,maxRadius?:number,existingPointer?:any,productHolder?:any}} [options] 
	 */
	constructor(options){
		/** @type {any} */
		options = options || {}
		options.maxSweeps = options.maxSweeps || 0
		options.maxThetas = options.maxThetas || 0
		options.maxRadius = options.maxRadius || 0
		if(options.productHolder){
			this.productHolder = options.productHolder
		}
		if(options.existingPointer){
			/** internal pointer to c++ data */
			this.pointer = options.existingPointer
		}else{
			/** internal pointer to c++ data */
			this.pointer = nativeModule.radarDataAllocate(options.maxSweeps, options.maxThetas, options.maxRadius)
		}
		
		
		
		
		/** number of full sweep disks in the buffer */ 
		this.sweepBufferCount = 0
		/** number of data points in theta rotations  
		 * there are 2 more than this count in the buffer that serve as padding for interpolation */
		this.thetaBufferCount = 0
		/** number of data points in the radius dimension */
		this.radiusBufferCount = 0
		/** size of the whole volume */
		this.fullBufferSize = 0
		/** size of an entire sweep */
		this.sweepBufferSize = 0
		/** size of one ray */
		this.thetaBufferSize = 0
		/**
		 * Raw float32 buffer for radar data  
		 * it has size [sweepCount, thetaCount + 2, radiusCount]  
		 * the theta dimension has an extra ray and the beginning and the end for padding
		 * @type {Float32Array | undefined} 
		 * */
		this.buffer = undefined
		// the preceding properties will be populated by nativeModule.radarDataUpdateProperties
		nativeModule.radarDataUpdateProperties(this.pointer, this)
		
	}
	
	/**
	 * force deallocation of the radar object
	 */
	deallocate(){
		nativeModule.radarDataDeallocate(this.pointer)
	}
	
	/**
	 * Loads a nexrad file from disk
	 * @param {string} filename path to nexrad file
	 * @param {number} volumeType VolumeTypes
	 */
	loadNexradFile(filename, volumeType){
		nativeModule.radarDataLoadFile(this.pointer, filename, volumeType)
		nativeModule.radarDataUpdateProperties(this.pointer, this)
		if(this.buffer){
			/**
			 * an indexable 3d array of the buffer  
			 * the padding rays are excluded
			 * @type {Float32Array[][]}
			 */
			this.bufferArray = []
			for(let sweep = 0; sweep < this.sweepBufferCount; sweep++){
				let sweepArray = []
				for(let theta = 1; theta <= this.thetaBufferCount; theta++){
					let location = this.sweepBufferSize * sweep + this.thetaBufferSize * theta
					sweepArray.push(this.buffer.subarray(location, location + this.thetaBufferSize))
				}
				this.bufferArray.push(sweepArray)
			}
		}
	}
	
	/**
	 * get object containing information about the radar volume
	 */
	getStats(){
		return nativeModule.radarDataGetStats(this.pointer)
	}
	
	/**
	 * get array of objects containing information about the sweeps in the radar volume
	 */
	getSweepInfo(){
		return nativeModule.radarDataGetSweepInfo(this.pointer)
	}
}


class RadarDataHolder {
	
	static DataStateUnloaded = 0
	static DataStateLoading = 1
	static DataStateLoaded = 2
	static DataStateFailed = 3
	
	/**
	 * a class that holds the radar products and related information
	 * it also manages loading in radar files and products asynchronously
	 */
	constructor(existingPointer=null, autoFree=true) {
		this.autoFree = autoFree
		if (existingPointer === null) {
			this.pointer = nativeModule.radarDataHolderAllocate()
		} else {
			this.pointer = existingPointer
		}
	}
	
	/**
	 * load a file or load new products if fileName is not specified
	 * this will keep existing data so call unload if changing files
	 */
	load(fileName) {
		
		this.fileName = fileName
		if (fileName === null) {
			/** internal pointer to c++ data */
			nativeModule.radarDataHolderLoad(this.pointer)
		} else {
			/** internal pointer to c++ data */
			nativeModule.radarDataHolderLoad(this.pointer, fileName)
		}
	}
	
	/** unload products and cancel loading */
	unload() {
		nativeModule.radarDataHolderUnload(this.pointer)
    }
    
    deallocate() {
        if (this.autoFree) {
            nativeModule.radarDataHolderDeallocate(this.pointer)
        }
    }
	
	/** get a product to be loaded or add it if it is not found, should be called before Load if adding new product */
	getProduct(volumeType) {
		let pointer = nativeModule.radarDataHolderGetProduct(this.pointer, volumeType)
		if (pointer) {
			return new RadarDataHolderProduct(pointer)
		}
    }
	
	/** get current loading state of this object. compare with the DataStates defined on this class*/
	getState() {
        return nativeModule.radarDataHolderGetState(this.pointer)
    }
}

class RadarDataHolderProduct {
	/**
	 * Holds info about a product and the radar data related to it.
	 * Created from RadarDataHolder
	 */
	constructor(existingPointer) {
		this.pointer = existingPointer
	}
	
	/**
	 * Get the radar data for this product.
	 */
	getRadarData() {
		console.log("getting ptr")
		let pointer = nativeModule.radarDataHolderProductGetRadarData(this.pointer)
		console.log("ptr", pointer)
		if (pointer) {
			// Radar data is managed by the holder so do not free it and pass this to prevent self from falling from scope which may take the native RadarData with it.
			return new RadarData({existingPointer:pointer, productHolder:this})
		}
	}
	
	/** True if the radar data for this product is ready. */
	isLoaded() {
		return nativeModule.radarDataHolderProductIsLoaded(this.pointer)
    }
	
	/**
	 * indicate that you are no longer using the radar data and it can be freed early  
	 * calloing this is not required and will be automatically done when this is garbage collected  
	 * the main product holder must also be unloaded for radar data to be freed  
	 */
	stopUsing(){
		nativeModule.radarDataHolderProductStopUsing(this.pointer)
	}
}


module.exports = {
	nativeModule,
	RadarData,
	VolumeTypes,
	RadarDataHolder,
	RadarDataHolderProduct,
}