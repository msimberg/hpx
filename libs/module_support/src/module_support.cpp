// TODO: License header
#include <hpx/module_support.hpp>

namespace hpx { namespace module_support { namespace detail {
            std::vector<module_config_entry>& get_module_config_entries_ref() {
                static std::vector<module_config_entry> config_entries;
                return config_entries;
            }

            std::vector<module_config_entry> get_module_config_entries() {
                return get_module_config_entries_ref();
            }

            void add_module_config_entry(module_config_entry const& entry)
            {
                get_module_config_entries_ref().push_back(entry);
                std::cout << "module registry now contains " << get_module_config_entries_ref().size() << " entries" << std::endl;
            }
}}}
