#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model() : modelListener(0){
    msg = MessageQueue::getInstance();
}

void Model::tick()
{
    if(msg->isNewGyroData()) {
    	float *gyroData = msg->getGyroData();
        modelListener -> notifyXYZChanged(gyroData);
    }
    if(msg->isNewSuspensionData()) {
    	float *suspensionData = msg->getSuspensionData();
        modelListener -> notifySuspensionChanged(suspensionData);
    }
    if(msg->getChangeMode()) {
        modelListener -> notifyChangeMode();
    }
}