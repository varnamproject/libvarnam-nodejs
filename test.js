var v = require('./build/Release/lib.target/varnam');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file);
suggestions = varnam.transliterate("#mOhan");
suggestions.forEach(function(item){
   console.log(item);
});


str = varnam.reverseTransliterate(suggestions[0]);
console.log(str);

varnam.close();