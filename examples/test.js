var v = require('bindings')('varnam.node');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learneddata");

for (i = 0; i < 3000; i++) {
	varnam.transliterate("test " + i, function(err, result) {
		console.log(result);
	});
}

