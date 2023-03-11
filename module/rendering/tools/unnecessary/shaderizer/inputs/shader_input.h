#ifndef UNNECESSARYENGINE_SHADER_INPUT_H
#define UNNECESSARYENGINE_SHADER_INPUT_H

#include <string>
#include <vector>

namespace un {
    class rich_input {
    private:
        std::string _name;
        std::string _type;

    public:
        rich_input(const std::string& name, const std::string& type);

        const std::string& get_name() const;

        const std::string& get_type() const;
    };

    class input_pack {
    private:
        std::vector<rich_input> _inputs;

    public:
        void operator+=(rich_input&& other);

        const std::vector<rich_input>& get_inputs() const;

        std::vector<rich_input>& get_inputs();
    };

    class input_usage {
    private:
        std::string _name;
        /**
         * Either in or out
         */
        std::string _modifier;

    public:
        input_usage(const std::string& name, const std::string& modifier);

        const std::string& get_name() const;

        const std::string& get_modifier() const;
    };
}
#endif
