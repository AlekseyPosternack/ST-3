// Copyright 2025 UNN-CS Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <memory>
#include <limits>
#include "TimedDoor.h"

struct MockTimerClient : TimerClient {
    MOCK_METHOD(void, Timeout, (), (override));
};

struct MockDoor : Door {
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

TEST(TochnoUnikalnyTest1, TochnoUnikalnyTest1) {
    TimedDoor d(1337);
    EXPECT_FALSE(d.isDoorOpened());
    EXPECT_EQ(d.getTimeOut(), 1337);
}

TEST(TochnoUnikalnyTest2, TochnoUnikalnyTest2) {
    TimedDoor d(1337);
    d.unlock();
    EXPECT_TRUE(d.isDoorOpened());
}

TEST(TochnoUnikalnyTest3, TochnoUnikalnyTest3) {
    TimedDoor d(1337);
    d.unlock();
    d.lock();
    EXPECT_FALSE(d.isDoorOpened());
}

TEST(TochnoUnikalnyTest4, TochnoUnikalnyTest4) {
    TimedDoor d(1337);
    for (int i = 0; i < 420; ++i) {
        d.unlock();
        EXPECT_TRUE(d.isDoorOpened());
        d.lock();
        EXPECT_FALSE(d.isDoorOpened());
    }
}

TEST(TochnoUnikalnyTest5, TochnoUnikalnyTest5) {
    TimedDoor d(314159);
    EXPECT_EQ(d.getTimeOut(), 314159);
}

TEST(TochnoUnikalnyTest6, TochnoUnikalnyTest6) {
    TimedDoor d(1337);
    EXPECT_THROW({
        try {
            d.throwState();
        } catch (const std::runtime_error& e) {
            EXPECT_STREQ(e.what(), "Error: Door is open after timeout!");
            throw;
        }
    }, std::runtime_error);
}

TEST(TochnoUnikalnyTest7, TochnoUnikalnyTest7) {
    TimedDoor d(666);
    DoorTimerAdapter a(d);
    d.unlock();
    EXPECT_THROW(a.Timeout(), std::runtime_error);
}

TEST(TochnoUnikalnyTest8, TochnoUnikalnyTest8) {
    TimedDoor d(666);
    DoorTimerAdapter a(d);
    d.lock();
    EXPECT_NO_THROW(a.Timeout());
}

TEST(TochnoUnikalnyTest9, TochnoUnikalnyTest9) {
    TimedDoor d(666);
    DoorTimerAdapter a(d);
    d.unlock();
    d.lock();
    d.unlock();
    EXPECT_THROW(a.Timeout(), std::runtime_error);
}

TEST(TochnoUnikalnyTest10, TochnoUnikalnyTest10) {
    Timer t;
    MockTimerClient c;
    EXPECT_CALL(c, Timeout()).Times(1);
    t.tregister(0, &c);
}

TEST(TochnoUnikalnyTest11, TochnoUnikalnyTest11) {
    Timer t;
    EXPECT_NO_THROW(t.tregister(0, nullptr));
}

TEST(TochnoUnikalnyTest12, TochnoUnikalnyTest12) {
    MockDoor d;
    EXPECT_CALL(d, lock()).Times(9999);
    for (int i = 0; i < 9999; ++i) d.lock();
}

TEST(TochnoUnikalnyTest13, TochnoUnikalnyTest13) {
    MockDoor d;
    EXPECT_CALL(d, unlock()).Times(100500);
    for (int i = 0; i < 100500; ++i) d.unlock();
}

TEST(TochnoUnikalnyTest14, TochnoUnikalnyTest14) {
    MockDoor d;
    EXPECT_CALL(d, isDoorOpened())
        .Times(42)
        .WillRepeatedly(::testing::Return(false));
    for (int i = 0; i < 42; ++i) EXPECT_FALSE(d.isDoorOpened());
}

TEST(TochnoUnikalnyTest15, TochnoUnikalnyTest15) {
    TimedDoor d(67);
    DoorTimerAdapter a(d);
    Timer t;
    d.unlock();
    EXPECT_THROW(t.tregister(0, &a), std::runtime_error);
}

TEST(TochnoUnikalnyTest16, TochnoUnikalnyTest16) {
    TimedDoor d(69);
    DoorTimerAdapter a(d);
    Timer t;
    d.lock();
    EXPECT_NO_THROW(t.tregister(0, &a));
}

TEST(TochnoUnikalnyTest17, TochnoUnikalnyTest17) {
    TimedDoor d(228);
    DoorTimerAdapter a(d);
    Timer t;
    d.unlock();
    std::thread closer([&d]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1337));
        d.lock();
    });
    EXPECT_NO_THROW(t.tregister(2, &a));
    closer.join();
}

