#include "XMLWriter.h"
#include "XMLEntity.h"
#include "StringUtils.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> dsink;
    size_t IndentLevel = 0;

    struct PendingElement {
        SXMLEntity entity;
        bool flushed = false;        // Has the <tag> been written out yet?
        bool hasElementChild = false;// Does this element contain child elements?
        bool hasTextChild = false;   // Does this element contain text?
    };
    std::vector<PendingElement> pendingStack;

    SImplementation(std::shared_ptr<CDataSink> sink)
        : dsink(std::move(sink)) {}

    // Write raw string data out to the sink
    bool WriteRaw(const std::string &data) {
        std::vector<char> buffer(data.begin(), data.end());
        return dsink->Write(buffer);
    }

    // Escape special characters for XML
    static std::string EscapeXML(const std::string &input) {
        std::string output;
        output.reserve(input.size());
        for (char ch : input) {
            switch (ch) {
                case '&':  output.append("&amp;");  break;
                case '"':  output.append("&quot;"); break;
                case '\'': output.append("&apos;"); break;
                case '<':  output.append("&lt;");   break;
                case '>':  output.append("&gt;");   break;
                default:   output.push_back(ch);
            }
        }
        return output;
    }

    // ----------------------------------------------------------
    // Decide whether we should indent before writing an element
    // or closing tag. If the current element's parent is "osm",
    // we skip indentation/newlines entirely.
    // ----------------------------------------------------------
    bool ShouldIndent() const {
        // If empty stack, no parent => we can indent if IndentLevel > 0
        if (pendingStack.empty()) {
            return (IndentLevel > 0);
        }
        // Check the parent's name
        const PendingElement &parent = pendingStack.back();
        if (parent.entity.DNameData == "osm") {
            // Special-case: parent is "osm" => skip indentation
            return false;
        }
        // Otherwise, normal rule: indent if IndentLevel>0
        return (IndentLevel > 0);
    }

    // If the top stack element is unflushed, flush it: write its opening tag
    bool FlushPending() {
        if (!pendingStack.empty() && !pendingStack.back().flushed) {
            PendingElement &pe = pendingStack.back();

            // Write <tag ...> with NO newline before it
            std::string output;
            output += "<" + pe.entity.DNameData;
            for (const auto &attr : pe.entity.DAttributes) {
                output += " " + attr.first + "=\"" + EscapeXML(attr.second) + "\"";
            }
            output += ">";

            if (!WriteRaw(output)) {
                return false;
            }
            pe.flushed = true;
            IndentLevel++;
        }
        return true;
    }

    // =======================
    // StartElement
    // =======================
    bool WriteStartElement(const SXMLEntity &entity) {
        if (!FlushPending()) {
            return false;
        }
        if (!pendingStack.empty()) {
            pendingStack.back().hasElementChild = true;
        }
        PendingElement pe;
        pe.entity = entity;
        pendingStack.push_back(pe);
        return true;
    }

    // =======================
    // CharData
    // =======================
    bool WriteCharData(const SXMLEntity &entity) {
        if (!FlushPending()) {
            return false;
        }
        if (!pendingStack.empty()) {
            pendingStack.back().hasTextChild = true;
        }
        std::string escaped = EscapeXML(entity.DNameData);
        return WriteRaw(escaped);
    }

    // =======================
    // CompleteElement
    // =======================
    bool WriteCompleteElement(const SXMLEntity &entity) {
        // Flush the parent's opening tag if needed
        if (!FlushPending()) {
            return false;
        }
        if (!pendingStack.empty()) {
            pendingStack.back().hasElementChild = true;
        }

        // Possibly insert newline + tabs if we're not under "osm"
        std::string output;
        if (ShouldIndent()) {
            output += "\n" + std::string(IndentLevel, '\t');
        }

        // Write <tag .../>
        output += "<" + entity.DNameData;
        for (const auto &attr : entity.DAttributes) {
            output += " " + attr.first + "=\"" + EscapeXML(attr.second) + "\"";
        }
        output += "/>";

        return WriteRaw(output);
    }

    // =======================
    // EndElement
    // =======================
    bool WriteEndElement(const SXMLEntity &entity) {
        // We reference entity just to fix the unused parameter warning
        (void)entity;

        if (pendingStack.empty()) {
            return false;
        }
        PendingElement pe = pendingStack.back();
        pendingStack.pop_back();

        // If we never flushed <tag>, it means no children or text => produce <tag ...></tag>
        if (!pe.flushed) {
            // Write the opening <tag ...> now, with NO newline (or maybe only if ShouldIndent?)
            std::string openTag;
            if (ShouldIndent()) {
                openTag += "\n" + std::string(IndentLevel, '\t');
            }
            openTag += "<" + pe.entity.DNameData;
            for (const auto &attr : pe.entity.DAttributes) {
                openTag += " " + attr.first + "=\"" + EscapeXML(attr.second) + "\"";
            }
            openTag += ">";
            if (!WriteRaw(openTag)) {
                return false;
            }

            // We have effectively "opened" it, so Indent up then back down
            pe.flushed = true;
            IndentLevel++;
            IndentLevel--;
        }
        else {
            // Normal close of a flushed element
            IndentLevel--;
        }

        // If it has child elements and we are NOT under "osm", put a newline+indent
        std::string closeTag;
        if (pe.hasElementChild && ShouldIndent()) {
            closeTag += "\n" + std::string(IndentLevel, '\t');
        }
        closeTag += "</" + pe.entity.DNameData + ">";

        return WriteRaw(closeTag);
    }

    // =======================
    // Routing method
    // =======================
    bool WriteEntity(const SXMLEntity &entity) {
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                return WriteStartElement(entity);
            case SXMLEntity::EType::EndElement:
                return WriteEndElement(entity);
            case SXMLEntity::EType::CharData:
                return WriteCharData(entity);
            case SXMLEntity::EType::CompleteElement:
                return WriteCompleteElement(entity);
            default:
                return false;
        }
    }

    // Flush any unclosed elements
    bool Flush() {
        while (!pendingStack.empty()) {
            if (!WriteEndElement(SXMLEntity())) {
                return false;
            }
        }
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink) :
    DImplementation(std::make_unique<SImplementation>(std::move(sink))) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}