#ifndef UNNECESSARYENGINE_HISTORIC_H
#define UNNECESSARYENGINE_HISTORIC_H
namespace un {
    template<class T>
    class Historic {
    protected:
        T current, last;
    public:
        Historic(T current, T last) : current(current), last(last) { }

        Historic() : current(), last() { }

        void set(T value) {
            last = current;
            current = value;
        }

        T peek() const {
            return current;
        }
    };

    class BooleanHistoric : public Historic<bool> {
    public:
        BooleanHistoric();

        bool wasJustActivated() const;

        bool wasJustDeactivated() const;
    };
}
#endif //UNNECESSARYENGINE_HISTORIC_H
