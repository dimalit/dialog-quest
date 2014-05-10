#include "PlatformPrecomp.h"
#include "LuaCassowary.h"
#include "luabind/operator.hpp"

#include <map>
#include <iostream>
#include <strstream>

using namespace std;

const int LuaClVariable::key4refs = 0;		// value doesn't matter, we need unique address

void print_call(ClSimplexSolver& solver, const string& func){
//	if((((int)&solver)&0xffff) == 0x8560)
//	cout << "CALL " << &solver << " " << func << std::endl;
}

void print_call(ClSimplexSolver& solver, string func, LuaClVariable* lv, const string& info){
//	if((((int)&solver)&0xffff) == 0x8560)
//	cout << "CALL " << &solver << " " << func << " " << *lv << " " << info << std::endl;
}

void print_call(ClSimplexSolver& solver, string func, const ClLinearConstraint& e){
//	if((((int)&solver)&0xffff) == 0x8560)
//	cout << "CALL " << &solver << " " << func << " " << e << std::endl;
}

std::string make_var_name(const luabind::object& obj, const string& key){
	string var_name;
	if(obj["id"])
		var_name += luabind::object_cast<string>(obj["id"])+".";
//	else
//		std::cout << "NOOOO ID" << std::endl;
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

		lua_pop(L, 1);
	}// else

	lua_pop(L, 2);

	value_in_table = PendingValue();
}

LuaCassowary::LuaCassowary(void){
	solver.SetAutosolve(true);
//	solver.SetAutoResetStayConstants(false);
	solver.SetExplaining(true);
	need_resolve = false;
	edit_mode = false;
	first_time = true;
}

LuaCassowary::~LuaCassowary(void){
}

