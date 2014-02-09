#include "PlatformPrecomp.h"
#include "LuaCassowary.h"
#include "luabind/operator.hpp"

#include <map>
#include <iostream>
#include <strstream>

using namespace std;

const int LuaClVariable::key4refs = 0;		// value doesn't matter, we need unique address

std::string make_var_name(const luabind::object& obj, const string& key){
	string var_name;
	if(obj["id"])
		var_name += luabind::object_cast<string>(obj["id"])+".";
	var_name += key;
	return var_name;
}

LuaClVariable::LuaClVariable(const luabind::object& obj, const std::string& key)
:key(key){
	this->L = obj.interpreter();
	ClFloatVariable::SetName(make_var_name(obj, key));

	// try find its obj_ref in registry
	lua_pushlightuserdata(L, (void*)&key4refs);
	lua_gettable(L, LUA_REGISTRYINDEX); assert(lua_istable(L, -1));		// get obj->ref storage in stack (-2)
	obj.push(L); assert(!lua_isnil(L, -1));								// -1
	lua_gettable(L, -2);												// get ref for it
	
	if(!lua_isnil(L, -1)){
		this->obj_ref = (int)lua_tonumber(L, -1);
	}
	// not found - obtain new reference
	else{
		obj.push(L);											// now -1
		this->obj_ref = luaL_ref(L, LUA_REGISTRYINDEX);			// popped

		// add to refs_table
		lua_pushlightuserdata(L, (void*)&key4refs);
		lua_gettable(L, LUA_REGISTRYINDEX);					// -3
		obj.push(L);										// -2
		lua_pushnumber(L, this->obj_ref);					// -1
		lua_settable(L, -3);								// obj->ref table
	}// else

	value_in_table = PendingValue();
}

LuaCassowary::LuaCassowary(void){
	solver.SetAutosolve(false);
	solver.SetExplaining(true);
	need_resolve = false;
}

LuaCassowary::~LuaCassowary(void){
}

void LuaCassowary::solve(){

//	solver.PrintOnVerbose(cout);

	bool log = true;//((unsigned)this&0xffff) == 0x8dc0;
	ostrstream ostr;	
	ostream& cout = log ? ::cout : ostr;

	cout << "**********" << this << "**********" << endl;

	// if there were changes in rules - apply computed solution
	if(need_resolve){

		// 1 add stays to whom we haven't yet
		// by doing it here (in render handler we assure that we use the latest var values)

		for(std::map<LuaClVariable*, bool, CompareVarsUnderPtr>::iterator it = cl_vars.begin(); it != cl_vars.end(); ++it){
			
			if(it->second == false)			// already there
				continue;

			// add stay 1.0 - 1.5
			LuaClVariable* lv = it->first;

			double strength = lv->key=="width" || lv->key=="height" ? 1000.0 : 1.0;

			ClConstraint* pcn;
			// specially for self width and height
//			if(cl_stays.find(lv) == cl_stays.end()){
				//strength *= it->second;			// !!!
				pcn = new ClStayConstraint(ClVariable(lv), ClsWeak(), strength);
//			}
			//else{
			//	assert(false);
			//	strength = 1.0;
			//	strength *= it->second;
			//	ClVariable var(lv);
			//	pcn = new ClStayConstraint(var, ClsStrong(), strength);			// weaker then real external edit
			//	//solver.AddEditVar(
			//}

			solver.AddConstraint(pcn);
			cout << *lv << " = " << lv->Value() << " weak (" << strength << ")" << endl;

			it->second = false;

//			cout << solver << endl;

		}// for

//		cout << solver << endl;

		// 2 solve
		solver.Solve();
		need_resolve = false;

//		cout << solver << endl;
		return;
	}// if

	// else read new values and solve
	try{
		solver.GetExternalVariables();
		cout << solver << endl;
	}catch(ExCLError& ex){
		cout << ex.description() << endl;
	}catch(...){
		cout << "caught something different in " << __FILE__ << ":" << __LINE__ << std::endl;
	}
			
}
void LuaCassowary::addEquation(luabind::object info1,
							   luabind::object info2,
							   double dx)
{
	bool log = true;//((unsigned)this&0xffff) == 0x8dc0;
	ostrstream ostr;	
	ostream& cout = log ? ::cout : ostr;

//	cout << "\tbegin link" << endl;
//	cout << this << ":" << endl;

	// input1
	luabind::object obj1 = info1[1];
	std::string key11 = luabind::object_cast<std::string>(info1[2]);
	double coef11 = luabind::object_cast<double>(info1[3]);
	std::string key12 = luabind::object_cast<std::string>(info1[4]);
	double coef12 = luabind::object_cast<double>(info1[5]);

	// input2
	luabind::object obj2 = info2[1];
	std::string key21 = luabind::object_cast<std::string>(info2[2]);
	double coef21 = luabind::object_cast<double>(info2[3]);
	std::string key22 = luabind::object_cast<std::string>(info2[4]);
	double coef22 = luabind::object_cast<double>(info2[5]);

	LuaClLinearExpression left  = coef11*LuaClLinearExpression(obj1,key11) + coef12*LuaClLinearExpression(obj1, key12);
	LuaClLinearExpression right = coef21*LuaClLinearExpression(obj2,key21) + coef22*LuaClLinearExpression(obj2, key22) + dx;
	this->addConstraint(left, "==", right);

	need_resolve = true;
//	cout << "\tend link" << endl;
}

