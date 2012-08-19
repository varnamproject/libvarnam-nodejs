var varnam = require('./build/Release/lib.target/varnam');


// addon.runCallback("ml-unicode.vst","jijEsh",function(msg){
//   console.log(msg);
// });
var file="ml-unicode.vst";

str = varnam.transliterate(file, "mOhan");
console.log(str);

str = varnam.reverse_transliterate(file, str);
console.log(str);

// addon.runCallback(file,"mohaN",function(msg){
//   console.log(msg);
// });


// addon.runCallback(file,"jijEsh",function(msg){
//   console.log(msg);
// });