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

console.log(radarData.getStats())
console.log(radarData.getSweepInfo())