void LuaCassowary::change_vars_to_cached(ClLinearExpression::ClVarToCoeffMap& terms){
	for(ClLinearExpression::ClVarToCoeffMap::iterator it = terms.begin(); it != terms.end();){
		LuaClVariable* lv = dynamic_cast<LuaClVariable*>(it->first.get_pclv());
			assert(lv);
		// add if absent
		if(cl_vars.find(lv) == cl_vars.end() && it->second != 0.0){
			cl_vars[lv] = true;
			++it;
		}
		// take if present
		else if(it->second != 0.0){
			// if it was dependent - leave it so
			double coef = it->second;

			ClLinearExpression::ClVarToCoeffMap::iterator next = it;			// need to remember next before erasing
				++next;
			terms.erase(it);

			ClVariable var(cl_vars.find(lv)->first);							// new ClVariable with old LuaClVariable
			terms[var] = coef;

			it = next;
		}
		else
			++it;
	}// for
}

void LuaCassowary::maximize(const LuaClLinearExpression& expr){
	
	// get from cache vars
	ClLinearExpression::ClVarToCoeffMap& terms = const_cast<ClLinearExpression::ClVarToCoeffMap&>(expr.expr.Terms());
	change_vars_to_cached(terms);

	// maximize
	ClLinearEquation eq(expr.expr, ClLinearExpression(1000000.0), ClsWeak());
	solver.AddConstraint(eq);

	// print
	cout << expr.expr << " -> MAX (weak)" << endl;
	need_resolve = true;
}

void LuaCassowary::minimize(const LuaClLinearExpression& expr){
	
	// get from cache vars
	ClLinearExpression::ClVarToCoeffMap& terms = const_cast<ClLinearExpression::ClVarToCoeffMap&>(expr.expr.Terms());
	change_vars_to_cached(terms);

	// minimize
	ClLinearEquation eq(expr.expr, ClLinearExpression(-1000000.0), ClsWeak());
	solver.AddConstraint(eq);

	// print
	cout << expr.expr << " -> MIN (weak)" << endl;
	need_resolve = true;
}

void LuaCassowary::addConstraint(const LuaClLinearExpression& left, string op_sign, const LuaClLinearExpression& right){
	
	// get from cache vars
	ClLinearExpression::ClVarToCoeffMap& terms_r = const_cast<ClLinearExpression::ClVarToCoeffMap&>(right.expr.Terms());
	ClLinearExpression::ClVarToCoeffMap& terms_l = const_cast<ClLinearExpression::ClVarToCoeffMap&>(left.expr.Terms());
	change_vars_to_cached(terms_r);
	change_vars_to_cached(terms_l);

	if(op_sign=="=="){
		ClLinearEquation eq(left.expr, right.expr);
		solver.AddConstraint(eq);
	}// if equality
	else{
		ClCnRelation op;
		if(op_sign==">")
			op = cnGT;
		else if(op_sign=="<")
			op = cnLT;
		else if(op_sign==">=")
			op = cnGEQ;
		else if(op_sign=="<=")
			op = cnLEQ;
		else
			assert(false);

		ClLinearInequality ineq(left.expr, op, right.expr, ClsRequired(), 2.0);		// ineq are stronger
		solver.AddConstraint(ineq);
	}// if inequality

	
	cout << left.expr << op_sign << right.expr << " required ";
	if(op_sign=="==")
		cout << "1.0" << endl;
	else
		cout << "2.0" << endl;

	need_resolve = true;
}

