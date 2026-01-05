//author: Simone Piccinini


//used to have a controlled access to a vector, specifically used for indexing of "varchi"

#include <stdexcept>

template<typename T>
class ArrayView {
private:
    const T* data_;
    int size_;

public:
    ArrayView() : data_(nullptr), size_(0) {} // default
    ArrayView(const T* data, int size) : data_(data), size_(size) {}

    const T& operator[](int i) const {
        if (i < 0 || i >= size_)
            throw std::out_of_range("ArrayView::operator[]");
        return data_[i];
    }

    int size() const { return size_; }
    const T* data() const { return data_; }
};
