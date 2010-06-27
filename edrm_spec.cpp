#include <cassert>

#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "edrm.h"

using namespace std;
using boost::any;
using namespace boost::posix_time;

void edrm_tag_data_type_should_infer_type_from_value() {
    assert(L"Text" == edrm_tag_data_type(wstring()));
    assert(L"Text" == edrm_tag_data_type(vector<wstring>()));
    assert(L"Integer" == edrm_tag_data_type(int32_t(0)));
    assert(L"DateTime" == edrm_tag_data_type(ptime()));
    //assert(L"Decimal" == edrm_tag_data_type(0.0));  (Float, unused)
    assert(L"Boolean" == edrm_tag_data_type(true));
    assert(L"LongInteger" == edrm_tag_data_type(int64_t(0)));
}

int edrm_spec(int argc, char **argv) {
    edrm_tag_data_type_should_infer_type_from_value();
    //edrm_tag_data_type_should_raise_error_if_type_unknown

    //edrm_tag_value_should_format_value_appropriately
    //edrm_tag_value_should_raise_error_if_type_unknown

    return 0;
}