void LuaCassowary::solve(){
	assert(!edit_mode);

//	solver.PrintOnVerbose(cout);

	bool log = true;//((unsigned)this&0xffff) == 0x8dc0;
	ostrstream ostr;	
	ostream& cout = log ? ::cout : ostr;

	cout << "**********" << this << "**********" << endl;

	// if there were changes in rules - apply computed solution
	if(need_resolve){

		// 1 add stays to whom we haven't yet
		// by doing it here (in render handler we assure that we use the latest var values)

		//for(std::map<LuaClVariable*, bool, CompareVarsUnderPtr>::iterator it = cl_vars.begin(); it != cl_vars.end(); ++it){
		//	
		//	if(it->second == false)			// already there
		//		continue;

		//	//update value AND add as stay
		//	it->first->value_in_table = it->first->PendingValue();
		//	ClVariable var(it->first);
		//	solver.AddStay(var, ClsWeak(), 1.0);
		//	cout << this << ":\t " << var << " = " << var.Value() << " explicit weak " << std::endl;

		//	it->second = false;
		//}// for

//		cout << solver << endl;

		// 2 solve
		solver.Solve();
		need_resolve = false;

//		cout << solver << endl;
		return;
	}// if

	// else read new values and solve
	//try{
	//	solver.GetExternalVariables();
	//	//cout << solver << endl;
	//}catch(ExCLError& ex){
	//	cout << ex.description() << endl;
	//}catch(...){
	//	cout << "caught something different in " << __FILE__ << ":" << __LINE__ << std::endl;
	//}
			
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

void LuaCassowary::change_vars_to_cached_and_remove_zeros(ClLinearExpression::ClVarToCoeffMap& terms){
	for(ClLinearExpression::ClVarToCoeffMap::iterator it = terms.begin(); it != terms.end();){
		LuaClVariable* lv = dynamic_cast<LuaClVariable*>(it->first.get_pclv());
			assert(lv);

		// remove if zero
		if(it->second == 0.0){
			terms.erase(it++);
			continue;
		}

		// add if absent
		if(cl_vars.find(lv) == cl_vars.end()){
			cl_vars.insert(lv);
			print_call(solver, "AddStay", lv, "weak 1.0");
			solver.AddStay(lv, ClsWeak(), 1.0);
			++it;
		}
		// take if present
		else{
			// if it was dependent - leave it so
			double coef = it->second;

			ClLinearExpression::ClVarToCoeffMap::iterator next = it;			// need to remember next before erasing
				++next;
			terms.erase(it);

			ClVariable var(*cl_vars.find(lv));							// new ClVariable with old LuaClVariable
			terms[var] = coef;

			it = next;
		} // else
	}// for
}

void LuaCassowary::maximize(const LuaClLinearExpression& expr){
	assert(!edit_mode);

	// get from cache vars
	ClLinearExpression::ClVarToCoeffMap& terms = const_cast<ClLinearExpression::ClVarToCoeffMap&>(expr.expr.Terms());
	change_vars_to_cached_and_remove_zeros(terms);

	// maximize
//	ClLinearEquation eq(expr.expr, ClLinearExpression(100000.0), ClsMedium(), 1.0);
	ClLinearInequality ineq(expr.expr, cnGEQ, ClLinearExpression(100000.0), ClsWeak(), 1.0);
	print_call(solver, "AddConstraint", ineq);
	solver.AddConstraint(ineq);

	// print
	cout << expr.expr << " -> MAX (weak 1.0)" << endl;
	need_resolve = true;
}

void LuaCassowary::minimize(const LuaClLinearExpression& expr){
	assert(!edit_mode);

	// get from cache vars
	ClLinearExpression::ClVarToCoeffMap& terms = const_cast<ClLinearExpression::ClVarToCoeffMap&>(expr.expr.Terms());
	change_vars_to_cached_and_remove_zeros(terms);

	// minimize
//	ClLinearEquation eq(expr.expr, ClLinearExpression(7.0), ClsMedium(), 1.0);
	ClLinearInequality ineq(expr.expr, cnLEQ, ClLinearExpression(7.0), ClsWeak(), 1.0);
	print_call(solver, "AddConstraint", ineq);	
	solver.AddConstraint(ineq);

	// print
	cout << expr.expr << " -> MIN (weak 1.0)" << endl;
	need_resolve = true;
}

void LuaCassowary::addConstraint(const LuaClLinearExpression& left, string op_sign, const LuaClLinearExpression& right){
	assert(!edit_mode);

	// get from cache vars
	ClLinearExpression::ClVarToCoeffMap& terms_r = const_cast<ClLinearExpression::ClVarToCoeffMap&>(right.expr.Terms());
	ClLinearExpression::ClVarToCoeffMap& terms_l = const_cast<ClLinearExpression::ClVarToCoeffMap&>(left.expr.Terms());
	change_vars_to_cached_and_remove_zeros(terms_r);
	change_vars_to_cached_and_remove_zeros(terms_l);

	if(op_sign=="=="){
		ClLinearEquation eq(left.expr, right.expr);
		print_call(solver, "AddConstraint", eq);
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
		print_call(solver, "AddConstraint", ineq);
		solver.AddConstraint(ineq);
	}// if inequality

	
	cout << this << ":\t " << left.expr << op_sign << right.expr << " required ";
	if(op_sign=="==")
		cout << "1.0" << endl;
	else
		cout << "2.0" << endl;

	need_resolve = true;
}

void LuaCassowary::addExternalStay(luabind::object obj, std::string key){
	assert(!edit_mode);

	// create
	LuaClVariable* lv = new LuaClVariable(obj, key);

	// or take from cache
	if(cl_vars.find(lv) != cl_vars.end()){
		LuaClVariable* cached = *cl_vars.find(lv);				// new ClVariable with old LuaClVariable
		delete lv;
		lv = cached;
	} // else

	// add stay
	cl_vars.insert(lv);// = false;												// add to cache

	print_call(solver, "AddStay", lv, "weak 100.0");
	solver.AddStay(lv, ClsWeak(), 100.0);									

	cout << this << ":\t " << *lv << " = " << lv->Value() << " explicit weak 100.0" << std::endl;
																		// solve will add it as weak when value is ready
	if((float)lv->Value() != (float)lv->PendingValue()){				// without cast to float comparison fails!!
		cout << lv->Value()  << " should be " << lv->PendingValue() << endl;
		assert(lv->Value() == lv->PendingValue());
	}

	need_resolve = true;		// TODO: need it?
}

void LuaCassowary::beginEdit(){
	edit_mode = true;
	assert(edit_list.empty());

	// turned off because autosolve is now on!
	//if(first_time){
	//	for(std::set<LuaClVariable*, CompareVarsUnderPtr>::iterator i = cl_vars.begin(); i != cl_vars.end(); ++i){
	//		LuaClVariable* lv = *i;
	//		if(lv->PendingValue() != lv->Value()){
	//			cout << *lv << " asserted to be " << lv->PendingValue() << endl;
	//			assert(false);
	//		}
	//	}
	//	first_time = false;
	//}//if

	// just do nothing
}

void LuaCassowary::addEditVariable(luabind::object obj, std::string key){
	assert(edit_mode);

	// create
	LuaClVariable* lv = new LuaClVariable(obj, key);

	// or take from cache
	if(cl_vars.find(lv) != cl_vars.end()){
		LuaClVariable* cached = *cl_vars.find(lv);				// new ClVariable with old LuaClVariable
		delete lv;
		lv = cached;
	}
	else{
		cout << "...found suggesting unexisting " << *lv << " (adding stay weak 1.0)" << std::endl;
		cl_vars.insert(lv);// = false;													// add to cache
		print_call(solver, "AddStay", lv, "weak 1.0");
		solver.AddStay(lv, ClsWeak(), 1.0);
		assert(lv->Value() == lv->PendingValue());
		return;
	}

	// simply add as stay
//	cout << *lv << " (adding stay weak 1.0)" << std::endl;
//	solver.AddStay(lv, ClsWeak(), 1.0);

	assert(edit_list.find(lv)==edit_list.end());
	edit_list[lv].val = lv->PendingValue();
	edit_list[lv].strength = 100.0;
}

void LuaCassowary::suggestAllValues(){
	assert(edit_mode);

	// 1 add to edit
	for(std::map<LuaClVariable*, val_strength>::iterator i = edit_list.begin(); i != edit_list.end(); ++i){
		LuaClVariable* lv = i->first;
		stringstream ss; ss << "strong " << i->second.strength;
		print_call(solver, "AddEditVar", lv, ss.str());
		solver.AddEditVar(lv, ClsStrong(), i->second.strength);
		std::cout << "Added Edit: " << *lv << " = " << i->second.val << " strong " << i->second.strength << std::endl;
	}

	// 2 suggest
	if(!edit_list.empty()){
		print_call(solver, "BeginEdit");
		solver.BeginEdit();

		for(std::map<LuaClVariable*, val_strength>::iterator i = edit_list.begin(); i != edit_list.end(); ++i){
			stringstream ss; ss << i->second.val;
			if(i->first->Name()=="cell_1_2.height" && i->second.val==46){
				int x = 3;
				x++;
			}
			print_call(solver, "SuggestValue", i->first, ss.str());
			solver.SuggestValue(i->first, i->second.val);
		}// for

		print_call(solver, "EndEdit");
		solver.EndEdit();			// solve here
	}
}

void LuaCassowary::endEdit(){
	assert(edit_mode);

	if(!edit_list.empty()){
		// add to stays
		//for(std::map<LuaClVariable*, val_strength>::iterator i = edit_list.begin(); i != edit_list.end(); ++i){
		//	// btw: not always equals ecause of required constraints
		//	// assert(i->first->Value() == i->first->PendingValue() && i->first->Value() == i->second);
		//	// TODO: Do we really need it? (used in custom onRequestLayOut)
		//	solver.AddStay(i->first, ClsWeak(), 1.0);
		//}// for

		edit_list.clear();
	}

	edit_mode = false;

//	if(goprint)
//		solver.PrintOnVerbose(std::cout);
}

// return number of changed vars
int LuaCassowary::addEditExternalVariables(){
	assert(edit_mode);
	// TODO Added this specially for custom onRequestLayOut in ButtonsElement. Check if we really need it!
	// read unchanged values and solve again

	const std::map<ClVariable, double>& ext_list = solver.GetExternallyEdited();
	int res = 0;

	for(std::map<ClVariable, double>::const_iterator i = ext_list.begin(); i != ext_list.end(); ++i){
		LuaClVariable* lv = dynamic_cast<LuaClVariable*>(i->first.get_pclv());
		assert(lv);
		assert(cl_vars.find(lv)!=cl_vars.end());
		
		if(!ClApprox(lv->Value(), lv->PendingValue())){
			cout << "found changed " << *lv << " -> " << lv->PendingValue() << endl;

			if(edit_list.find(lv)!=edit_list.end()){
				edit_list[lv].val = lv->PendingValue();		// update value
				// TODO: HACK: maybe assert here? it's undesired to change after being edited!
			}
			res++;
		}

		if(edit_list.find(lv)!=edit_list.end())			// skip already added
			continue;

		edit_list[lv].val = lv->PendingValue();
		edit_list[lv].strength = 1.0;

//		cout << *lv << " (adding stay weak 1.0)" << std::endl;
//		solver.AddStay(lv, ClsWeak(), 1.0);
	}// for

	return res;
}

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
		.def("addExternalStay", &LuaCassowary::addExternalStay)
		.def("beginEdit", &LuaCassowary::beginEdit)
		.def("addEditVariable", &LuaCassowary::addEditVariable)
		.def("suggestAllValues", &LuaCassowary::suggestAllValues)
		.def("endEdit", &LuaCassowary::endEdit)
		.def("addEditExternalVariables", (int (LuaCassowary::*)())&LuaCassowary::addEditExternalVariables)
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