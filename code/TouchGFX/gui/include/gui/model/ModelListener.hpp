#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void notifyXYZChanged(float gyroData[3]) {}
    virtual void notifySuspensionChanged(float suspensionData[4]) {}
    virtual void notifyChangeMode() {}
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
