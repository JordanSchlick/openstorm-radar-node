// @ts-check

let OpenStormRadar = require(".")


let radarData = new OpenStormRadar.RadarData()
console.log(radarData.pointer)

let time = performance.now()
radarData.loadNexradFile("../OpenStorm/Content/Data/Demo/KMKX_20220723_235820", OpenStormRadar.VolumeTypes.VOLUME_REFLECTIVITY)
time = performance.now() - time
console.log("Benchmark, loadNexradFile took " + time + "ms")

time = performance.now()
radarData.updateProperties()
time = performance.now() - time
console.log("Benchmark, updateProperties took " + time + "ms")

time = performance.now()
let rayInfos = radarData.getRayInfo()
time = performance.now() - time
console.log("Benchmark, getRayInfo took " + time + "ms to get " + rayInfos.length + " ray infos")

time = performance.now()
radarData.getSweepInfo()
time = performance.now() - time
console.log("Benchmark, getSweepInfo took " + time + "ms")

time = performance.now()
radarData.getStats()
time = performance.now() - time
console.log("Benchmark, getStats took " + time + "ms")

// radarData.bufferArray is a 3D array representation of the buffer
if(radarData.bufferArray){
	console.log("bufferArray size:",radarData.bufferArray.length, radarData.bufferArray[0].length, radarData.bufferArray[0][0].length)
	// ray info is also included for each ray
	console.log("first ray info:", radarData.bufferArray[0][0].rayInfo)
	console.log("first value:", radarData.bufferArray[0][0][0])
}


console.log("RadarData object:")
// undefine radarData.bufferArray so it doesn't log to console
radarData.bufferArray = undefined
console.log(radarData)


console.log("Get info:")

console.log("Stats:", radarData.getStats())
console.log("Sweep info:", radarData.getSweepInfo())


async function sleep(ms){
	return new Promise((resolve) => {
		setTimeout(resolve, ms)
	})
}


// RadarDataHolder is a more advanced way of loading data.
// It supports loading multiple products at once as well as computer products like storm relative velocity.
// It is asynchronous as well.
void (async function(){
	// allocate holder
	let radarDataHolder = new OpenStormRadar.RadarDataHolder()
	// define products to load
	let reflectivityProduct = radarDataHolder.getProduct(OpenStormRadar.VolumeTypes.VOLUME_REFLECTIVITY)
	let spectrumWidthProduct = radarDataHolder.getProduct(OpenStormRadar.VolumeTypes.VOLUME_SPECTRUM_WIDTH)
	let srvProduct = radarDataHolder.getProduct(OpenStormRadar.VolumeTypes.VOLUME_STORM_RELATIVE_VELOCITY)
	// load file
	radarDataHolder.load("../OpenStorm/Content/Data/Demo/KTLX20130531_231434_V06")
	console.log(radarDataHolder.getState())
	process.stdout.write("loading")
	// wait for it to be done loading asynchronously
	while(radarDataHolder.getState() == OpenStormRadar.RadarDataHolder.DATA_STATE_LOADING){
		process.stdout.write(".")
		await sleep(100)
	}
	console.log("\nloaded      ")
	console.log(radarDataHolder.getState())
	// get radar data from products
	let reflectivityData = reflectivityProduct?.getRadarData()
	let spectrumWidthData = spectrumWidthProduct?.getRadarData()
	let srvData = srvProduct?.getRadarData()
	console.log("is reflectivity loaded?", reflectivityProduct?.isLoaded(), "   is spectrumWidth loaded?", spectrumWidthProduct?.isLoaded(), "   is srv loaded?", srvProduct?.isLoaded())
	
	// log information about data
	console.log(reflectivityData?.getStats())
	console.log("reflectivityData length:", reflectivityData?.buffer?.length)
	console.log("spectrumWidthData length:", spectrumWidthData?.buffer?.length)
	console.log("srvData length:", srvData?.buffer?.length)

	console.log("finished");
})()