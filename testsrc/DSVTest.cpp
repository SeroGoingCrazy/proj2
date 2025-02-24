#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

class DSVTest : public ::testing::Test {
protected:
    std::shared_ptr<CStringDataSource> CreateSource(const std::string& data) {
        return std::make_shared<CStringDataSource>(data);
    }
    
    std::shared_ptr<CStringDataSink> CreateSink() {
        return std::make_shared<CStringDataSink>();
    }
};

TEST_F(DSVTest, ReadSingleRow) {
    std::string data = "Jacky,21,CS\n";
    auto dataSource = CreateSource(data);
    CDSVReader reader(dataSource, ',');
    std::vector<std::string> row;
    
    ASSERT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "Jacky");
    EXPECT_EQ(row[1], "21");
    EXPECT_EQ(row[2], "CS");
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, ReadMultipleRows) {
    std::string data = "Jacky,21,CS\nKelly,20,Communication\n";
    auto dataSource = CreateSource(data);
    CDSVReader reader(dataSource, ',');
    std::vector<std::string> row;
    
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row[0], "Jacky");
    EXPECT_EQ(row[1], "21");
    EXPECT_EQ(row[2], "CS");
    
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row[0], "Kelly");
    EXPECT_EQ(row[1], "20");
    EXPECT_EQ(row[2], "Communication");
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, ReadQuotedValues) {
    std::string data = "\"Jacky, Wang\",21,CS\n";
    auto dataSource = CreateSource(data);
    CDSVReader reader(dataSource, ',');
    std::vector<std::string> row;
    
    ASSERT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "Jacky, Wang");
    EXPECT_EQ(row[1], "21");
    EXPECT_EQ(row[2], "CS");
    EXPECT_TRUE(reader.End());
}


TEST_F(DSVTest, WriteSingleRow) {
    auto dataSink = CreateSink();
    CDSVWriter writer(dataSink, ',', false);
    std::vector<std::string> row = {"Jacky", "21", "CS"};
    
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(dataSink->String(), "Jacky,21,CS\n");
}

TEST_F(DSVTest, WriteMultipleRows) {
    auto dataSink = CreateSink();
    CDSVWriter writer(dataSink, ',', false);
    std::vector<std::string> row1 = {"Jacky", "21", "CS"};
    std::vector<std::string> row2 = {"Kelly", "20", "Communication"};
    
    ASSERT_TRUE(writer.WriteRow(row1));
    ASSERT_TRUE(writer.WriteRow(row2));
    EXPECT_EQ(dataSink->String(), "Jacky,21,CS\nKelly,20,Communication\n");
}

TEST_F(DSVTest, WriteQuotedValues) {
    auto dataSink = CreateSink();
    CDSVWriter writer(dataSink, ',', false);
    std::vector<std::string> row = {"Jacky, Wang", "21", "CS"};
    
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(dataSink->String(), "\"Jacky, Wang\",21,CS\n");
}

TEST_F(DSVTest, WriteEmptyFields) {
    auto dataSink = CreateSink();
    CDSVWriter writer(dataSink, ',', false);
    std::vector<std::string> row = {"Jacky", "", "CS"};
    
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(dataSink->String(), "Jacky,,CS\n");
}

TEST_F(DSVTest, WriteWithForceQuoting) {
    // 1. Create a data sink for capturing the CSV output.
    auto dataSink = CreateSink(); 

    // 2. Create a writer with (delimiter = ',', forceQuoting = true).
    //    forceQuoting = true => every field is enclosed in quotes,
    //    and internal quotes in the data are doubled.
    CDSVWriter writer(dataSink, ',', true);

    // 3. Prepare a row that includes internal double quotes: 
    //    - In C++ literals, each " must be escaped as \".
    //    - So writing "My name is \"\"Bob\"\"!" in code 
    //      becomes My name is ""Bob""! in the CSV data.
// In C++ source, escape a single quote with \"
// so that it is truly only one quote in memory.
// Then the CSV writer doubles each one => "" in the final CSV.

    std::vector<std::string> row = {
        "2",
        "My name is \"Bob\"!", // <--- Only one quote on each side of Bob
        "3.3"
    };

    // 4. Write the row.
    ASSERT_TRUE(writer.WriteRow(row));

    // 5. Because forceQuoting is enabled, the output should be:
    //    "2","My name is ""Bob""!","3.3"
    //    followed by a newline.
    //
    // Make sure the EXPECT_EQ string has its own escaped quotes:
    //    "\"2\",\"My name is \"\"Bob\"\"!\",\"3.3\"\n"
    //
    // Explanation of escapes:
    // - Outer quotes for the C++ string literal
    // - \"2\" => "2"
    // - \"My name is \"\"Bob\"\"!\" => "My name is ""Bob""!"
    // - \"3.3\" => "3.3"
    // - \n => newline
    //
    // That's exactly what we compare to:
    EXPECT_EQ(dataSink->String(), "\"2\",\"My name is \"\"Bob\"\"!\",\"3.3\"\n");
}
