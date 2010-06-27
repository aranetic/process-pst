#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>
#include <boost/any.hpp>

namespace pstsdk { class message; }

/// An EDRM Document representing either a message or an ordinary file.
class document {
public:
    enum document_type {
        unknown,
        file,
        message
    };

private:
    std::wstring m_id;
    document_type m_type;
    std::wstring m_content_type;

    std::map<std::wstring, boost::any> m_tags;

    bool m_has_text;
    std::wstring m_text;
    std::vector<uint8_t> m_native;

    void initialize_fields();
    void initialize_from_message(const pstsdk::message &m);

public:
    document() { initialize_fields(); }
    document(const pstsdk::message &m);
    document(const pstsdk::attachment &a);

    std::wstring id() const { return m_id; }
    document &set_id(const std::wstring &id) { m_id = id; return *this; }

    document_type type() const { return m_type; }
    document &set_type(document_type t) { m_type = t; return *this; }

    std::wstring content_type() const { return m_content_type; }
    document &set_content_type(const std::wstring &ct)
        { m_content_type = ct; return *this; }

    boost::any &operator[](const std::wstring &key);

    /// The native file associated with this document.
    const std::vector<uint8_t> &native() const { return m_native; }

    /// Does this document have associated plain text?
    bool has_text() const { return m_has_text; }

    /// The plain text associated with this document.
    /// \pre has_text() == true
    const std::wstring &text() const { return m_text; }
};

#endif // DOCUMENT_H
