//@ts-check

// this file iterates over a directory and finds all c/c++ files to include in the build
// it is used in binding.gyp

let path = require("path")
let fs = require("fs")

async function getFiles(dir) {
	let directoryEntries = await fs.promises.readdir(dir, { withFileTypes: true });
	let files = await Promise.all(
		directoryEntries.map(dirent => {
			const fullPath = path.resolve(dir, dirent.name);
			return dirent.isDirectory() ? getFiles(fullPath) : fullPath;
		}),
	);
	return Array.prototype.concat(...files);
}

(async () => {
	try{
		let fromDirectory = process.argv[2]
		let files = (await getFiles(path.resolve(fromDirectory)))
			.filter(filename =>
				filename.endsWith(".cpp") ||
				//v.endsWith(".h") ||
				//v.endsWith(".hpp") ||
				filename.endsWith(".cc") ||
				filename.endsWith(".c"),
			)
			// .map(v => path.relative(__dirname, v))
			.map(v => v.replace(/\\/g, "/"))
			.join("' '");
		process.stdout.write("'")
		process.stdout.write(files)
		process.stdout.write("'")
		return files;
	}catch(e){
		// console.error(e)
	}
})();