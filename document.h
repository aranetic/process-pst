#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>
#include <boost/any.hpp>

class document {
    std::map<std::wstring, boost::any> m_tags;

public:
    boost::any &operator[](const std::wstring &key);
};

#endif // DOCUMENT_H
