var v = require('./build/Release/lib.target/varnam');

var file = "ml-unicode.vst";

var varnam = new v.Varnam(file)
str = varnam.transliterate("mOhan");
console.log(str);

str = varnam.reverseTransliterate(str);
console.log(str);

varnam.close();