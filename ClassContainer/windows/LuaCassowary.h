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
	virtual double Value() const {
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj_ref);
		luabind::object obj(luabind::from_stack(L, -1));
		assert(_finite(luabind::object_cast<double>(obj[key])));
		return luabind::object_cast<double>(obj[key]);
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
	}
	virtual void ChangeValue(double val){
		assert(_finite(val));
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj_ref);
		luabind::object obj(luabind::from_stack(L, -1));
		std::cout << "Changing " << *this << " = " << val << std::endl;
		obj[key] = val; 
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
private:
	struct fake_handle{
		lua_State* m_interpreter;
		int m_index;
	};
	static bool objects_less(const luabind::object& l, const luabind::object& r){
		fake_handle* l_h = reinterpret_cast<fake_handle*>(const_cast<luabind::object*>(&l));
		fake_handle* r_h = reinterpret_cast<fake_handle*>(const_cast<luabind::object*>(&r));
		assert(l_h->m_interpreter==l_h->m_interpreter);
		return lua_lessthan(l_h->m_interpreter, l_h->m_index, r_h->m_index);
	}
	static bool objects_equal(const luabind::object& l, const luabind::object& r){
		fake_handle* l_h = reinterpret_cast<fake_handle*>(const_cast<luabind::object*>(&l));
		fake_handle* r_h = reinterpret_cast<fake_handle*>(const_cast<luabind::object*>(&r));
		assert(l_h->m_interpreter==l_h->m_interpreter);
		return lua_equal(l_h->m_interpreter, l_h->m_index, r_h->m_index);
	}
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
	void addConstraint(const LuaClLinearExpression& left, string op_sign, const LuaClLinearExpression& right);
	void addExternalStay(luabind::object obj, std::string key);
private:
	ClSimplexSolver solver;
	bool need_resolve;
	typedef std::pair<luabind::object, std::string> obj_key;
	
	struct CompareVarsUnderPtr{
		bool operator()(const LuaClVariable* left, const LuaClVariable* right){
			return *left < *right;
		}
	};

	// if somebody wants to add existing var - take it from here
	// also remember stay strength coef for this var
	// if == 0 - do not add it in solve()
	// if > 0 - mult strength by it 
	std::map<LuaClVariable*, double, CompareVarsUnderPtr> cl_vars;
	std::set<LuaClVariable*, CompareVarsUnderPtr> cl_stays;					// who stays with strength=2.0 (self width and height)
};
