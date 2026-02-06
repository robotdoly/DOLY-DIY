#pragma once
#include <stdint.h>
#include "ServoEventListener.h"

namespace ServoEvent
{
	// Add listener class
	void AddListener(ServoEventListener* observer, bool priority = false);

	// Remove listener class
	void RemoveListener(ServoEventListener* observer);

	// Add static event fuction to complete event queue
	void AddListenerOnComplete(void(*onEvent)(uint16_t id, ServoId channel));

	// Remove static event fuction from complete event queue
	void RemoveListenerOnComplete(void(*onEvent)(uint16_t id, ServoId channel));

	// Add static event fuction to error event queue
	void AddListenerOnAbort(void(*onEvent)(uint16_t id, ServoId channel));

	// Remove static event fuction from error event queue
	void RemoveListenerOnAbort(void(*onEvent)(uint16_t id, ServoId channel));

	// Add static event fuction to error event queue
	void AddListenerOnError(void(*onEvent)(uint16_t id, ServoId channel));

	// Remove static event fuction from error event queue
	void RemoveListenerOnError(void(*onEvent)(uint16_t id, ServoId channel));

	// Call complete event listeners
	void ServoComplete(uint16_t id, ServoId channel);

	// Call abort event listeners
	void ServoAbort(uint16_t id, ServoId channel);

	// Call error event listeners
	void ServoError(uint16_t id, ServoId channel);

};


