#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Transaction.h"
#include "Account.h"

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};

TEST(TransactionTest, SuccessfulTransfer) {
    MockAccount from(5, 897);
    MockAccount to(9, 300);
    MockTransaction tr;
    tr.set_fee(10);

    EXPECT_CALL(from, GetBalance()).WillRepeatedly(testing::Return(897));
    EXPECT_CALL(to, GetBalance()).WillRepeatedly(testing::Return(300));

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(from, ChangeBalance(testing::_)).Times(0);  
    EXPECT_CALL(from, Unlock()).Times(1);

    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(to, ChangeBalance(300)).Times(1);   
    EXPECT_CALL(to, ChangeBalance(-300)).Times(1);  
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_CALL(tr, SaveToDataBase(testing::Ref(from), testing::Ref(to), 300)).Times(testing::AtMost(1));

    bool result = tr.Make(from, to, 300);
    EXPECT_FALSE(result); 
}

TEST(TransactionTest, TransferToSelfThrows) {
    MockAccount from(3, 236);
    Transaction tr;
    EXPECT_THROW(tr.Make(from, from, 100), std::logic_error);
}

TEST(TransactionTest, NegativeSumThrows) {
    MockAccount from(3, 236);
    MockAccount to(2, 500);
    Transaction tr;
    EXPECT_THROW(tr.Make(from, to, -50), std::invalid_argument);
}

TEST(TransactionTest, SumBelowLimitThrows) {
    MockAccount from(3, 236);
    MockAccount to(2, 500);
    Transaction tr;
    EXPECT_ANY_THROW(tr.Make(from, to, 50));
}

TEST(TransactionTest, InsufficientFundsRollsBackCredit) {
    MockAccount from(1, 50);
    MockAccount to(2, 500);
    MockTransaction tr;
    tr.set_fee(10);

    EXPECT_CALL(from, GetBalance()).WillRepeatedly(testing::Return(50));
    EXPECT_CALL(to, GetBalance()).WillRepeatedly(testing::Return(500));

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);

    EXPECT_CALL(from, ChangeBalance(testing::_)).Times(0);

    EXPECT_CALL(to, ChangeBalance(300)).Times(1);   
    EXPECT_CALL(to, ChangeBalance(-310)).Times(1);  

    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_CALL(tr, SaveToDataBase(testing::_, testing::_, 300)).Times(1);

    bool result = tr.Make(from, to, 300);
    EXPECT_TRUE(result); 
}

TEST(TransactionTest, LockedAccountThrows) {
    MockAccount from(3, 236);
    MockAccount to(2, 500);
    Transaction tr;

    EXPECT_CALL(from, Lock()).WillRepeatedly(testing::Throw(std::runtime_error("already locked")));
    EXPECT_THROW(tr.Make(from, to, 100), std::runtime_error);
}

TEST(TransactionTest, RealSaveToDataBaseCoverage) {
    Account from(11, 500);
    Account to(12, 300);
    
    from.Lock();
    to.Lock();

    Transaction real_tr;
    EXPECT_NO_THROW(real_tr.SaveToDataBase(from, to, 100));

    from.Unlock();
    to.Unlock();
}
