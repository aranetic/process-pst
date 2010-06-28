#include <cassert>
#include <stdexcept>
#include <sstream>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "edrm.h"
#include "xml_context.h"

using namespace std;
using boost::any;
using namespace boost::posix_time;

namespace {
    const char *value_of_unsupported_type = "";
}

// A type which we don't support outputting to EDRM files.
void edrm_tag_data_type_should_infer_type_from_value() {
    assert(L"Text" == edrm_tag_data_type(wstring()));
    assert(L"Text" == edrm_tag_data_type(vector<wstring>()));
    assert(L"Integer" == edrm_tag_data_type(int32_t(0)));
    assert(L"DateTime" == edrm_tag_data_type(ptime()));
    //assert(L"Decimal" == edrm_tag_data_type(0.0));  (Float, unused)
    assert(L"Boolean" == edrm_tag_data_type(true));
    assert(L"LongInteger" == edrm_tag_data_type(int64_t(0)));
}

void edrm_tag_data_type_should_raise_error_if_type_unknown() {
    bool caught_exception(false);
    try {
        edrm_tag_data_type(value_of_unsupported_type);
    } catch (std::exception &) {
        caught_exception = true;
    }
    assert(caught_exception);
}

void edrm_tag_value_should_format_value_appropriately() {
    assert(L"Text" == edrm_tag_value(wstring(L"Text")));

    vector<wstring> v;
    v.push_back(L"Foo");
    v.push_back(L"Bar");
    assert(L"Foo;Bar" == edrm_tag_value(v));

    assert(L"-1" == edrm_tag_value(int32_t(-1)));
    assert(L"2002-01-31T23:59:59Z" ==
           edrm_tag_value(from_iso_string("20020131T235959Z")));
    assert(L"true" == edrm_tag_value(true));
    assert(L"false" == edrm_tag_value(false));
    assert(L"-1" == edrm_tag_value(int64_t(-1)));
}

void edrm_tag_value_should_raise_error_if_type_unknown() {
    bool caught_exception(false);
    try {
        edrm_tag_value(value_of_unsupported_type);
    } catch (std::exception &) {
        caught_exception = true;
    }
    assert(caught_exception);
}

void edrm_context_should_have_xml_context_for_loadfile() {
    ostringstream out;
    edrm_context edrm(out);
    edrm.loadfile().lt("Root").slash_gt();
    assert("<?xml version='1.0' encoding='UTF-8'?>\n<Root/>\n" == out.str());
}

void edrm_context_should_generate_doc_ids() {
    ostringstream out;
    edrm_context edrm(out);
    assert("d0000001" == edrm.next_doc_id());
    assert("d0000002" == edrm.next_doc_id());
}

int edrm_spec(int argc, char **argv) {
    edrm_tag_data_type_should_infer_type_from_value();
    edrm_tag_data_type_should_raise_error_if_type_unknown();

    edrm_tag_value_should_format_value_appropriately();
    edrm_tag_value_should_raise_error_if_type_unknown();

    edrm_context_should_have_xml_context_for_loadfile();
    edrm_context_should_generate_doc_ids();

    return 0;
}
