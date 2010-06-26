#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pstsdk/pst.h>

#include "document.h"

using namespace std;
using boost::any;
using namespace boost::posix_time;
using namespace pstsdk;

namespace {
    // Rather than spend quite so much time checking for exceptions, let's
    // build a simple wrapper function which checks for us.
    template <typename R, typename T>
    bool has_prop(const pstsdk::message &m, R (T::*pmf)() const) {
        try {
            (m.*pmf)();
            return true;
        } catch (key_not_found<prop_id> &) {
            return false;
        }
    }
}

document::document(const pstsdk::message &m) {
    property_bag props(m.get_property_bag());

    if (has_prop(m, &message::get_subject))
        (*this)[L"#Subject"] = wstring(m.get_subject());

    if (props.prop_exists(0x007d)) // PidTagTransportMessageHeaders
        (*this)[L"#Header"] = props.read_prop<wstring>(0x007d);

    if (props.prop_exists(0x0039)) // PidTagClientSubmitTime
        (*this)[L"#DateSent"] = from_time_t(props.read_time_t_prop(0x0039));

    if (props.prop_exists(0x0e06)) // PidTagMessageDeliveryTime
        (*this)[L"#DateReceived"] = from_time_t(props.read_time_t_prop(0x0e06));

    if (props.prop_exists(0x0e07)) // PidTagMessageFlags
        (*this)[L"#ReadFlag"] =
            (props.read_prop<int32_t>(0x0e07) & 0x1) ? true : false;
}

any &document::operator[](const wstring &key) {
    return m_tags[key];
}
