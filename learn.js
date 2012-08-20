var v = require('./build/Release/lib.target/varnam');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file);

str = varnam.learn(process.argv[2])
console.log(str);
console.log("learned " + process.argv[2])

varnam.close()