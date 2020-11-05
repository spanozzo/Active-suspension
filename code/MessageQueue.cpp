/*
 * MessageQueue.cpp
 *
 *  Created on: Oct 12, 2020
 *      Author: stefano
 */
#include "MessageQueue.hpp"

MessageQueue *MessageQueue::instance = 0;

MessageQueue *MessageQueue::getInstance() {
	if(instance == 0) {
		instance = new MessageQueue();
	}
	return instance;
}

MessageQueue::MessageQueue() {
	gyroData = 0;
	suspensionData = 0;
	newGyroData = false;
	newSuspensionData = false;
	changeMode = false;
}

float* MessageQueue::getGyroData() {
	newGyroData = false;
	return gyroData;
}

float* MessageQueue::getSuspensionData() {
	newSuspensionData = false;
	return suspensionData;
}

void MessageQueue::setGyroData(float *data) {
	gyroData = data;
	newGyroData = true;
}

void MessageQueue::setSuspensionData(float *data) {
	suspensionData = data;
	newSuspensionData = true;
}

bool MessageQueue::isNewGyroData() {
	return newGyroData;
}

bool MessageQueue::isNewSuspensionData() {
	return newSuspensionData;
}

void MessageQueue::setChangeMode() {
	changeMode = true;
}

bool MessageQueue::getChangeMode() {
	// only Model.cpp use this method, so it's safe to do this
	if(changeMode) {
		changeMode = false;
		return true;
	}
	return changeMode;
}
