// TODO: License header
#include <string>
#include <vector>
#include <iostream>

namespace hpx { namespace module_support { namespace detail {
            struct module_config_entry {
                std::string module_name;
                std::vector<std::string> config_strings;
            };

            std::vector<module_config_entry> get_module_config_entries();
            void add_module_config_entry(module_config_entry const& entry);

            struct register_module_config_entry {
                register_module_config_entry(module_config_entry const& entry) {
                    std::cout << "registering module " << entry.module_name << std::endl;
                    add_module_config_entry(entry);
                }
            };
}}}
