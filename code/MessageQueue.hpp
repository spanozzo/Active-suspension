/*
 * MessageQueue.hpp
 *
 *  Created on: Oct 12, 2020
 *      Author: stefano
 */

#ifndef MESSAGEQUEUE_HPP_
#define MESSAGEQUEUE_HPP_

class MessageQueue {
public:
	static MessageQueue *getInstance();
	float *getGyroData();
	float *getSuspensionData();
	void setGyroData(float *data);
	void setSuspensionData(float *data);
	bool isNewGyroData();
	bool isNewSuspensionData();
	void setChangeMode();
	bool getChangeMode();

private:
	MessageQueue();
	static MessageQueue *instance;
	float *gyroData;
	float *suspensionData;
	bool newGyroData;
	bool newSuspensionData;
	bool changeMode;
};

#endif /* MESSAGEQUEUE_HPP_ */