//void LuaCassowary::addExternalStay(luabind::object obj, std::string key){
//	// create or get from cache
//	LuaClVariable* lv = new LuaClVariable(obj, key);
//	// add if absent
//	if(cl_vars.find(lv) == cl_vars.end()){
//		cl_vars[lv] = 1.0;
//	}
//	// take if present
//	else{
//		LuaClVariable* tmp = lv;
//		lv = cl_vars.find(lv)->first;
//		delete tmp;
//	}
//
//	// add stay
//	cl_stays.insert(lv);
//	cout << "Adde stay: " << *lv << std::endl;
//
//	//ClVariable var(lv);
//	//solver.AddStay(var, ClsWeak(), 2.0);
//	//cl_vars[lv] = true;
//	//cout << var << " = " << var.Value() << endl;
//
//	//need_resolve = true;
//}

//void LuaCassowary::editVar(luabind::object obj, std::string key){
//	double val = luabind::object_cast<double>(obj[key]);
//
//	// must be present
//	assert( cl_vars.find(make_pair(obj, key)) != cl_vars.end() );
//
//	ClVariable cl_var = cl_vars[make_pair(obj, key)];
//
//	solver.AddEditVar(cl_var);
//
//	// go edit
//	solver.BeginEdit();
//	solver.SuggestValue(cl_var, val);
//	solver.EndEdit();
//}
//
//void LuaCassowary::editPoint(luabind::object obj, std::string key1, std::string key2){
//	double val1 = luabind::object_cast<double>(obj[key1]);
//	double val2 = luabind::object_cast<double>(obj[key2]);
//
//	// must be present - but:
//	// solver excludes vars from table if thay have coeff=0
//	// just ignore it and simple change the var
//	if( cl_vars.find(make_pair(obj, key1)) == cl_vars.end() ||
//		cl_vars.find(make_pair(obj, key2)) == cl_vars.end() )
//	{
//		cout << "Ignoring editing of " << key1 << " and " << key2 << endl;
//		return;
//	}
//
//	ClVariable cl_var1 = cl_vars[make_pair(obj, key1)];
//	ClVariable cl_var2 = cl_vars[make_pair(obj, key2)];
//
//	// TODO: This should be prevented earlier in map
//	try{
//		solver.AddEditVar(cl_var1);
//		solver.AddEditVar(cl_var2);
//	}catch(ExCLError& ex){
//		cout << ex.description() << endl;
//		return;
//	}
//
//	// go edit
//	solver.BeginEdit();
//	solver.SuggestValue(cl_var1, val1);
//	solver.SuggestValue(cl_var2, val2);
//	solver.Resolve();
//	solver.EndEdit();
//}

void LuaCassowary::luabind(lua_State* L){

	// register table for storing references inside LuaClVariable
	//luabind::registry(L)[(void*)&LuaClVariable::key4refs] = luabind::newtable(L);
	lua_pushlightuserdata(L, (void*)&LuaClVariable::key4refs);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	luabind::module(L) [
		luabind::class_<LuaCassowary>("Cassowary")
		.def(luabind::constructor<>())
		.def("solve", &LuaCassowary::solve)
		//.def("addStay", (void (LuaCassowary::*)(luabind::object, std::string key, double x))&LuaCassowary::addStay)
		//.def("addStay", (void (LuaCassowary::*)(luabind::object, std::string key1, double coef1, std::string key2, double coef2, double x))&LuaCassowary::addStay)
		//.def("addPointStay", &LuaCassowary::addPointStay)
		.def("maximize", &LuaCassowary::maximize)
		.def("minimize", &LuaCassowary::minimize)
		.def("addEquation", &LuaCassowary::addEquation)
		.def("addConstraint", &LuaCassowary::addConstraint)
//		.def("addExternalStay", &LuaCassowary::addExternalStay)
//		.def("editVar", &LuaCassowary::editVar)
//		.def("editPoint", &LuaCassowary::editPoint)
		
		,

		luabind::class_<LuaClLinearExpression>("Expr")
		.def(luabind::constructor<double>())
		.def(luabind::constructor<const luabind::object&, const std::string&>())
		.def(luabind::self + luabind::other<LuaClLinearExpression&>())
		.def(luabind::self - luabind::other<LuaClLinearExpression&>())
		.def(luabind::self * luabind::other<LuaClLinearExpression&>())
		.def(luabind::self / luabind::other<LuaClLinearExpression&>())
	];
}