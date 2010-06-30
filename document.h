#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>
#include <boost/any.hpp>

namespace pstsdk {
    class message;
    class attachment;
}

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

    typedef std::map<std::wstring, boost::any> tag_map;
    tag_map m_tags;

    bool m_has_native;
    std::vector<uint8_t> m_native;
    bool m_has_text;
    std::wstring m_text;
    bool m_has_html;
    std::vector<uint8_t> m_html;

    void initialize_fields();
    void initialize_from_message(const pstsdk::message &m);

public:
    typedef tag_map::const_iterator tag_iterator;

    document() { initialize_fields(); }
    explicit document(const pstsdk::message &m);
    explicit document(const pstsdk::attachment &a);

    std::wstring id() const { return m_id; }
    document &set_id(const std::wstring &id) { m_id = id; return *this; }

    document_type type() const { return m_type; }
    document &set_type(document_type t) { m_type = t; return *this; }
    std::wstring type_string() const;

    std::wstring content_type() const { return m_content_type; }
    document &set_content_type(const std::wstring &ct)
        { m_content_type = ct; return *this; }

    boost::any &operator[](const std::wstring &key);
    const boost::any operator[](const std::wstring &key) const;

    tag_iterator tag_begin() const { return m_tags.begin(); }
    tag_iterator tag_end() const { return m_tags.end(); }

    /// Set the native file associated with this document.
    void set_native(const std::vector<uint8_t> &native);

    /// Does this document have an associated native file?
    bool has_native() const { return m_has_native; }

    /// The native file associated with this document.
    /// \pre has_native() == true
    const std::vector<uint8_t> &native() const { return m_native; }

    /// Set the plain text associated with this document.
    void set_text(const std::wstring &text);

    /// Does this document have associated plain text?
    bool has_text() const { return m_has_text; }

    /// The plain text associated with this document.
    /// \pre has_text() == true
    const std::wstring &text() const { return m_text; }

    /// Set the HTML associated with this document.
    void set_html(const std::vector<uint8_t> &html);

    /// Does this document have associated HTML?
    bool has_html() const { return m_has_html; }

    /// An HTML representation of this document.
    /// \pre has_html() == true
    const std::vector<uint8_t> &html() const { return m_html; }
};

#endif // DOCUMENT_H
