#include <napi.h>
#include "../include/lua.hpp"
#include <iostream>
#include <string>
#include <vector>

#define B2STR(b)((b)?"true":"false")



std::string parseTable(lua_State *L){
	std::string data, s, tkey;
	data = "{";
	s = tkey = "";
	
	lua_pushnil(L);
	while(lua_next(L, -2) != 0){		
		if(luaL_typename(L, -2) == "number"){
			tkey = std::to_string(lua_tonumber(L, -2)).substr(0, std::to_string(lua_tonumber(L, -2)).find("."));
		}else if(luaL_typename(L, -2) == "string"){
			tkey = lua_tostring(L, -2);
		}
		
		if(luaL_typename(L, -1) == "number"){
			data +=  s + tkey + "=" + std::string(lua_tostring(L, -1));
			s = ",";
		}else if(luaL_typename(L, -1) == "string"){
			data +=  s + std::string(lua_tostring(L, -2)) + "='" +  std::string(lua_tostring(L, -1)) + "'";
			s = ",";
		}else if(luaL_typename(L, -1) == "boolean"){
			data +=  s + tkey + "=" +  std::string(B2STR(lua_toboolean(L, -1)));
			s = ",";
		}else if(luaL_typename(L, -1) == "table"){
			std::string sData = "UNKNOWN";
			sData = parseTable(L);			
			if(luaL_typename(L, -2) == "string"){
				data +=  s + tkey + "=" +  sData;
			}else{
				data +=  s + sData;
			}
			s = ",";
		}
		
		lua_pop(L, 1);		
	}
	
	data += "}";
	return data;	
}

void pmsg(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n", msg, lua_tostring(L, -1));
	exit(1);
}

std::string NluaDoString(std::string a){
    lua_State *L;
    L = luaL_newstate();   
    luaL_openlibs(L); 	
	if(luaL_dostring(L, (&*a.begin())))
		pmsg(L, (char *)"luaL_dostring() failed"); 
	std::string str = lua_tostring(L, -1);	
    lua_close(L);                        
	return str;
}

std::string NluaDoFile(std::string a){
    lua_State *L;
    L = luaL_newstate();   
    luaL_openlibs(L);      
	if(luaL_dofile(L, (&*a.begin())))
		pmsg(L, (char *)"luaL_dofile() failed"); 
	std::string str = lua_tostring(L, -1);	
    lua_close(L);                        
	return str;
}

std::string NluaCallFunction(std::string fileName, std::string funcName, std::vector<std::string> args , std::vector<std::string> argsType, int resLen){
    std::string jd = "{\"type\":\"\",\"data\":\"\"}";
	int resLenLocal = (int)resLen;
    lua_State *L;
    L = luaL_newstate();   
    luaL_openlibs(L);      	
    if(luaL_loadfile(L,(&*fileName.begin())) || lua_pcall(L, 0, LUA_MULTRET, 0))
		pmsg(L, (char *)"luaL_loadfile() failed");
    lua_getglobal(L, funcName.c_str());	
	for(int i = 0; i < args.size(); i++){
		if(argsType[i] == "n"){
			std::string num= args[i];
			lua_pushnumber(L, std::stod(num));
		}else{
			lua_pushstring(L, (args[i].c_str()));
		}
	}
    if(lua_pcall(L, (int)args.size(), (int)resLen, 0)){
		pmsg(L, (char *)"lua_pcall() failed");	
	}else{
		if((int)resLen != 0){
			std::string resp = "";
			if(lua_isnumber(L, -1)){
				resp = lua_tostring(L, -1);
				jd.insert(19,resp);
				jd.insert(9,"number");
				lua_pop(L, 1);	
				lua_close(L);
			}else if(lua_isstring(L, -1)){
				resp = lua_tostring(L, -1);
				jd.insert(19,resp);
				jd.insert(9,"string");
				lua_pop(L, 1);	
				lua_close(L);
			}else if(lua_isboolean(L, -1)){
				resp = B2STR(lua_toboolean(L, -1));
				jd.insert(19,resp);
				jd.insert(9,"boolean");
				lua_pop(L, 1);	
				lua_close(L);
			}else if(lua_isnil(L, -1)){
				resp = "nil";
				jd.insert(19,resp);
				jd.insert(9,"nil");
				lua_pop(L, 1);	
				lua_close(L);
			}else if(lua_istable(L, -1)){
				resp = "{}"	;
				resp = parseTable(L);
				jd.insert(19,resp);
				jd.insert(9,"table");
				lua_pop(L, 1);	
				lua_close(L);
			}
			return jd;
		}
	}
    lua_close(L);                        	
	return "";
}

