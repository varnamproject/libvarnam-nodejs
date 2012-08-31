var os = require('os')
if (os.type().toLowerCase() == 'darwin') {
	var v = require('./varnam');
}
else {
	var v = require('./varnam');
}

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learneddata");

for (i = 0; i < 20; i++) {
	// console.log(varnam.transliterateSync('mohan'));
varnam.transliterate("mohan " + i, function(err, result) {
	console.log(result);
	console.log(err);
	// console.log(varnam.getOpenHandles());
});

}

//console.log(varnam.getOpenHandles());

// suggestions.forEach(function(item){
//    console.log(item);
// });

// rtl = varnam.reverseTransliterate(suggestions[0]);
// console.log(rtl);

//varnam.close();
