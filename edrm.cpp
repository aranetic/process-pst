#include <vector>
#include <sstream>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <pstsdk/pst.h>

#include "utilities.h"
#include "document.h"
#include "edrm.h"
#include "xml_context.h"
#include "rfc822.h"

using namespace std;
using boost::any;
using boost::any_cast;
using boost::lexical_cast;
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

/// Generate a unique document identifier.  We try to keep these to 8
/// characters for the few remaining legal shops that use 8.3 filenames.
wstring edrm_context::next_doc_id() {
    size_t id = m_next_doc_id++;
    wostringstream out;
    out << L"d" << setw(7) << setfill(L'0') << id;
    return out.str();
}

void edrm_context::relationship(const wstring &type,
                                const wstring &parent_doc_id,
                                const wstring &child_doc_id) {
    relationship_info r(type, parent_doc_id, child_doc_id);
    m_relationships.push_back(r);
}

void edrm_context::output_relationships() {
    xml_context &x(loadfile());
    x.lt("Relationships").gt();
    
    BOOST_FOREACH(const relationship_info &r, m_relationships) {
        x.lt("Relationship")
            .attr("Type", r.type)
            .attr("ParentDocID", r.parent_doc_id)
            .attr("ChildDocID", r.child_doc_id)
            .slash_gt();
    }

    x.end_tag("Relationships");
}

namespace {
    wstring native_filename(const document &d) {
        wstring filename(d.id());
        any extension(d[L"#FileExtension"]);
        if (!extension.empty())
            filename += L"." + any_cast<wstring>(extension);
        return filename;
    }

    void output_tag(edrm_context &edrm, document::tag_iterator kv) {
        edrm.loadfile().lt("Tag")
            .attr("TagName", kv->first)
            .attr("TagValue", edrm_tag_value(kv->second))
            .attr("TagDataType", edrm_tag_data_type(kv->second))
            .slash_gt();
    }

    void output_file(edrm_context &edrm, const wstring &edrm_file_type,
                     const wstring &filename, const vector<uint8_t> &data) {
        wstring size(lexical_cast<wstring>(data.size()));
        wstring hash(string_to_wstring(md5(data)));

        xml_context &x(edrm.loadfile());
        x.lt("File").attr("FileType", edrm_file_type).gt();
        x.lt("ExternalFile")
            .attr("FileName", filename)
            .attr("FileSize", size)
            .attr("Hash", hash)
            .slash_gt();
        x.end_tag("File");
        
        path native_path(edrm.out_dir() / wstring_to_string(filename));
        ofstream f(native_path.file_string().c_str(),
                   ios_base::out | ios_base::trunc | ios_base::binary);
        f.write(reinterpret_cast<const char *>(&data[0]), data.size());
        f.close();
    }

    void output_eml_file(edrm_context &edrm, const document &d) {
        ostringstream eml;
        document_to_rfc822(eml, d);
        string eml_str(eml.str());
        output_file(edrm, L"Native", d.id() + L".eml",
                    vector<uint8_t>(eml_str.begin(), eml_str.end()));
    }

    void output_native_file(edrm_context &edrm, const document &d) {
        output_file(edrm, L"Native", native_filename(d), d.native());
    }

    void output_text_file(edrm_context &edrm, const document &d) {
        string utf8_str(wstring_to_utf8(d.text()));
        vector<uint8_t> utf8(utf8_str.begin(), utf8_str.end());
        output_file(edrm, L"Text", d.id() + L".txt", utf8);
    }

    void output_document(edrm_context &edrm, const document &d) {
        xml_context &x(edrm.loadfile());

        x.lt("Document")
            .attr("DocID", d.id())
            .attr("DocType", d.type_string())
            .gt();

        x.lt("Files").gt();
        if (d.type() == document::message) {
            output_eml_file(edrm, d);
        } else {
            if (d.has_native())
                output_native_file(edrm, d);
            if (d.has_text())
                output_text_file(edrm, d);
        }
        x.end_tag("Files");

        x.lt("Tags").gt();        
        document::tag_iterator ti(d.tag_begin());
        for (; ti != d.tag_end(); ++ti)
            output_tag(edrm, ti);
        x.end_tag("Tags");

        x.end_tag("Document");
    }

    void output_message(edrm_context &edrm, const message &m,
                        const document *attached_to = NULL);

    void output_attachment(edrm_context &edrm, const attachment &a,
                           const document *attached_to) {
        if (a.is_message()) {
            output_message(edrm, a.open_as_message(), attached_to);
        } else {
            document d(a);
            d.set_id(edrm.next_doc_id());
            output_document(edrm, d);
            edrm.relationship(L"Attachment", attached_to->id(), d.id());
        }
    }

    void output_message(edrm_context &edrm, const message &m,
                        const document *attached_to) {
        document d(m);
        d.set_id(edrm.next_doc_id());
        output_document(edrm, d);
        if (attached_to)
            edrm.relationship(L"Attachment", attached_to->id(), d.id());

        if (m.get_attachment_count() > 0) {
            message::attachment_iterator ai(m.attachment_begin());
            for (; ai != m.attachment_end(); ++ai)
                output_attachment(edrm, *ai, &d);
        }
    }
}

void convert_to_edrm(shared_ptr<pst> pst_file, ostream &loadfile,
                     const path &output_directory) {
    edrm_context edrm(loadfile, output_directory);
    xml_context &x(edrm.loadfile());

    x.lt("Root").attr("DataInterchangeType", L"Update").gt();
    x.lt("Batch").gt();
    x.lt("Documents").gt();

    pst::message_iterator mi(pst_file->message_begin());
    for (; mi != pst_file->message_end(); ++mi)
        output_message(edrm, *mi);

    x.end_tag("Documents");
    edrm.output_relationships();
    x.end_tag("Batch");
    x.end_tag("Root");
}
