//
// Created by Simon on 2020/5/10.
//

#include "config.hpp"

Helper* Helper::helper = nullptr;
int main()
{
    REGISTER(Sphere)
    REGISTER(Cube)
    REGISTER(Cone)
    ObjectInfo obj;
    obj.Init();
    system("pause");
}