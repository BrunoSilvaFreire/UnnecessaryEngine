#include <unnecessary/misc/historic.h>

namespace un {
    bool boolean_historic::was_just_deactivated() const {
        return _last && !_current;
    }

    bool boolean_historic::was_just_activated() const {
        return !_last && _current;
    }

    boolean_historic::boolean_historic() : historic<bool>(false, false) {
    }
}
