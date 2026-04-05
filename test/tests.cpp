// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <stdexcept>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(2);
    }

    void TearDown() override {
        delete door;
    }

    TimedDoor* door;
};

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
    void SetUp() override {
        timedDoor = new TimedDoor(2);
        adapter = new DoorTimerAdapter(*timedDoor);
    }

    void TearDown() override {
        delete adapter;
        delete timedDoor;
    }

    TimedDoor* timedDoor;
    DoorTimerAdapter* adapter;
};

class TimerTest : public ::testing::Test {
 protected:
    void SetUp() override {
        timer = new Timer();
        mockClient = new MockTimerClient();
    }

    void TearDown() override {
        delete mockClient;
        delete timer;
    }

    Timer* timer;
    MockTimerClient* mockClient;
};

TEST_F(TimedDoorTest, ConstructorInitializesCorrectly) {
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_EQ(door->getTimeOut(), 2);
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockAndUnlockWorkCorrectly) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());

    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
    EXPECT_EQ(door->getTimeOut(), 2);
}

TEST_F(TimedDoorTest, ThrowStateThrowsRuntimeError) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, TimeoutThrowsExceptionWhenDoorOpened) {
    timedDoor->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, TimeoutDoesNotThrowWhenDoorClosed) {
    timedDoor->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(TimerTest, TregisterCallsTimeoutAfterDelay) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer->tregister(0, mockClient);
}

TEST_F(TimerTest, TregisterWithNullptrDoesNotThrow) {
    EXPECT_NO_THROW(timer->tregister(0, nullptr));
}

TEST(DoorInterfaceTest, MockDoorLockIsCalled) {
    MockDoor door;
    EXPECT_CALL(door, lock()).Times(1);
    door.lock();
}

TEST(DoorInterfaceTest, MockDoorUnlockIsCalled) {
    MockDoor door;
    EXPECT_CALL(door, unlock()).Times(1);
    door.unlock();
}

TEST(DoorInterfaceTest, MockDoorIsDoorOpenedIsCalled) {
    MockDoor door;
    EXPECT_CALL(door, isDoorOpened()).Times(1)
        .WillOnce(::testing::Return(true));

    EXPECT_TRUE(door.isDoorOpened());
}

TEST(IntegrationTest, DoorOpenedAndTimerCausesException) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    Timer timer;

    door.unlock();
    EXPECT_THROW(timer.tregister(0, &adapter), std::runtime_error);
}

TEST(IntegrationTest, DoorClosedAndTimerDoesNotCauseException) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    Timer timer;

    door.lock();
    EXPECT_NO_THROW(timer.tregister(0, &adapter));
}

TEST(IntegrationTest, DoorClosedBeforeTimeoutNoException) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    Timer timer;

    door.unlock();

    std::thread closeDoor([&door]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        door.lock();
    });

    EXPECT_NO_THROW(timer.tregister(1, &adapter));
    closeDoor.join();
}

