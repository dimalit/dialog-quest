#include "PlatformPrecomp.h"
#include "LuaCassowary.h"

#include <map>
#include <iostream>
#include <strstream>

using namespace std;

LuaCassowary::LuaCassowary(void){
	solver.SetAutosolve(false);
	need_resolve = false;
}

LuaCassowary::~LuaCassowary(void){
}

void LuaCassowary::solve(){
	bool log = true;//((unsigned)this&0xffff) == 0x8dc0;
	ostrstream ostr;	
	ostream& cout = log ? ::cout : ostr;

	cout << "**********" << this << "**********" << endl;

	// 0 if there were changes in rules - apply computed solution
	if(need_resolve){
		// 1 Read final values of Stay vars and honor them
		//for(std::map<obj_key, ClConstraint*>::iterator it = cl_stays.begin(); it != cl_stays.end(); ++it){
		//	const luabind::object&	obj = it->first.first;
		//	const string&			key = it->first.second;
		//	ClVariable				var = cl_vars[it->first];
		//	double					val = luabind::object_cast<double>(obj[key]);

		//	cout << "Stay " << var << " = " << val << endl;
		//	var.SetValue(val);

		//	ClConstraint* pcn = new ClStayConstraint(var, ClsWeak());
		//	solver.AddConstraint(pcn);
		//		assert(it->second==NULL);
		//	it->second = pcn;
		//}// for

		// 2 solve
		solver.Solve();

		// 3 apply
		for(std::map<obj_key, ClVariable>::iterator it = cl_vars.begin(); it != cl_vars.end(); ++it){
			const luabind::object& obj = it->first.first;
			const string& key = it->first.second;
			obj[key] = it->second->Value();
			cout << "Pre-Solving " << it->second  << endl;
		}// for

		// 4 exit
		need_resolve = false;
		return;
	}// if

	// store iterators to changed vars here
	std::vector<std::map<obj_key, ClVariable>::iterator> changed;

	//cout << "Before:\n"; solver.PrintOn(cout);

	// 1 Read vars that changed
	for(std::map<obj_key, ClVariable>::iterator it = cl_vars.begin(); it != cl_vars.end(); ++it){
		const luabind::object& obj = it->first.first;
		const string& key = it->first.second;
		double lua_val = luabind::object_cast<double>(obj[key]);
		// if changed
		if( !ClApprox( it->second->Value(),  lua_val) ){

			try{
//				solver.RemoveConstraint(cl_stays[make_pair(obj, key)];
				solver.AddEditVar(it->second, ClsWeak(), 2.0);			// edits are heavier then stays
				changed.push_back(it);
				cout << "Edited: " << it->second << " = " << lua_val << endl;
			}catch(ExCLError& ex){
				cout << ex.description() << "\tVar: " << it->second->Name() << endl;
			}// catch
			catch(...){
				cout << "Another ex type?!" << endl;
			}
		}
	}// for

	if(changed.size() == 0){
		cout << endl;
		return;
	}

	// 2 go edit
	solver.BeginEdit();
	for(int i=0; i<changed.size(); i++){
		std::map<obj_key, ClVariable>::iterator it = changed[i];
		const luabind::object& obj = it->first.first;
		const string& key = it->first.second;

		solver.SuggestValue(it->second, luabind::object_cast<double>(obj[key]));
	}

	solver.Resolve();
	solver.EndEdit();

	//cout << "After:\n"; solver.PrintOn(cout);

	// TODO Should create special storage for independent vars!!
	// 3 update lua vars
	for(std::map<obj_key, ClVariable>::iterator it = cl_vars.begin(); it != cl_vars.end(); ++it){
		const luabind::object& obj = it->first.first;
		const string& key = it->first.second;
		obj[key] = it->second->Value();
		cout << "Solving " << it->second  << endl;
	}// for

	cout << endl;
}

std::string make_var_name(const luabind::object& obj, const string& key){
	string var_name;
	if(obj["id"])
		var_name += luabind::object_cast<string>(obj["id"])+".";
	var_name += key;
	return var_name;
}

