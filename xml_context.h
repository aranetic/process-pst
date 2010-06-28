#ifndef XML_CONTEXT_H
#define XML_CONTEXT_H

#include <string>
#include <iostream>

/// A somewhat eccentric XML writer class refactored out of the EDRM
/// output code.  The API is decidedly odd, but it seems to match our
/// needs.
class xml_context {
    std::ostream &m_out;
    int m_indent;

    void indent();

public:
    xml_context(std::ostream &out);

    xml_context &lt(const std::string &tag_name);
    xml_context &attr(const std::string &name, const std::wstring &value);
    void gt();
    void slash_gt();

    void end_tag(const std::string &tag_name);
};

#endif // XML_CONTEXT_H
