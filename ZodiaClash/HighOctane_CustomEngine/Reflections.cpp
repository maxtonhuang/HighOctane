#include "Reflections.h"
#include <iostream>
#include <rttr/registration>
#include <rttr/type>

//class MyClass {
//public:
//    MyClass() {}
//    void printMessage() { std::cout << "Hello from MyClass!" << std::endl; }
//};
//
//RTTR_REGISTRATION
//{
//    rttr::registration::class_<MyClass>("MyClass")
//        .method("printMessage", &MyClass::printMessage);
//}
//
//void testFunc() {
//    rttr::type t = rttr::type::get<MyClass>();
//    std::cout << "Type name: " << t.get_name().to_string() << std::endl;
//
//    MyClass myObj;
//    rttr::method meth = t.get_method("printMessage");
//    meth.invoke(myObj);
//}
