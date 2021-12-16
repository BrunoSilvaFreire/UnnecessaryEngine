#include <unnecessary/misc/historic.h>

namespace un {

    bool BooleanHistoric::wasJustDeactivated() const {
        return last && !current;
    }

    bool BooleanHistoric::wasJustActivated() const {
        return !last && current;
    }

    BooleanHistoric::BooleanHistoric() : Historic<bool>(false, false) {}
}