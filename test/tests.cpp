// Copyright 2025 UNN-CS Team

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
        door = new TimedDoor(3);
    }

    void TearDown() override {
        delete door;
    }

    TimedDoor* door;
};

class DoorTimerAdapterTest : public ::testing::Test {
 protected:
    void SetUp() override {
        timedDoor = new TimedDoor(3);
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

TEST_F(TimedDoorTest, TochnoUnikalnyTest1) {
    EXPECT_FALSE(door->isDoorOpened());
    EXPECT_EQ(door->getTimeOut(), 3);
}

TEST_F(TimedDoorTest, TochnoUnikalnyTest2) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TochnoUnikalnyTest3) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TochnoUnikalnyTest4) {
    for (int cycle = 0; cycle < 5; ++cycle) {
        door->unlock();
        EXPECT_TRUE(door->isDoorOpened());
        door->lock();
        EXPECT_FALSE(door->isDoorOpened());
    }
}

TEST_F(TimedDoorTest, TochnoUnikalnyTest5) {
    TimedDoor customDoor(7);
    EXPECT_EQ(customDoor.getTimeOut(), 7);
}

TEST_F(TimedDoorTest, TochnoUnikalnyTest6) {
    EXPECT_THROW({
        try {
            door->throwState();
        } catch (const std::runtime_error& e) {
            EXPECT_STREQ(e.what(),
                         "Error: Door is open after timeout!");
            throw;
        }
    }, std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, TochnoUnikalnyTest7) {
    timedDoor->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(DoorTimerAdapterTest, TochnoUnikalnyTest8) {
    timedDoor->lock();
    EXPECT_NO_THROW(adapter->Timeout());
}

TEST_F(DoorTimerAdapterTest, TochnoUnikalnyTest9) {
    timedDoor->unlock();
    timedDoor->lock();
    timedDoor->unlock();
    EXPECT_THROW(adapter->Timeout(), std::runtime_error);
}

TEST_F(TimerTest, TochnoUnikalnyTest10) {
    EXPECT_CALL(*mockClient, Timeout()).Times(1);
    timer->tregister(0, mockClient);
}

TEST_F(TimerTest, TochnoUnikalnyTest11) {
    EXPECT_NO_THROW(timer->tregister(0, nullptr));
}

TEST(MockDoorVerification, TochnoUnikalnyTest12) {
    MockDoor door;
    EXPECT_CALL(door, lock()).Times(1);
    door.lock();
}

TEST(MockDoorVerification, TochnoUnikalnyTest13) {
    MockDoor door;
    EXPECT_CALL(door, unlock()).Times(1);
    door.unlock();
}

TEST(MockDoorVerification, TochnoUnikalnyTest14) {
    MockDoor door;
    EXPECT_CALL(door, isDoorOpened())
        .Times(1)
        .WillOnce(::testing::Return(false));
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(FullSystemCheck, TochnoUnikalnyTest15) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    Timer timer;

    door.unlock();
    EXPECT_THROW(timer.tregister(0, &adapter), std::runtime_error);
}

TEST(FullSystemCheck, TochnoUnikalnyTest16) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    Timer timer;

    door.lock();
    EXPECT_NO_THROW(timer.tregister(0, &adapter));
}

TEST(FullSystemCheck, TochnoUnikalnyTest17) {
    TimedDoor door(2);
    DoorTimerAdapter adapter(door);
    Timer timer;

    door.unlock();

    std::thread autoClose([&door]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        door.lock();
    });

    EXPECT_NO_THROW(timer.tregister(1, &adapter));
    autoClose.join();
}