void LuaCassowary::addStay(luabind::object obj, std::string key, double x){
	assert(false);
	double val = luabind::object_cast<double>(obj[key]);

	ClVariable cl_var(make_var_name(obj, key), val);

	// should be absent
	assert( cl_vars.find(make_pair(obj, key)) == cl_vars.end() );
		cl_vars[make_pair(obj, key)] = cl_var;

	ClLinearEquation eq(
		ClLinearExpression(cl_var),
		ClLinearExpression(x),
		ClsWeak()
	);

	solver.AddConstraint(eq);
	need_resolve = true;
}

void LuaCassowary::addStay(luabind::object obj, std::string key1, double coef1,
												std::string key2, double coef2,
							double x)
{
	assert(false);
	double val1 = luabind::object_cast<double>(obj[key1]);
	double val2 = luabind::object_cast<double>(obj[key2]);

	ClVariable cl_var1(make_var_name(obj, key1), val1);
	ClVariable cl_var2(make_var_name(obj, key2), val1);

	// presense not necessary
	// get from map if present
	// add if coef!=0.0
	if( cl_vars.find(make_pair(obj, key1)) == cl_vars.end() && coef1 != 0.0)	// 0.0 means ignore it
		cl_vars[make_pair(obj, key1)] = cl_var1;
	else if(coef1 != 0.0)
		cl_var1 = cl_vars[make_pair(obj, key1)];
	if( cl_vars.find(make_pair(obj, key2)) == cl_vars.end() && coef2 != 0.0)
		cl_vars[make_pair(obj, key2)] = cl_var2;
	else if(coef2 != 0.0)
		cl_var2 = cl_vars[make_pair(obj, key2)];

	ClLinearEquation eq(
		ClLinearExpression(cl_var1).Times(coef1).Plus( ClLinearExpression(cl_var2).Times(coef2) ),
		ClLinearExpression(x),
		ClsWeak()
	);

	solver.AddConstraint(eq);

//	std::cout << "Added " << key1 << "*" << coef1 << " + " << key2 << "*" << coef2 << " = " << x << std::endl;
	need_resolve = true;
}

