#pragma once

#define strcasecmp strcmpi		// HACK
#include <cassowary/Cl.h>

#include <luabind/luabind.hpp>

#include <string>
#include <vector>

// TODO: Should inherit directly from ClAbstractVariable!!
class LuaClVariable: public ClFloatVariable{
friend class LuaClLinearExpression;
friend class LuaCassowary;
public:
	LuaClVariable(const luabind::object& obj, const std::string& key);
	virtual void SetName(string const &Name){assert(0 && "I know my name better.");}
	virtual double Value() const {
		return luabind::object_cast<double>(obj[key]);
	}
	virtual int IntValue() const {
		return int(Value() + 0.5);
	}
	virtual void SetValue(double val){
		obj[key] = val;
	}
	virtual void ChangeValue(double val){
		obj[key] = val; 
	}

	bool operator<(const LuaClVariable& right) const{
		if(this->obj == right.obj)
			return this->key < right.key;
		else
			return this->obj < right.obj;
	}
private:
//	ClVariable var;
	luabind::object obj;
	std::string key;
};

class LuaClLinearExpression{

	friend class LuaCassowary;

	class VarsCompareCheckEqual{
	public:
		bool operator()(const LuaClVariable& left, const LuaClVariable& right){
			if(!(left < right) && !(right < left)){
				assert(left.obj == right.obj && left.key == right.key);
			}// if equal
			return left < right;
		}
	};
public:
	LuaClLinearExpression(double num = 0.0):expr(num){}
	LuaClLinearExpression(const ClVariable& var):expr(var){}
	LuaClLinearExpression(const luabind::object& obj, const std::string& key)
		:expr(ClVariable(new LuaClVariable(obj, key))){}
	friend LuaClLinearExpression operator+(const LuaClLinearExpression& left, const LuaClLinearExpression& right){
		LuaClLinearExpression res;
		res.expr = left.expr + right.expr;
		return res;
	}
	friend LuaClLinearExpression operator-(const LuaClLinearExpression& left, const LuaClLinearExpression& right){
		LuaClLinearExpression res;
		res.expr = left.expr - right.expr;
		return res;
	}
	friend LuaClLinearExpression operator*(const LuaClLinearExpression& left, const LuaClLinearExpression& right){
		LuaClLinearExpression res;
		res.expr = left.expr * right.expr;
		return res;
	}
	friend LuaClLinearExpression operator/(const LuaClLinearExpression& left, const LuaClLinearExpression& right){
		LuaClLinearExpression res;
		res.expr = left.expr / right.expr;
		return res;
	}
	// add to dst_maps out vars if absent
	// otherwise - take our vars from dst_map
	//void harmonizeVars(std::map<std::pair<luabind::object, string>, ClVariable>& dst_map){
	//	ClLinearExpression::ClVarToCoeffMap& terms = expr.Terms();
	//	for(ClLinearExpression::ClVarToCoeffMap::iterator it = terms.begin(); it != terms.end(); ++it){
	//		LuaClVariable* lv = dynamic_cast<LuaClVariable*>(it->first.get_pclv());
	//			assert(lv);
	//		// add if absent
	//		if(dst_map.find(make_pair(lv->obj, lv->key)) == dst_map.end())
	//			dst_map[make_pair(lv->obj, lv->key)] = it->first;
	//		else{
	//			double coef = it->second;
	//			terms.erase(it);
	//			ClVariable var(lv);
	//			terms[var] = coef;
	//		}
	//	}// for
	//}
private:
	ClLinearExpression expr;
};

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
	void addConstraint(const LuaClLinearExpression& left, const LuaClLinearExpression& right, string sign);
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
