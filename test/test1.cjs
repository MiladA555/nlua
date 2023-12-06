const nlua = require('../build/Release/nlua.node');

nlua.doFile("./lua/script1.lua")
.then(data => {
	console.log(data);
}).catch(err => {
	console.log("---------",err);
});

