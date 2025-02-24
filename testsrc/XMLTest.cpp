#include <gtest/gtest.h>
#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <memory>

class XMLTest : public ::testing::Test {
protected:
    std::shared_ptr<CStringDataSource> CreateSource(const std::string& data) {
        return std::make_shared<CStringDataSource>(data);
    }
    
    std::shared_ptr<CStringDataSink> CreateSink() {
        return std::make_shared<CStringDataSink>();
    }
};

TEST_F(XMLTest, BasicReaderTest) {
    auto source = CreateSource("<root><child>Hello</child></root>");
    CXMLReader reader(source);
    SXMLEntity entity;
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::StartElement, entity.DType);
    EXPECT_EQ("root", entity.DNameData);
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::StartElement, entity.DType);
    EXPECT_EQ("child", entity.DNameData);
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::CharData, entity.DType);
    EXPECT_EQ("Hello", entity.DNameData);
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::EndElement, entity.DType);
    EXPECT_EQ("child", entity.DNameData);
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::EndElement, entity.DType);
    EXPECT_EQ("root", entity.DNameData);
    
    EXPECT_TRUE(reader.End());
}

TEST_F(XMLTest, AttributeReaderTest) {
    auto source = CreateSource("<element attr1=\"value1\" attr2=\"value2\"/>");
    CXMLReader reader(source);
    SXMLEntity entity;
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::StartElement, entity.DType);
    EXPECT_EQ("element", entity.DNameData);
    EXPECT_EQ("value1", entity.AttributeValue("attr1"));
    EXPECT_EQ("value2", entity.AttributeValue("attr2"));
}

TEST_F(XMLTest, BasicWriterTest) {
    auto sink = CreateSink();
    CXMLWriter writer(sink);
    
    SXMLEntity root;
    root.DType = SXMLEntity::EType::StartElement;
    root.DNameData = "root";
    
    SXMLEntity text;
    text.DType = SXMLEntity::EType::CharData;
    text.DNameData = "Hello";
    
    SXMLEntity endRoot;
    endRoot.DType = SXMLEntity::EType::EndElement;
    endRoot.DNameData = "root";
    
    ASSERT_TRUE(writer.WriteEntity(root));
    ASSERT_TRUE(writer.WriteEntity(text));
    ASSERT_TRUE(writer.WriteEntity(endRoot));
    ASSERT_TRUE(writer.Flush());
    
    EXPECT_EQ("<root>Hello</root>", sink->String());
}

TEST_F(XMLTest, AttributeWriterTest) {
    auto sink = CreateSink();
    CXMLWriter writer(sink);
    
    SXMLEntity element;
    element.DType = SXMLEntity::EType::StartElement;
    element.DNameData = "element";
    element.SetAttribute("attr1", "value1");
    element.SetAttribute("attr2", "value2");
    
    SXMLEntity endElement;
    endElement.DType = SXMLEntity::EType::EndElement;
    endElement.DNameData = "element";
    
    ASSERT_TRUE(writer.WriteEntity(element));
    ASSERT_TRUE(writer.WriteEntity(endElement));
    ASSERT_TRUE(writer.Flush());
    
    EXPECT_EQ("<element attr1=\"value1\" attr2=\"value2\"></element>", sink->String());
}

TEST_F(XMLTest, SpecialCharacterTest) {
    auto sink = CreateSink();
    CXMLWriter writer(sink);
    
    SXMLEntity element;
    element.DType = SXMLEntity::EType::StartElement;
    element.DNameData = "element";
    
    SXMLEntity text;
    text.DType = SXMLEntity::EType::CharData;
    text.DNameData = "Hello & Goodbye < > World";
    
    SXMLEntity endElement;
    endElement.DType = SXMLEntity::EType::EndElement;
    endElement.DNameData = "element";
    
    ASSERT_TRUE(writer.WriteEntity(element));
    ASSERT_TRUE(writer.WriteEntity(text));
    ASSERT_TRUE(writer.WriteEntity(endElement));
    ASSERT_TRUE(writer.Flush());
    
    EXPECT_EQ("<element>Hello &amp; Goodbye &lt; &gt; World</element>", sink->String());
}

TEST_F(XMLTest, ReaderWriterIntegrationTest) {
    std::string original = "<root attr=\"value\"><child>Text</child></root>";
    auto source = CreateSource(original);
    auto sink = CreateSink();
    
    CXMLReader reader(source);
    CXMLWriter writer(sink);
    SXMLEntity entity;
    
    while (!reader.End() && reader.ReadEntity(entity)) {
        ASSERT_TRUE(writer.WriteEntity(entity));
    }
    ASSERT_TRUE(writer.Flush());
    
    EXPECT_EQ(original, sink->String());
}

TEST_F(XMLTest, EmptyElementTest) {
    auto source = CreateSource("<element/>");
    CXMLReader reader(source);
    SXMLEntity entity;
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::StartElement, entity.DType);
    EXPECT_EQ("element", entity.DNameData);
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(SXMLEntity::EType::EndElement, entity.DType);
    EXPECT_EQ("element", entity.DNameData);
    
    EXPECT_TRUE(reader.End());
}