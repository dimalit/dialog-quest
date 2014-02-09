#pragma once

#define strcasecmp strcmpi		// HACK
#include <cassowary/Cl.h>

#include <luabind/luabind.hpp>

#include <string>
#include <vector>
#include <iostream>

#include <cfloat>

// TODO: Should inherit directly from ClAbstractVariable!!
class LuaClVariable: public ClFloatVariable{
friend class LuaClLinearExpression;
friend class LuaCassowary;
public:
	LuaClVariable(const luabind::object& obj, const std::string& key);
	virtual void SetName(string const &Name){assert(0 && "I know my name better.");}

	// return value obtained from Lua but not in the simplex table
	// TODO: This was experimental and probably is unneeded
	// TODO: Find why Cassowary fucks my Edit vars!! (table 3x3)
	virtual double PendingValue() const {
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj_ref);
		luabind::object obj(luabind::from_stack(L, -1));
		double val = luabind::object_cast<double>(obj[key]);
		assert(_finite(val));
		return val;
	}

	// get value as in the table
	virtual double Value() const {
//		assert((float)value_in_table == (float)PendingValue()); fails in GetExternalVars
		return value_in_table;
	}
	virtual int IntValue() const {
		return int(Value() + 0.5);
	}
	virtual void SetValue(double val){
		assert(_finite(val));
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj_ref);
		luabind::object obj(luabind::from_stack(L, -1));
		std::cout << "Setting " << *this << " = " << val << std::endl;
		obj[key] = val;
		value_in_table = val;
	}
	virtual void ChangeValue(double val){
		assert(_finite(val));
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj_ref);
		luabind::object obj(luabind::from_stack(L, -1));
		std::cout << "Changing " << *this << " = " << val << std::endl;
		obj[key] = val; 
		value_in_table = val;
	}

	bool operator<(const LuaClVariable& right) const{
//		assert(this->obj != luabind::nil && right.obj != luabind::nil);
		if(this->obj_ref == right.obj_ref)//(objects_equal(this->obj, right.obj))			// do not want to overload == everywhere
			return this->key < right.key;
		else
			return this->obj_ref < right.obj_ref;//objects_less(this->obj, right.obj);
	}
private:
//	ClVariable var;
	static const int key4refs;
	//luabind::object obj;
	int obj_ref;
	std::string key;
	lua_State* L;
	double value_in_table;
};

class LuaClLinearExpression{

	friend class LuaCassowary;

	class VarsCompareCheckEqual{
	public:
		bool operator()(const LuaClVariable& left, const LuaClVariable& right){
			if(!(left < right) && !(right < left)){
				assert(left.obj_ref == right.obj_ref && left.key == right.key);
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
public:
	LuaCassowary(void);
	~LuaCassowary(void);
	static void luabind(lua_State* L);

	void solve();			// update lua vars from Cassowary vars!

	void addEquation(luabind::object info1,
					 luabind::object info2,
					 double dx);
	void minimize(const LuaClLinearExpression& expr);
	void maximize(const LuaClLinearExpression& expr);
	void addConstraint(const LuaClLinearExpression& left, string op_sign, const LuaClLinearExpression& right);
//	void addExternalStay(luabind::object obj, std::string key);
private:
	ClSimplexSolver solver;
	bool need_resolve;
	typedef std::pair<luabind::object, std::string> obj_key;
	
	void change_vars_to_cached(ClLinearExpression::ClVarToCoeffMap& terms);

	struct CompareVarsUnderPtr{
		bool operator()(const LuaClVariable* left, const LuaClVariable* right) const{
			return *left < *right;
		}
	};

	// if somebody wants to add existing var - take it from here
	// true means it is new and should be made stay
	// if == 0 - do not add it in solve()
	// if > 0 - mult strength by it 
	std::map<LuaClVariable*, bool, CompareVarsUnderPtr> cl_vars;
//	std::set<LuaClVariable*, CompareVarsUnderPtr> cl_stays;					// who stays with strength=2.0 (self width and height)
};
