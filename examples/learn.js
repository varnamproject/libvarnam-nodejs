var v = require('bindings')('varnam.node');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learned");

str = varnam.learn(process.argv[2])
console.log(str);
console.log("learned " + process.argv[2])

varnam.close()