#ifndef UNNECESSARYENGINE_SHADER_INPUT_H
#define UNNECESSARYENGINE_SHADER_INPUT_H

#include <string>
#include <vector>

namespace un {

    class RichInput {
    private:
        std::string name;
        std::string type;
    public:
        RichInput(const std::string& name, const std::string& type);

        const std::string& getName() const;

        const std::string& getType() const;
    };

    class InputPack {
    private:
        std::vector<RichInput> inputs;
    public:
        void operator+=(RichInput&& other);

        const std::vector<RichInput>& getInputs() const;

        std::vector<RichInput>& getInputs();
    };

    class InputUsage {
    private:
        std::string name;
        /**
         * Either in or out
         */
        std::string modifier;
    public:
        InputUsage(const std::string& name, const std::string& modifier);

        const std::string& getName() const;

        const std::string& getModifier() const;
    };
}
#endif
