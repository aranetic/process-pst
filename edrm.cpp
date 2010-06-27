#include <vector>
#include <sstream>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>

#include "utilities.h"

using namespace std;
using boost::any;
using boost::any_cast;
using namespace boost::posix_time;

/// Return an official EDRM TagDataType string for 'value'.
wstring edrm_tag_data_type(const any &value) {
    if (value.type() == typeid(wstring))
        return L"Text";
    else if (value.type() == typeid(vector<wstring>))
        return L"Text";
    else if (value.type() == typeid(int32_t))
        return L"Integer";
    else if (value.type() == typeid(ptime))
        return L"DateTime";
    else if (value.type() == typeid(bool))
        return L"Boolean";
    else if (value.type() == typeid(int64_t))
        return L"LongInteger";

    throw runtime_error("Unable to determine EDRM TagDataType for value");
}

namespace {
    template <typename T>
    wstring to_tag_value(const T& value) {
        wostringstream out;
        out << value;
        return out.str();
    }

    template <>
    wstring to_tag_value(const vector<wstring> &values) {
        wstring result;
        bool first = true;
        BOOST_FOREACH(wstring v, values) {
            if (first)
                first = false;
            else
                result += L";";
            result += v;
        }
        return result;
    }

    template <>
    wstring to_tag_value(const bool &value) {
        return value ? L"true" : L"false";
    }
}

// Convert a C++ value to an EDRM TagValue string for serialization to XML.
wstring edrm_tag_value(const any &value) {
    if (value.type() == typeid(wstring))
        return to_tag_value(any_cast<wstring>(value));
    else if (value.type() == typeid(vector<wstring>))
        return to_tag_value(any_cast<vector<wstring> >(value));
    else if (value.type() == typeid(int32_t))
        return to_tag_value(any_cast<int32_t>(value));
    //else if (value.type() == typeid(ptime))
    //    return to_tag_value(any_cast<ptime>(value));;
    else if (value.type() == typeid(bool))
        return to_tag_value(any_cast<bool>(value));
    else if (value.type() == typeid(int64_t))
        return to_tag_value(any_cast<int64_t>(value));

    throw runtime_error("Unable to output EDRM TagValue for value");
}
