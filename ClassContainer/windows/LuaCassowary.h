#pragma once

#define strcasecmp strcmpi		// HACK
#include <cassowary/Cl.h>

#include <luabind/luabind.hpp>

#include <string>
#include <vector>

class LuaCassowary
{
	struct lua_var{
		luabind::object obj;
		std::string key;
		lua_var(luabind::object obj, std::string key)
			:obj(obj), key(key){}
	};
public:
	LuaCassowary(void);
	~LuaCassowary(void);
	static void luabind(lua_State* L);

	void solve();			// update lua vars from Cassowary vars!

	void addStay(luabind::object obj, std::string key, double x);
	void addStay(luabind::object obj, std::string key1, double coef1, std::string key2, double coef2, double x);
	void addPointStay(luabind::object obj, std::string key1, std::string key2);
	void addEquation(luabind::object info1,
					 luabind::object info2,
					 double dx);
	//void editVar(luabind::object obj, std::string key1);	
	//void editPoint(luabind::object obj, std::string key1, std::string key2);

private:
	ClSimplexSolver solver;
	bool need_resolve;
	typedef std::pair<luabind::object, std::string> obj_key;
	friend bool operator<(const obj_key& k1, const obj_key& k2){
		if(k1.first == k2.first)
			return k1.second < k2.second;
		else
			return k1.first < k2.first;
	}
	std::map<obj_key, ClVariable> cl_vars;
	std::map<obj_key, ClConstraint*> cl_stays;
};
