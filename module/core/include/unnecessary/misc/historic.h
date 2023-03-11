#ifndef UNNECESSARYENGINE_HISTORIC_H
#define UNNECESSARYENGINE_HISTORIC_H

namespace un {
    template<class t_value>
    class historic {
    public:
        using value_type = t_value;
    protected:
        value_type _current, _last;

    public:
        historic(value_type current, value_type last) : _current(current), _last(last) {
        }

        historic() : _current(), _last() {
        }

        void set(value_type value) {
            _last = _current;
            _current = value;
        }

        value_type peek() const {
            return _current;
        }
    };

    class boolean_historic : public historic<bool> {
    public:
        boolean_historic();

        bool was_just_activated() const;

        bool was_just_deactivated() const;
    };
}
#endif