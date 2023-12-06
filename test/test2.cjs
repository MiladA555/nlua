const nlua = require('../build/Release/nlua.node');

nlua.callFunction("./lua/script2.lua","x",[1,2],1)
.then(data => {
	console.log(data);
}).catch(err => {
	console.log("---------",err);
});