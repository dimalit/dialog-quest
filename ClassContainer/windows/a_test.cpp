#include "PlatformPrecomp.h"
#include <luabind/config.hpp>
#include <luabind/typeid.hpp>
#include <luabind/detail/inheritance.hpp>

typedef std::size_t class_id;
extern LUABIND_API class_id luabind::detail::allocate_class_id(type_id const& cls);
class_id static_var = luabind::detail::allocate_class_id(typeid(int));

/*
int main(){
	luabind::detail::allocate_class_id(typeid(int));
	return 0;
}
*/