void LuaCassowary::addPointStay(luabind::object obj, std::string key1, std::string key2){
	assert(false);

	double val1 = luabind::object_cast<double>(obj[key1]);
	double val2 = luabind::object_cast<double>(obj[key2]);

	ClVariable cl_var1(make_var_name(obj, key1), val1);
	ClVariable cl_var2(make_var_name(obj, key2), val2);
	
	// not present yet
	assert( cl_vars.find(make_pair(obj, key1)) == cl_vars.end() );
		cl_vars[make_pair(obj, key1)] = cl_var1;
	assert( cl_vars.find(make_pair(obj, key2)) == cl_vars.end() );
		cl_vars[make_pair(obj, key2)] = cl_var2;

	solver.AddPointStay(cl_var1, cl_var2);
	need_resolve = true;
}
void LuaCassowary::addEquation(luabind::object info1,
							   luabind::object info2,
							   double dx)
{
	bool log = true;//((unsigned)this&0xffff) == 0x8dc0;
	ostrstream ostr;	
	ostream& cout = log ? ::cout : ostr;

//	cout << "\tbegin link" << endl;
	cout << this << ":" << endl;

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

	double val11 = luabind::object_cast<double>(obj1[key11]);
	double val12 = luabind::object_cast<double>(obj1[key12]);
	double val21 = luabind::object_cast<double>(obj2[key21]);
	double val22 = luabind::object_cast<double>(obj2[key22]);

	ClVariable cl_var11(make_var_name(obj1, key11), val11);
	ClVariable cl_var12(make_var_name(obj1, key12), val12);
	ClVariable cl_var21(make_var_name(obj2, key21), val21);
	ClVariable cl_var22(make_var_name(obj2, key22), val22);

	// find patient's vars if present
	if( cl_vars.find(make_pair(obj1, key11)) == cl_vars.end() && coef11 != 0.0){
		cl_vars[make_pair(obj1, key11)] = cl_var11;
//		cl_stays[make_pair(obj1, key11)] = NULL;			// will be added in solve()
	}
	else if(coef11 != 0.0)
		cl_var11 = cl_vars[make_pair(obj1, key11)];
	if( cl_vars.find(make_pair(obj1, key12)) == cl_vars.end() && coef12 != 0.0){
		cl_vars[make_pair(obj1, key12)] = cl_var12;
//		cl_stays[make_pair(obj1, key12)] = NULL;			// will be added in solve()
	}
	else if(coef12 != 0.0)
		cl_var12 = cl_vars[make_pair(obj1, key12)];

	// nurse should be present. if not - add it as "stay"
		// 1
	if( cl_vars.find(make_pair(obj2, key21)) == cl_vars.end() && coef21 != 0.0){
		cl_vars[make_pair(obj2, key21)] = cl_var21;
		
		ClConstraint* pcn = new ClStayConstraint(cl_var21, ClsWeak());
		solver.AddConstraint(pcn);
		//cl_stays[make_pair(obj2, key21)] = pcn;			// will be added in solve()

		cout << cl_var21 << " = " << cl_var21.Value() << endl;
	}
	else if(coef21 != 0.0){
		cl_var21 = cl_vars[make_pair(obj2, key21)];
	}
		// 2
	if( cl_vars.find(make_pair(obj2, key22)) == cl_vars.end() && coef22 != 0.0){
		cl_vars[make_pair(obj2, key22)] = cl_var22;

		ClConstraint* pcn = new ClStayConstraint(cl_var22, ClsWeak());
		solver.AddConstraint(pcn);
		//cl_stays[make_pair(obj2, key22)] = pcn;			// will be added in solve()

		cout << cl_var22 << " = " << cl_var22.Value() << endl;
	}
	else if(coef22 != 0.0){
		cl_var22 = cl_vars[make_pair(obj2, key22)];
	}

	ClLinearExpression left;
	if(coef11 != 0.0 && coef12 != 0)
		left = ClLinearExpression(cl_var11).Times(coef11).Plus( ClLinearExpression(cl_var12).Times(coef12) );
	else if(coef11 != 0.0)
		left = ClLinearExpression(cl_var11).Times(coef11);
	else if(coef12 != 0.0)
		left = ClLinearExpression(cl_var12).Times(coef12);
	else
		left = ClLinearExpression(0.0);

	ClLinearExpression right;
	if(coef21 != 0.0 && coef22 != 0)
		right = ClLinearExpression(cl_var21).Times(coef21).Plus( ClLinearExpression(cl_var22).Times(coef22) ).Plus(dx);
	else if(coef21 != 0.0)
		right = ClLinearExpression(cl_var21).Times(coef21).Plus(dx);
	else if(coef22 != 0.0)
		right = ClLinearExpression(cl_var22).Times(coef22).Plus(dx);
	else
		right = ClLinearExpression(dx);

	cout << left << " = " << right << endl;

	ClLinearEquation eq(left, right);

	solver.AddConstraint(eq);
	need_resolve = true;
//	cout << "\tend link" << endl;
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
	luabind::module(L) [
		luabind::class_<LuaCassowary>("Cassowary")
		.def(luabind::constructor<>())
		.def("solve", &LuaCassowary::solve)
		//.def("addStay", (void (LuaCassowary::*)(luabind::object, std::string key, double x))&LuaCassowary::addStay)
		//.def("addStay", (void (LuaCassowary::*)(luabind::object, std::string key1, double coef1, std::string key2, double coef2, double x))&LuaCassowary::addStay)
		//.def("addPointStay", &LuaCassowary::addPointStay)
		.def("addEquation", &LuaCassowary::addEquation)
//		.def("editVar", &LuaCassowary::editVar)
//		.def("editPoint", &LuaCassowary::editPoint)
	];
}