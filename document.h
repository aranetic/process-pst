#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>
#include <boost/any.hpp>

/// An EDRM Document representing either a message or an ordinary file.
class document {
public:
    enum document_type {
        file,
        message
    };

private:
    std::wstring m_id;
    document_type m_type;
    std::wstring m_content_type;

    std::map<std::wstring, boost::any> m_tags;

public:
    std::wstring id() const { return m_id; }
    document &set_id(const std::wstring &id) { m_id = id; return *this; }

    document_type type() const { return m_type; }
    document &set_type(document_type t) { m_type = t; return *this; }

    std::wstring content_type() const { return m_content_type; }
    document &set_content_type(const std::wstring &ct)
        { m_content_type = ct; return *this; }

    boost::any &operator[](const std::wstring &key);
};

#endif // DOCUMENT_H
