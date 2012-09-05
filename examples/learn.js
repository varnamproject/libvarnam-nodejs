var v = require('bindings')('varnam.node');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learned");

for (i = 0; i < 10; i++) {
	varnam.learn(process.argv[2], function(err) {
		console.log(err);
	});
}

