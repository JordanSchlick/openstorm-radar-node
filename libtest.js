// @ts-check

let OpenStormRadar = require(".")


let radarData = new OpenStormRadar.RadarData()
console.log(radarData.pointer)

radarData.loadNexradFile("../OpenStorm/Content/Data/Demo/KMKX_20220723_235820", OpenStormRadar.VolumeTypes.VOLUME_REFLECTIVITY)


console.log("bufferArray size: ",radarData.bufferArray?.length, radarData.bufferArray?.[0].length, radarData.bufferArray?.[0][0].length)

// undefine it so it doesn't log to console
// it is a 3D array representation of the buffer
radarData.bufferArray = undefined
console.log(radarData)

//@ts-ignore
console.log(radarData.buffer?.buffer?.pointer)

console.log(radarData.getStats())
console.log(radarData.getSweepInfo())


async function sleep(ms){
	new Promise((resolve) => {
		setTimeout(resolve, ms)
	})
}

void (async function(){
	let radarDataHolder = new OpenStormRadar.RadarDataHolder()
	let reflectivityProduct = radarDataHolder.getProduct(OpenStormRadar.VolumeTypes.VOLUME_REFLECTIVITY)
	let rotationProduct = radarDataHolder.getProduct(OpenStormRadar.VolumeTypes.VOLUME_ROTATION)
	let srvProduct = radarDataHolder.getProduct(OpenStormRadar.VolumeTypes.VOLUME_STORM_RELATIVE_VELOCITY)
	radarDataHolder.load("../OpenStorm/Content/Data/Demo/KTLX20130531_231434_V06")
	console.log(radarDataHolder.getState())
	while(radarDataHolder.getState() == OpenStormRadar.RadarDataHolder.DataStateLoading){
		process.stdout.write("loading...\r")
		sleep(100)
	}
	console.log("loaded      ")
	console.log(radarDataHolder.getState())
	let reflectivityData = reflectivityProduct?.getRadarData()
	let rotationData = rotationProduct?.getRadarData()
	let srvData = srvProduct?.getRadarData()
	console.log("got radar data")
	sleep(100)
	console.log("is reflectivity loaded?", reflectivityProduct?.isLoaded(), "   is rotation loaded?", rotationProduct?.isLoaded(), "   is srv loaded?", srvProduct?.isLoaded())

	console.log(reflectivityData?.getStats())
	console.log("reflectivityData length:", reflectivityData?.buffer?.length)
	console.log("reflectivityData length:", rotationData?.buffer?.length)
	console.log("reflectivityData length:", srvData?.buffer?.length)

	console.log("finished");
})()