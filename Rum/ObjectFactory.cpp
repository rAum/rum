#include "ObjectFactory.hpp"
#include "Objects.hpp"
#include "ParserException.hpp"
#include <sstream>
#include <algorithm>


IObject* ObjectFactory::Construct(const std::string & str)
{
    std::stringstream stream(str);
    IObject* obj (NULL);

    obj = Construct(stream);

    return obj;
}

/// [+ - obiekt
/// [% - modyfikator
IObject* ObjectFactory::Construct(std::istream & str)
{
    char lb;
    char mod;
    std::string name;
    IObject* object (NULL);

    str >> lb;
    if (lb != '[') throw MakeParserException("Expected '['");

    str >> mod;

    try
    {
        switch (mod)
        {
        case '+':// add object
            str >> name;
            toLowercase(name);
            object = CreateObject(name, str);
            break;
        case '%':// add modifier
            str >> name;
            toLowercase(name);
            object = CreateModifier(name, str);
            break;
        default:
            throw MakeParserException("Expected '%' or '+'");
        };
    } catch (ParserException& ex) {
        throw ex;
    }

    return object;
}


IObject* ObjectFactory::CreateModifier(std::string & name, std::istream & str)
{
    IObject* obj(nullptr);
    IObject *a = nullptr, *b = nullptr;
    std::string tmp;

    try
    {
    if (name == "boolean") 
    {
        str >> tmp;
        toLowercase(tmp);

        if (tmp != "add" && tmp != "+" && tmp != "sub" && tmp != "-" && tmp != "subtract" && tmp != "union" && tmp != "&" && tmp != "blend" && tmp != "~" && tmp != "@")
            throw MakeParserException("Bad parameter for boolean operation.");

        a = Construct(str);
        b = Construct(str);

        if (tmp == "add" || tmp == "+")
            obj = new Boolean(a, b, Boolean::BOOL_ADD);
        else if (tmp == "sub" || tmp == "-" || tmp == "subtract")
            obj = new Boolean(a, b, Boolean::BOOL_SUBTRACT_B);
        else if (tmp == "union" || tmp == "&")
            obj = new Boolean(a, b, Boolean::BOOL_UNION);
        else if (tmp == "blend" || tmp == "~" || tmp == "@")
            obj = new Boolean(a, b, Boolean::BOOL_BLEND);
        else throw MakeParserException("Bad boolean option, expected 'add', 'sub', 'blend' or 'union'");
    }
    else if (name == "duplicate")
    {
        Vector3D mod;

        str >> mod;
        a = Construct(str);

        if (a == NULL) throw MakeParserException("Bad object in duplicate.");

        obj = new Duplicate(a, mod);
    }
    //else if (name == "pointmod")
    //{
    //    Matrix<> matrix;

    //    str >> matrix;
    //    a = Construct(str);

    //    obj = new PointMod(obj, [matrix](const Point4D& p) { return matrix * p; });
    //}
    else if (name == "matrix" || name == "transrot")
    {
        Matrix<> matrix;
        
        str >> matrix;
        a = Construct(str);

        obj = new TransRot(a, matrix);
    }
    else if (name == "scale")
    {
        double scale;

        str >> scale;
        a = Construct(str);

        obj = new Scale(a, scale);
    }
    else throw MakeParserException("Bad operation type. Expected one of 'boolean', 'matrix' or 'scale'");

    } catch (ParserException & ex) {
        SAFE_RELEASE(b);
        SAFE_RELEASE(a);
        SAFE_RELEASE(obj);
        throw ex;
    }

    char   rbracket;
    str >> rbracket; 
    if (rbracket != ']') 
    {
        SAFE_RELEASE(b);
        SAFE_RELEASE(a);
        SAFE_RELEASE(obj);
        throw MakeParserException("Expected ']'"); 
    }

    return obj;
}


IObject* ObjectFactory::CreateObject(std::string & name, std::istream & str)
{
    IObject* obj(NULL);

    try
    {
        if (name == "sphere")
        {
            Point4D pos;
            double rad;

            if (!(str >> pos >> rad)) throw MakeParserException("Invalid value for sphere");
            //str >> mat;
            obj = new Sphere(pos, rad);
        }
        else if (name == "cube")
        {
            Point4D pos;
            double size;

            if (!(str >> pos >> size)) throw MakeParserException("Invalid value for cube");

            obj = new Box(pos, size);
        }
        else if (name == "box")
        {
            Point4D pos;
            Point4D size;

            if (!(str >> pos >> size)) throw MakeParserException("Invalid value for box");

            obj = new Box(pos, size);
        }
        else if (name == "torus")
        {
            Point4D pos;
            double a, b;

            if (!(str >> pos >> a >> b)) throw MakeParserException("Invalid value for torus");
            
            obj = new Torus(pos, a, b);
        }
        else if (name == "cylinder")
        {
            Point4D pos;
            double rad, size;

            if (!(str >> pos >> rad >> size)) throw MakeParserException("Invalid value for cylinder");

            obj = new Cylinder(pos, rad, size);
        }
        else if (name == "plane")
        {
            Point4D pos, normal;

            if (!(str >> pos >> normal)) throw MakeParserException("Invalid value for plane");

            obj = new Plane(pos, normal);
        }
        else 
        {
            /// lookup in memory or throw exception
            throw MakeParserException(std::string("Unknow object type - ") + name);
        }
    } catch(ParserException & ex)
    {
        SAFE_RELEASE(obj);
        throw ex;
    }

    char   rbracket;
    str >> rbracket; 
    if (rbracket != ']') {
        SAFE_RELEASE(obj);
        throw MakeParserException("Expected ']'"); 
    }

    return obj;
}
