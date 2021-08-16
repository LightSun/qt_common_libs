#ifndef COMMONS_H
#define COMMONS_H

#include <functional>
#include <future>

#define FUNC_PLACEHOLDERS_1 std::placeholders::_1
#define FUNC_PLACEHOLDERS_2 FUNC_PLACEHOLDERS_1,std::placeholders::_2
#define FUNC_PLACEHOLDERS_3 FUNC_PLACEHOLDERS_2,std::placeholders::_3
#define FUNC_PLACEHOLDERS_4 FUNC_PLACEHOLDERS_3,std::placeholders::_4
#define FUNC_PLACEHOLDERS_5 FUNC_PLACEHOLDERS_4,std::placeholders::_5
#define FUNC_PLACEHOLDERS_6 FUNC_PLACEHOLDERS_5,std::placeholders::_6
#define FUNC_PLACEHOLDERS_7 FUNC_PLACEHOLDERS_6,std::placeholders::_7
#define FUNC_PLACEHOLDERS_8 FUNC_PLACEHOLDERS_7,std::placeholders::_8
#define FUNC_PLACEHOLDERS_9 FUNC_PLACEHOLDERS_8,std::placeholders::_9

#define FUNC_SHARED_PTR(func_expre) std::shared_ptr<std::packaged_task<func_expre>>
#define FUNC_MAKE_SHARED_PTR(func, func_expre, place_holders) \
std::make_shared<std::packaged_task<func_expre>>(std::bind(func, place_holders))

#define FUNC_MAKE_SHARED_PTR_1(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_1)
#define FUNC_MAKE_SHARED_PTR_2(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_2)
#define FUNC_MAKE_SHARED_PTR_3(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_3)
#define FUNC_MAKE_SHARED_PTR_4(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_4)
#define FUNC_MAKE_SHARED_PTR_5(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_5)
#define FUNC_MAKE_SHARED_PTR_6(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_6)
#define FUNC_MAKE_SHARED_PTR_7(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_7)
#define FUNC_MAKE_SHARED_PTR_8(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_8)
#define FUNC_MAKE_SHARED_PTR_9(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_9)


#endif // COMMONS_H
