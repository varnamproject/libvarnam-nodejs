var os = require('os')
if (os.type().toLowerCase() == 'darwin') {
	var v = require('./build/Release/varnam');
}
else {
	var v = require('./build/Release/lib.target/varnam');
}

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file, "learneddata");
suggestions = varnam.transliterate("mOhan");
suggestions.forEach(function(item){
   console.log(item);
});

rtl = varnam.reverseTransliterate(suggestions[0]);
console.log(rtl);

varnam.close();
