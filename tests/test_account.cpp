#include <gtest/gtest.h>
#include "Account.h"

TEST(AccountTest, ConstructorAndGetters) {
    Account acc(123, 5000);
    EXPECT_EQ(acc.id(), 123);
    EXPECT_EQ(acc.GetBalance(), 5000);
}

TEST(AccountTest, ChangeBalanceWorksWhenLocked) {
    Account acc(1, 1000);
    acc.Lock();
    acc.ChangeBalance(500);
    EXPECT_EQ(acc.GetBalance(), 1500);
    
    acc.ChangeBalance(-300);
    EXPECT_EQ(acc.GetBalance(), 1200);
    acc.Unlock();
}

TEST(AccountTest, ChangeBalanceThrowsWhenNotLocked) {
    Account acc(1, 100);
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
}

TEST(AccountTest, DoubleLockThrows) {
    Account acc(1, 100);
    acc.Lock();
    EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(AccountTest, UnlockWithoutLockDoesNothing) {
    Account acc(1, 100);
    EXPECT_NO_THROW(acc.Unlock());
}
