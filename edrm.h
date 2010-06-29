#ifndef EDRM_H
#define EDRM_H

#include <string>
#include <boost/utility.hpp>
#include <boost/filesystem.hpp>

#include "xml_context.h"

namespace boost { class any; }
namespace pstsdk { class pst; }

extern std::wstring edrm_tag_data_type(const boost::any &value);
extern std::wstring edrm_tag_value(const boost::any &value);

/// This class holds various information needed to generate EDRM output.
class edrm_context : boost::noncopyable {
    xml_context m_loadfile;
    boost::filesystem::path m_out_dir;
    size_t m_next_doc_id;

    struct relationship_info {
        std::wstring type;
        std::wstring parent_doc_id;
        std::wstring child_doc_id;

        relationship_info(const std::wstring &t,
                          const std::wstring &p,
                          const std::wstring &c)
            : type(t), parent_doc_id(p), child_doc_id(c) {}
    };

    std::vector<relationship_info> m_relationships;

public:
    edrm_context(std::ostream &out, const boost::filesystem::path &out_dir)
        : m_loadfile(out), m_out_dir(out_dir), m_next_doc_id(1) { }

    xml_context &loadfile() { return m_loadfile; }
    boost::filesystem::path out_dir() const { return m_out_dir; }
    std::wstring next_doc_id();

    void relationship(const std::wstring &type,
                      const std::wstring &parent_doc_id,
                      const std::wstring &child_doc_id);
    void output_relationships();
};

extern void convert_to_edrm(std::shared_ptr<pstsdk::pst> pst_file,
                            std::ostream &loadfile,
                            const boost::filesystem::path &output_directory);

#endif // EDRM_H
