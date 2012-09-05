var v = require('bindings')('varnam.node');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learned");

for (i = 0; i < 10; i++) {
	varnam.transliterate("navaneeth", function(err, result) {
		console.log(result);
	});
}

