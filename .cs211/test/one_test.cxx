#include <catch.hxx>

constexpr int N = 1 << 20;

void helper(int limit);

TEST_CASE("i == i")
{
    helper(N);
}

void helper(int limit)
{
    for (int i = 0; i < limit; ++i)
        CHECK( i == i );
}

