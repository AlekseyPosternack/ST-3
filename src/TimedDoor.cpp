// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& refDoor) : door(refDoor) {
}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeoutValue) :
          iTimeout(timeoutValue), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    if (adapter != nullptr) {
        delete adapter;
        adapter = nullptr;
    }
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}


void TimedDoor::throwState() {
    throw std::runtime_error("Error: Door is open after timeout!");
}

void Timer::sleep(int duration) {
    std::this_thread::sleep_for(std::chrono::seconds(duration));
}

void Timer::tregister(int timeout, TimerClient* clientPtr) {
    client = clientPtr;
    sleep(timeout);
    if (client != nullptr) {
        client->Timeout();
    }
}