class NluaAsync: public Napi::AsyncWorker{
	public:
		NluaAsync(const Napi::Env &env, int op, std::string data);
		NluaAsync(const Napi::Env &env, int op, std::string data, std::string funcName, std::vector<std::string> args , std::vector<std::string> argsType, int resLen);
		~NluaAsync(){};
		void Execute();
		void OnOK();
		void OnError(const Napi::Error& error);
		Napi::Promise::Deferred deferred;
	private:
		std::string res;
		std::string data;
		std::string funcName;
		std::vector<std::string> args;
		std::vector<std::string> argsType;
		int resLen;
		int op;
};

NluaAsync::NluaAsync(const Napi::Env &env, int op, std::string data) : Napi::AsyncWorker(env), 
			op(op), 
			data(data), 
			deferred(Napi::Promise::Deferred::New(env)),
			res(""){};
			
NluaAsync::NluaAsync(const Napi::Env &env, int op, std::string data, std::string funcName, std::vector<std::string> args, std::vector<std::string> argsType, int resLen) : Napi::AsyncWorker(env), 
			op(op), 
			data(data),
			funcName(funcName),	
			deferred(Napi::Promise::Deferred::New(env)), 
			args(args),
			argsType(argsType),
			resLen(resLen),
			res(""){};

void NluaAsync::Execute(){	
	switch(op){
		case 0:
			if (data == ""){
				Napi::AsyncWorker::SetError("Oops! Failed.");
				break;
			}		
			res = NluaDoFile(data);		
			break;
		case 1:
			res = NluaDoString(data);			
			break;
		case 2:
			if (data == "" || funcName == ""){
				Napi::AsyncWorker::SetError("Oops! Failed.");
				break;
			}		
			res = NluaCallFunction(data, funcName, args, argsType, resLen);			
			break;			
	}
}

void NluaAsync::OnOK(){
	deferred.Resolve(Napi::String::New(Env(), res));
}

void NluaAsync::OnError(const Napi::Error& error){
	deferred.Reject(error.Value());
}

Napi::Promise doFile(const Napi::CallbackInfo& info){
	NluaAsync* asyncWorker = new NluaAsync(info.Env(), 0, info[0].As<Napi::String>());
	Napi::Promise promise = asyncWorker->deferred.Promise();
	asyncWorker->Queue();
    return promise;	
}

Napi::Promise doString(const Napi::CallbackInfo& info){
	NluaAsync* asyncWorker = new NluaAsync(info.Env(), 1, info[0].As<Napi::String>());
	Napi::Promise promise = asyncWorker->deferred.Promise();
	asyncWorker->Queue();
    return promise;	
}

Napi::Promise callFunction(const Napi::CallbackInfo& info){	

    Napi::Array b = info[2].As<Napi::Array>();
	int len = (int)b.Length();
	std::vector<std::string> args(len); 
	std::vector<std::string> argsType(len);
	
	for(int i = 0; i< len; i++){
		Napi::Value v = b[i];
		if(v.IsNumber()){
			double value = (double)v.As<Napi::Number>();
			std::string valueStr = std::to_string(value);
			args[i] = valueStr;
			argsType[i] = "n";
		}else if(v.IsString()){
			std::string value = v.As<Napi::String>();	
			args[i] = value;
			argsType[i] = "s";
		}else if(v.IsBoolean()){
			bool value = v.As<Napi::Boolean>();	
			std::string valueStr = B2STR(value);
			args[i] = valueStr;
			argsType[i] = "b";
		}
    }
	
	NluaAsync* asyncWorker = new NluaAsync(info.Env(), 
			2, 
			info[0].As<Napi::String>(),
			info[1].As<Napi::String>(),
			args,
			argsType,
			info[3].As<Napi::Number>());
			
	Napi::Promise promise = asyncWorker->deferred.Promise();
	asyncWorker->Queue();
    return promise;	
}



Napi::Object Init(Napi::Env env, Napi::Object exports){
	exports.Set("doFile", Napi::Function::New(env, doFile));
	exports.Set("doString", Napi::Function::New(env, doString));
	exports.Set("callFunction", Napi::Function::New(env, callFunction));
	return exports;
}

NODE_API_MODULE(nlua, Init)
