#include <vector>
#include <sstream>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <pstsdk/pst.h>

#include "utilities.h"
#include "document.h"
#include "edrm.h"

using namespace std;
using boost::any;
using boost::any_cast;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace pstsdk;

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
    wstring to_tag_value(const ptime &value) {
        return string_to_wstring(to_iso_extended_string(value)) + L"Z";
    }

    template <>
    wstring to_tag_value(const bool &value) {
        return value ? L"true" : L"false";
    }
}

// Convert a C++ value to an EDRM TagValue string for serialization to XML.
wstring edrm_tag_value(const any &value) {
    if (value.type() == typeid(wstring))
        return any_cast<wstring>(value);
    else if (value.type() == typeid(vector<wstring>))
        return to_tag_value(any_cast<vector<wstring> >(value));
    else if (value.type() == typeid(int32_t))
        return to_tag_value(any_cast<int32_t>(value));
    else if (value.type() == typeid(ptime))
        return to_tag_value(any_cast<ptime>(value));
    else if (value.type() == typeid(bool))
        return to_tag_value(any_cast<bool>(value));
    else if (value.type() == typeid(int64_t))
        return to_tag_value(any_cast<int64_t>(value));

    throw runtime_error("Unable to output EDRM TagValue for value");
}

void convert_to_edrm(shared_ptr<pst> pst_file, ostream &loadfile,
                     const path &output_directory) {
    loadfile << "<?xml version='1.0' encoding='UTF-8'?>" << endl
             << "<Root DataInterchangeType='Update'>" << endl
             << "  <Batch>" << endl
             << "    <Documents>" << endl;

    pst::message_iterator mi(pst_file->message_begin());
    for (; mi != pst_file->message_end(); ++mi) {
        document d(*mi);
        loadfile << "      <Document DocType='Message'>" << endl
                 << "        <Tags>" << endl;
        
        document::tag_iterator ti(d.tag_begin());
        for (; ti != d.tag_end(); ++ti) {
            loadfile << "          <Tag TagName='" << xml_quote(ti->first)
                     << "' TagValue='" << xml_quote(edrm_tag_value(ti->second))
                     << "' TagDataType='"
                     << xml_quote(edrm_tag_data_type(ti->second)) << "' />"
                     << endl;
        }

        loadfile << "        </Tags>" << endl
                 << "      </Document>" << endl;
    }

    loadfile << "    </Documents>" << endl
             << "    <Relationships>" << endl
             << "    </Relationships>" << endl
             << "  </Batch>" << endl
             << "</Root>" << endl;
}
