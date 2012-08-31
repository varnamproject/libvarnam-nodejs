var os = require('os')
if (os.type().toLowerCase() == 'darwin') {
	var v = require('./build/Release/varnam');
}
else {
	var v = require('./build/Release/lib.target/varnam');
}

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learneddata");

for (i = 0; i < 2000; i++) {
varnam.transliterate("mOhan", function(err, result) {
	console.log(varnam.getOpenHandles());
});

}

console.log(varnam.getOpenHandles());

// suggestions.forEach(function(item){
//    console.log(item);
// });

// rtl = varnam.reverseTransliterate(suggestions[0]);
// console.log(rtl);

// varnam.close();
