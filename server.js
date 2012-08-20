var http = require('http');
var os = require('os')
if (os.type().toLowerCase() == 'darwin') {
  var addon = require('./build/Release/varnam');
}
else {
  var addon = require('./build/Release/lib.target/varnam');
}

var file="ml-unicode.vst";

var url = require('url');

var file = "ml-unicode.vst";

var varnam = new addon.Varnam(file);

http.createServer(function(req,res) {
    var input = url.parse(req.url,true).query.input;
        if(input === undefined){
           res.writeHead(404);
           res.write('Bad request 404\n');
           res.end();
        }else{
            var results = varnam.transliterate(input);
            results.push("test");
             res.writeHead(200, {'Content-Type': 'application/json; charset=utf-8;', 'Access-Control-Allow-Origin': '*'});
             json = JSON.stringify(results);
             res.write(json);
            res.end();
          }
    }).listen(3003);

console.log("Server is running in 3003");