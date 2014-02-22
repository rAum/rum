#ifndef __OBJECT_SCALE_HPP__
#define __OBJECT_SCALE_HPP__
#include "IObject.hpp"

/// Enable object scalling
class Scale : public IObject
{
public:
    explicit Scale(const IObject* object, double scale = 2.) : mObj(object), mScale(scale) { }
private:
    virtual double dist(const Point4D & point) const {
        return mObj->Distance(point / mScale) * mScale;
    }
    const IObject* mObj;
    double mScale;
};


#endif //__OBJECT_SCALE_HPP__
