//@ts-check

// this file includes optional defines based on conditions
// it is used in binding.gyp

let path = require("path")
let fs = require("fs")

// DEFINE_NAME exists path/to/something
if(process.argv[3] == "exists"){
	if(fs.existsSync(process.argv[4])){
		process.stdout.write(process.argv[2])
	}
}