TEST(TochnoUnikalnyTest18, TochnoUnikalnyTest18) {
    TimedDoor d(1337);
    for (int i = 0; i < 666; ++i) d.unlock();
    EXPECT_TRUE(d.isDoorOpened());
}

TEST(TochnoUnikalnyTest19, TochnoUnikalnyTest19) {
    TimedDoor d(1337);
    for (int i = 0; i < 1337; ++i) d.lock();
    EXPECT_FALSE(d.isDoorOpened());
}

TEST(TochnoUnikalnyTest20, TochnoUnikalnyTest20) {
    TimedDoor d1(42), d2(666), d3(1337);
    EXPECT_EQ(d1.getTimeOut(), 42);
    EXPECT_EQ(d2.getTimeOut(), 666);
    EXPECT_EQ(d3.getTimeOut(), 1337);
}

TEST(TochnoUnikalnyTest21, TochnoUnikalnyTest21) {
    TimedDoor d(666);
    DoorTimerAdapter a(d);
    d.unlock();
    EXPECT_THROW(a.Timeout(), std::runtime_error);
    d.lock();
    EXPECT_NO_THROW(a.Timeout());
}

TEST(TochnoUnikalnyTest22, TochnoUnikalnyTest22) {
    TimedDoor d(666);
    DoorTimerAdapter a(d);
    for (int i = 0; i < 999; ++i) {
        d.lock();
        EXPECT_NO_THROW(a.Timeout());
    }
}

TEST(TochnoUnikalnyTest23, TochnoUnikalnyTest23) {
    Timer t;
    MockTimerClient c;
    EXPECT_CALL(c, Timeout()).Times(420);
    for (int i = 0; i < 420; ++i) t.tregister(0, &c);
}

TEST(TochnoUnikalnyTest24, TochnoUnikalnyTest24) {
    MockDoor d;
    EXPECT_CALL(d, isDoorOpened())
        .Times(1)
        .WillOnce(::testing::Return(true));
    EXPECT_TRUE(d.isDoorOpened());
}

TEST(TochnoUnikalnyTest25, TochnoUnikalnyTest25) {
    MockDoor d;
    ::testing::InSequence seq;
    EXPECT_CALL(d, unlock()).Times(69);
    EXPECT_CALL(d, isDoorOpened())
        .WillOnce(::testing::Return(true));
    EXPECT_CALL(d, lock()).Times(420);
    EXPECT_CALL(d, isDoorOpened())
        .WillOnce(::testing::Return(false));
    for (int i = 0; i < 69; ++i) d.unlock();
    d.isDoorOpened();
    for (int i = 0; i < 420; ++i) d.lock();
    d.isDoorOpened();
}

TEST(TochnoUnikalnyTest26, TochnoUnikalnyTest26) {
    TimedDoor d(999);
    DoorTimerAdapter a(d);
    Timer t;
    for (int i = 0; i < 1337; ++i) {
        d.lock();
        d.unlock();
        d.lock();
    }
    EXPECT_NO_THROW(t.tregister(0, &a));
}

TEST(TochnoUnikalnyTest27, TochnoUnikalnyTest27) {
    int values[] = {0, 1, -42, 666, 1337, 314159, 999999,
                    std::numeric_limits<int>::max()};
    for (int v : values) {
        TimedDoor d(v);
        EXPECT_EQ(d.getTimeOut(), v);
    }
}

TEST(TochnoUnikalnyTest28, TochnoUnikalnyTest28) {
    bool states[] = {true, false};
    for (bool s : states) {
        TimedDoor d(420);
        if (s) d.unlock();
        EXPECT_EQ(d.isDoorOpened(), s);
    }
}
