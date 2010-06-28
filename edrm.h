#ifndef EDRM_H
#define EDRM_H

#include <string>
#include <boost/filesystem.hpp>

namespace boost { class any; }
namespace pstsdk { class pst; }

extern std::wstring edrm_tag_data_type(const boost::any &value);
extern std::wstring edrm_tag_value(const boost::any &value);
extern void convert_to_edrm(std::shared_ptr<pstsdk::pst> pst_file,
                            std::ostream &loadfile,
                            const boost::filesystem::path &output_directory);

#endif // EDRM_H
