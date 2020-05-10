//
// Created by Simon on 2020/5/10.
//

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
using namespace std;
class Object;

#define REGISTER(class_name) \
	Helper::Inst()->Push(#class_name,[]()->Object* \
	{ return new class_name;});\

class Helper
{
private:
    map<string, function<Object*()>> mapStr2Func;
    static Helper* helper;
    Helper() {}
public:
    static Helper* Inst()
    {
        if (!helper)
        {
            helper = new Helper();
        }
        return helper;
    }
    Object* CreateObject(string name)
    {
        if (mapStr2Func.find(name) != mapStr2Func.end())
            return mapStr2Func[name]();
        return nullptr;
    }
    void Push(string name, function<Object*()> func) { mapStr2Func[name] = func; }
};

class Object
{
public:
    virtual void Print() = 0;
    const char* GetClassName()
    {
        return typeid(*this).name();
    }
};

class Sphere : public Object
{
    void Print() override { cout << GetClassName() << endl; }
};


class Cube : public Object
{
    void Print() override { cout << GetClassName() << endl; }
};

class Cone : public Object
{
    void Print() override { cout << GetClassName() << endl; }
};

class ObjectInfo
{
private:
    vector<Object*> vecObjs;
public:
    void Init()
    {
        vector<string> name =
                {
                        "Sphere",
                        "Cube",
                        "Cube",
                        "Cone",
                };
        for (int i = 0; i < name.size(); i++)
        {
            Object* obj = Helper::Inst()->CreateObject(name[i]);
            if (obj)
            {
                vecObjs.push_back(obj);
                obj->Print();
            }
        }
    }
};