#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, SliceTest)
{
    EXPECT_EQ("Hello", StringUtils::Slice("Hello World", 0, 5));
    EXPECT_EQ("", StringUtils::Slice("Hello", 3, 2));
}

TEST(StringUtilsTest, Capitalize)
{
  
    EXPECT_EQ("Hello", StringUtils::Capitalize("hello"));
    EXPECT_EQ("Hello", StringUtils::Capitalize("hEllO"));
    EXPECT_EQ("Hello", StringUtils::Capitalize("Hello"));
    EXPECT_EQ("A", StringUtils::Capitalize("a"));
}

TEST(StringUtilsTest, Upper)
{
    EXPECT_EQ("HELLO", StringUtils::Upper("hello"));
    EXPECT_EQ("ABC123!", StringUtils::Upper("AbC123!"));
    EXPECT_EQ("", StringUtils::Upper(""));
}

TEST(StringUtilsTest, Lower)
{
    EXPECT_EQ("hello", StringUtils::Lower("HELLO"));
    EXPECT_EQ("abc123!", StringUtils::Lower("AbC123!"));
    EXPECT_EQ("", StringUtils::Lower(""));
}

TEST(StringUtilsTest, LStrip)
{
    EXPECT_EQ("Hello  ", StringUtils::LStrip("   Hello  "));
    EXPECT_EQ("World", StringUtils::LStrip("World"));
    EXPECT_EQ("", StringUtils::LStrip("   "));
    EXPECT_EQ("", StringUtils::LStrip(""));
}

TEST(StringUtilsTest, RStrip)
{

    EXPECT_EQ("   Hello", StringUtils::RStrip("   Hello   "));
    EXPECT_EQ("World", StringUtils::RStrip("World"));
    EXPECT_EQ("", StringUtils::RStrip("   "));
    EXPECT_EQ("", StringUtils::RStrip(""));
}

TEST(StringUtilsTest, Strip)
{
    EXPECT_EQ("Hello", StringUtils::Strip("   Hello   "));
    EXPECT_EQ("World", StringUtils::Strip("World"));
    EXPECT_EQ("", StringUtils::Strip("   "));
    EXPECT_EQ("", StringUtils::Strip(""));
}

TEST(StringUtilsTest, Center)
{
    EXPECT_EQ("  Hello  ", StringUtils::Center("Hello", 9, ' '));
    EXPECT_EQ("--Hi---", StringUtils::Center("Hi", 7, '-'));
    EXPECT_EQ("Hello", StringUtils::Center("Hello", 3, '-'));
    EXPECT_EQ("Hello", StringUtils::Center("Hello", 5, '-'));
}

TEST(StringUtilsTest, LJust)
{
    EXPECT_EQ("Hello-----", StringUtils::LJust("Hello", 10, '-'));
    EXPECT_EQ("Hello", StringUtils::LJust("Hello", 4, '-'));
}

TEST(StringUtilsTest, RJust)
{
    EXPECT_EQ("-----Hello", StringUtils::RJust("Hello", 10, '-'));
    EXPECT_EQ("Hello", StringUtils::RJust("Hello", 3, '-'));
}

TEST(StringUtilsTest, Replace)
{
    EXPECT_EQ("Hello Jacky", StringUtils::Replace("Hello World", "World", "Jacky"));
    EXPECT_EQ("AAAA", StringUtils::Replace("ABAB", "B", "A"));
    EXPECT_EQ("Hello", StringUtils::Replace("Hello", "XYZ", "123"));
    EXPECT_EQ("Hello", StringUtils::Replace("Hello", "", "123"));
}

TEST(StringUtilsTest, Split)
{
    std::vector<std::string> result = StringUtils::Split("Hello World", " ");
    ASSERT_EQ(2u, result.size());
    EXPECT_EQ("Hello", result[0]);
    EXPECT_EQ("World", result[1]);

    result = StringUtils::Split("One,Two,Three", ",");
    ASSERT_EQ(3u, result.size());
    EXPECT_EQ("One", result[0]);
    EXPECT_EQ("Two", result[1]);
    EXPECT_EQ("Three", result[2]);

    result = StringUtils::Split("   Hello   World   ", "");
    ASSERT_EQ(2u, result.size());
    EXPECT_EQ("Hello", result[0]);
    EXPECT_EQ("World", result[1]);

    result = StringUtils::Split("Hello", "|");
    ASSERT_EQ(1u, result.size());
    EXPECT_EQ("Hello", result[0]);
}

TEST(StringUtilsTest, Join)
{
    std::vector<std::string> vect = {"Hello", "World"};
    EXPECT_EQ("HelloWorld", StringUtils::Join("", vect));
    EXPECT_EQ("Hello,World", StringUtils::Join(",", vect));
    vect = {"Hello"};
    EXPECT_EQ("Hello", StringUtils::Join(";", vect));
    vect = {};
    EXPECT_EQ("", StringUtils::Join(";", vect));
}

TEST(StringUtilsTest, ExpandTabs)
{
    EXPECT_EQ("Hello   World", StringUtils::ExpandTabs("Hello\tWorld", 8));
    EXPECT_EQ("AB  CD", StringUtils::ExpandTabs("AB\tCD", 4));
    EXPECT_EQ("Hello", StringUtils::ExpandTabs("Hello", 4));
}

TEST(StringUtilsTest, EditDistance)
{
    EXPECT_EQ(3, StringUtils::EditDistance("kitten", "sitting", false));
    EXPECT_EQ(0, StringUtils::EditDistance("ABC", "abc", true));
    EXPECT_EQ(2, StringUtils::EditDistance("cat", "catch", false));
    EXPECT_EQ(3, StringUtils::EditDistance("abc", "xyz", false));
    EXPECT_EQ(3, StringUtils::EditDistance("", "xyz", false));
    EXPECT_EQ(0, StringUtils::EditDistance("", "", false));
}