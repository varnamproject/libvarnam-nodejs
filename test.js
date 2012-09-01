var v = require('./varnam');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learneddata");

for (i = 0; i < 100; i++) {
	varnam.transliterate("test " + i, function(err, result) {
		console.log(result);
	});

	// simulating delay
	// Without this, it throws segmentation fault
	var now = new Date().getTime();
  	while(new Date().getTime() < now + 1) {
   			// do nothing
  	}
}

