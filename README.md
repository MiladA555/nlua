
# nlua

A tool for calling Lua scripts from Node.js



## Installation

Install nlua with npm

```bash
  npm i nlua
```

## Supported OS and Architectures

Linux-x64

Linux-arm64

Windows-x64


## Example for ES Modules

```javascript
//Execute a script
import nlua from 'nlua';

nlua.doFile("./scripts/script.lua")//Lua file path in ur project
.then(data => {
	console.log(data);
}).catch(err => {
	console.log("---------",err);
});
```

```javascript
//Execute a function in script
import nlua from 'nlua';

/* Lua script 
function x(a,b)	
	local df = a + b
	return df
end
*/

/**
 * Adds two values together and returns the result.
 * @param {string} "x" The first value is the function name
 * @param {[]} array The list of parameters to be passed to the function
 * Array containing elements: type can be {number} or {string} or {boolean} .
 * @param {number} 1 The third value can be 0 or 1, which is the length of the returned value
 */
nlua.callFunction("./scripts/script.lua","x",[100,200],1)//Lua file path in ur project
.then(data => {
	console.log(data);
}).catch(err => {
	console.log("---------",err);
});
```

## Example for CommonJS

```javascript
const nlua = require('nlua');

nlua.doFile("./scripts/script.lua")
.then(data => {
	console.log(data);
}).catch(err => {
	console.log("---------",err);
});
```
## Running Tests

To run tests, run the following commands

```bash
    git clone https://github.com/MiladA555/nlua
    cd nlua
    npm i
    npm run test1
    npm run test2
    npm run test1cjs
    npm run test2cjs   
```


## License

[ISC](https://choosealicense.com/licenses/isc/)

