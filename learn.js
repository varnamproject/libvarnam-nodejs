var os = require('os')
if (os.type().toLowerCase() == 'darwin') {
	var v = require('./build/Release/varnam');
}
else {
	var v = require('./build/Release/lib.target/varnam');
}

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learned");

str = varnam.learn(process.argv[2])
console.log(str);
console.log("learned " + process.argv[2])

varnam.close()