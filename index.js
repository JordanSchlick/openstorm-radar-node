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
			this.pointer = options.existingPointer
		}else{
			this.pointer = nativeModule.radarDataAllocate(options.maxSweeps, options.maxThetas, options.maxRadius)
		}
		
		// the following properties will be populated by nativeModule.radarDataUpdateProperties
		
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



module.exports = {
	nativeModule,
	RadarData,
	VolumeTypes,
}