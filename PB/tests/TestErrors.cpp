#include <gtest/gtest.h>

#include <PBError.h>

using namespace PB;

TEST(TestErrors, TestError) {
    PB::Error()<<"Test description"<<PB::ErrorKind::Unknown;
}
