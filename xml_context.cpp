#include <stdexcept>

#include "utilities.h"
#include "xml_context.h"

using namespace std;

void xml_context::indent() {
    for (int i = 0; i < m_indent; ++i)
        m_out << "  ";
}

xml_context::xml_context(ostream &out) : m_out(out), m_indent(0) {
    m_out << "<?xml version='1.0' encoding='UTF-8'?>" << endl;
}

xml_context &xml_context::lt(const string &tag_name) {
    indent();
    m_out << "<" << tag_name;
    return *this;
}

xml_context &xml_context::attr(const string &name, const wstring &value) {
    m_out << " " << name << "='" << xml_quote(value) << "'";
    return *this;
}

void xml_context::gt() {
    m_out << ">" << endl;
    ++m_indent;
}

void xml_context::slash_gt() {
    m_out << "/>" << endl;
}

void xml_context::end_tag(const string &tag_name) {
    if (m_indent < 1)
        throw runtime_error("Unbalanced tags in XML");
    --m_indent;
    indent();
    m_out << "</" << tag_name << ">" << endl;
}
