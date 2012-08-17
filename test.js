var addon = require('./build/Release/lib.target/varnam');


// addon.runCallback("ml-unicode.vst","jijEsh",function(msg){
//   console.log(msg);
// });
var file="ml-unicode.vst";

addon.runCallback(file,"mohaN",function(msg){
  console.log(msg);
});


addon.runCallback(file,"jijEsh",function(msg){
  console.log(msg);
});