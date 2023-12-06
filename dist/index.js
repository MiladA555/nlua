import { createRequire } from 'module';
const require = createRequire(import.meta.url);
const nlua = require('../build/Release/nlua.node');

export default nlua;