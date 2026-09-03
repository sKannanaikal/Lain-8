/**
 *
 *
 *
 *
 *
 *
 */

#include <gtest/gtest.h>

/**
* @brief basic exemplar test template
*
*/
TEST(HelloTest, BasicAssertions)
{
	EXPECT_STRNE("hello", "world");
	EXPECT_EQ(7*6, 42);
}
