/*
 * Created by Weston Wallace on 12/2/2021
 * Version 1.0
 */

#ifndef MATRIXL_H
#define MATRIXL_H
#include <vector>
#include <initializer_list>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <future>

template <typename T> class MatrixL;
template <typename T> std::ostream& operator<<(
        std::ostream&, const MatrixL<T>&);

template<typename T>
class MatrixL {
public:
    MatrixL();
    ~MatrixL();
    MatrixL(const MatrixL<T>& that);
    explicit MatrixL(const std::vector<size_t>& shape, const T* vals = nullptr, size_t n = 0);
    MatrixL(const std::initializer_list<size_t>& shape, const T* vals = nullptr, size_t n = 0);
    explicit MatrixL(const std::vector<size_t>& shape, T* start, const T* end);
    MatrixL(const std::initializer_list<size_t>& shape, T* start, const T* end);
    explicit MatrixL(const T& val);
    MatrixL<T>& operator=(const MatrixL<T>& that);
    MatrixL<T>& operator=(const std::initializer_list<T>& vals);
    MatrixL<T>& operator=(const T& val);

    const std::vector<size_t>& shape() const { return shape_; }
    int dimensions() const { return dimensions_; }
    size_t length() const { return capacity; }

    MatrixL<T>& operator[](std::initializer_list<size_t> indices);
    MatrixL<T>& operator[](std::initializer_list<size_t> indices) const;
    MatrixL<T>& operator[](size_t index);
    MatrixL<T>& operator[](size_t index) const;
    MatrixL<T>& at(std::initializer_list<size_t> indices);
    MatrixL<T>& at(std::initializer_list<size_t> indices) const;
    MatrixL<T>& at(size_t index);
    MatrixL<T>& at(size_t index) const;
    T& operator*();
    T& val();
    MatrixL<T>& reshape(const std::vector<size_t>& shape) const;
    MatrixL<T>& reshape_in_place(const std::vector<size_t> &shape);
    MatrixL<T>& flat() const;
    T& flatAt(size_t index);
    MatrixL<T>& transpose() const;

    MatrixL<T>& operator+(const T& addend) const;
    MatrixL<T>& operator+(const MatrixL<T>& addends) const;
    MatrixL<T>& operator+=(const T& addend);
    MatrixL<T>& operator+=(const MatrixL<T>& addends);
    MatrixL<T>& operator-(const T& subtrahend) const;
    MatrixL<T>& operator-(const MatrixL<T>& subtrahend) const;
    MatrixL<T>& operator-=(const T& subtrahend);
    MatrixL<T>& operator-=(const MatrixL<T>& subtrahend);
    MatrixL<T>& operator*(const T& multiplicand) const;
    MatrixL<T>& operator*(const MatrixL<T>& multiplicand) const;
    MatrixL<T>& operator*=(const T& multiplicand);
    MatrixL<T>& operator*=(const MatrixL<T>& multiplicand);
    MatrixL<T>& operator/(const T& divisor) const;
    MatrixL<T>& operator/(const MatrixL<T>& divisors) const;
    MatrixL<T>& operator/=(const T& divisor);
    MatrixL<T>& operator/=(const MatrixL<T>& divisors);
    MatrixL<T>& operator-() const;
    T& sum() const;
    T& product() const;
    MatrixL<T>& reg_matmul(const MatrixL<T>& that) const;

    bool operator==(const MatrixL<T>& that);
    bool operator!=(const MatrixL<T>& that);

    static MatrixL<T>& arange(size_t n);

    friend std::ostream & operator<< <> (std::ostream &, const MatrixL &);
private:
    static void mixed_radix_perfect_shuffle(T* start, const std::vector<size_t>& shape);
    static MatrixL<T>& mixed_radix_perfect_shuffle(const MatrixL<T> &);
    static int middle_factor(int n);
    void update_offsets_and_capacity();

    T *values;
    std::vector<size_t> shape_, offsets;
    int dimensions_;
    size_t capacity;
};

template<typename T>
MatrixL<T>::MatrixL() {
    values = nullptr;
    shape_ = std::vector<size_t>(1, 0);
    offsets = std::vector<size_t>(1, 0);
    dimensions_ = -1;
    capacity = 0;
}

template<typename T>
MatrixL<T>::~MatrixL() {
    delete[] values;
}

template<typename T>
MatrixL<T>::MatrixL(const MatrixL<T> &that) {
    shape_ = that.shape_;
    offsets = that.offsets;
    dimensions_ = that.dimensions_;
    capacity = that.capacity;
    values = new T[capacity];
    std::vector<std::thread> threads;
    threads.reserve(12);
    for (int i = 0; i < 12; i++) {
        threads.emplace_back(std::thread([this, &that](int index) {
            int n_elements = this->capacity / 12;
            int offset = index * n_elements;
            n_elements += (index == 11) * (this->capacity % 12);
            for (int j = offset; j < offset + n_elements; j++) {
                this->values[j] = that.values[j];
            }
        }, i));
    }
    for (auto& t : threads) {
        t.join();
    }
}

template<typename T>
MatrixL<T>::MatrixL(const std::vector<size_t>& shape, const T* vals, size_t n) {
    if (shape.empty()) {
        shape_ = std::vector<size_t>(1, 1);
        offsets = std::vector<size_t>(1, 1);
        capacity = 1;
        dimensions_ = 0;
        values = new T[capacity];
        if (vals) {
            *values = *vals;
        }
    }
    else {
        shape_ = shape;
        dimensions_ = shape_.size();
        update_offsets_and_capacity();
        values = new T[capacity];
        if (vals) {
            for (int i = 0; i < n; i++) {
                values[i] = vals[i];
            }
        }
    }
}

template<typename T>
MatrixL<T>::MatrixL(const std::initializer_list<size_t>& shape, const T* vals, size_t n) {
    if (shape.size() == 0) {
        shape_ = std::vector<size_t>(1, 1);
        capacity = 1;
        dimensions_ = 0;
        offsets = std::vector<size_t>(1, 1);
        values = new T[capacity];
        if (vals) {
            *values = *vals;
        }
    }
    else {
        shape_ = shape;
        dimensions_ = shape_.size();
        update_offsets_and_capacity();
        values = new T[capacity];
        if (vals) {
            for (int i = 0; i < n; i++) {
                values[i] = vals[i];
            }
        }
    }
}

template<typename T>
MatrixL<T>::MatrixL(const std::vector<size_t>& shape, T* start, const T* end) {
    shape_ = shape;
    dimensions_ = shape_.size();
    update_offsets_and_capacity();
    values = new T[capacity];
    for (int i = 0; i < capacity && start != end; i++) {
        values[i] = *start;
        start++;
    }
}

template<typename T>
MatrixL<T>::MatrixL(const std::initializer_list<size_t>& shape, T* start, const T* end) {
    shape_ = shape;
    dimensions_ = shape_.size();
    update_offsets_and_capacity();
    values = new T[capacity];
    for (int i = 0; i < capacity && start != end; i++) {
        values[i] = *start;
        start++;
    }
}

template<typename T>
MatrixL<T>::MatrixL(const T& val) {
    shape_ = std::vector<size_t>(1, 1);
    capacity = 1;
    dimensions_ = 0;
    offsets = std::vector<size_t>(1, 1);
    values = new T[capacity];
    values[0] = val;
}

template<typename T>
MatrixL<T> &MatrixL<T>::operator=(const MatrixL<T>& that) {
    if (this != &that) {
        delete[] values;
        shape_ = that.shape_;
        offsets = that.offsets;
        dimensions_ = that.dimensions_;
        capacity = that.capacity;
        values = new T[capacity];
        for (int i = 0; i < capacity; i++) {
            values[i] = that.values[i];
        }
    }
    return *this;
}

template<typename T>
MatrixL<T> &MatrixL<T>::operator=(const std::initializer_list<T>& vals) {
    if (capacity == 0) {
        throw std::logic_error("From operator=: Can't assign values to an uninitialized matrixL");
    }
    if (vals.size() <= capacity) {
        int i = 0;
        for (const auto& item: vals) {
            values[i] = item;
            i++;
        }
    }
    else {
        throw std::length_error("From operator=: Too many values to assign");
    }
    return *this;
}

template<typename T>
MatrixL<T> &MatrixL<T>::operator=(const T& val) {
    if (capacity == 0) {
        throw std::logic_error("From operator=: Can't assign values to an uninitialized matrixL");
    }
    *values = val;
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator[](std::initializer_list<size_t> indices) {
    if (indices.size() > dimensions_) {
        std::stringstream ss;
        ss << "From operator[]: " << indices.size() << " is too many indices_ for operator[].";
        throw std::range_error(ss.str());
    }
    std::vector<size_t> indices_ = indices;
    for (int i = 0; i < indices_.size(); i++) {
        if (indices_[i] >= shape_[i]) {
            std::stringstream ss;
            ss << "From operator[]: Index " << indices_[i] << " out of range for length " << shape_[i] << std::endl;
            throw std::range_error(ss.str());
        }
    }
    int offset = 0;
    for (int i = 0; i < indices_.size() - 1; i++) {
        offset += indices_[i] * offsets[i + 1];
    }
    if (indices_.size() == dimensions_) {
        offset += indices_[indices_.size() - 1];
    }
    else {
        offset += indices_[indices_.size() - 1] * offsets[offsets.size() - 1];
    }

    auto* sub_mat = new MatrixL<T>(std::vector<size_t>(shape_.begin() + indices_.size(), shape_.end()));
    sub_mat->values = values + offset;

    return *sub_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator[](std::initializer_list<size_t> indices) const {
    if (indices.size() > dimensions_) {
        std::stringstream ss;
        ss << "From operator[]: " << indices.size() << " is too many indices_ for operator[].";
        throw std::range_error(ss.str());
    }
    std::vector<size_t> indices_ = indices;
    for (int i = 0; i < indices_.size(); i++) {
        if (indices_[i] >= shape_[i]) {
            std::stringstream ss;
            ss << "From operator[]: Index " << indices_[i] << " out of range for length " << shape_[i] << std::endl;
            throw std::range_error(ss.str());
        }
    }
    int offset = 0;
    for (int i = 0; i < indices_.size() - 1; i++) {
        offset += indices_[i] * offsets[i + 1];
    }
    if (indices_.size() == dimensions_) {
        offset += indices_[indices_.size() - 1];
    }
    else {
        offset += indices_[indices_.size() - 1] * offsets[offsets.size() - 1];
    }

    auto* sub_mat = new MatrixL<T>(std::vector<size_t>(shape_.begin() + indices_.size(), shape_.end()));
    for (int i = 0; i < sub_mat->capacity; i++) {
        sub_mat->values[i] = values[i + offset];
    }

    return *sub_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator[](size_t index) {
    return (*this)[{index}];
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator[](size_t index) const {
    return (*this)[{index}];
}

template <typename T>
MatrixL<T>& MatrixL<T>::at(std::initializer_list<size_t> indices) {
    return (*this)[indices];
}

template <typename T>
MatrixL<T>& MatrixL<T>::at(std::initializer_list<size_t> indices) const {
    return (*this)[indices];
}

template <typename T>
MatrixL<T>& MatrixL<T>::at(size_t index) {
return this->at({index});
}

template <typename T>
MatrixL<T>& MatrixL<T>::at(size_t index) const {
    return this->at({index});
}

template<typename T>
T& MatrixL<T>::operator*() {
    return *values;
}

template<typename T>
T& MatrixL<T>::val() {
    return *values;
}

template<typename T>
MatrixL<T>& MatrixL<T>::reshape(const std::vector<size_t> &shape) const {
    auto* reshaped_mat = new MatrixL<T>(*this);
    reshaped_mat->shape_ = shape;
    reshaped_mat->dimensions_ = shape_.size();
    reshaped_mat->update_offsets_and_capacity();
    return *reshaped_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::reshape_in_place(const std::vector<size_t> &shape) {
    shape_ = shape;
    dimensions_ = shape_.size();
    update_offsets_and_capacity();
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::flat() const {
    return reshape({capacity});
}

template<typename T>
T& MatrixL<T>::flatAt(size_t index) {
    if (index > capacity) {
        throw std::length_error("index out of bounds");
    }
    return values[index];
}

template<typename T>
MatrixL<T>& MatrixL<T>::transpose() const {
    MatrixL<T> *transposed_mat;
    if (dimensions_ > 1) {
        if (dimensions_ == 2) {
            transposed_mat = &mixed_radix_perfect_shuffle(*this);
        }
        else {
            std::vector<size_t> new_shape(shape_.begin() + 2, shape_.end());
            new_shape.insert(new_shape.begin(), shape_[0] * shape_[1]);
            transposed_mat = &this->reshape(new_shape).transpose();
            new_shape = transposed_mat->shape_;
            new_shape.pop_back();
            new_shape.push_back(this->shape_[0]);
            new_shape.push_back(this->shape_[1]);
            transposed_mat->reshape_in_place(new_shape);
            int amount_mats = 1;
            for (int i = 0; i < transposed_mat->shape_.size() - 2; i++) {
                amount_mats *= transposed_mat->shape_[i];
            }
            for (int i = 0; i < amount_mats; i++) {
                mixed_radix_perfect_shuffle(transposed_mat->values + (i * transposed_mat->offsets[offsets.size() - 2]),
                                            {this->shape_[0], this->shape_[1]});
            }
            new_shape.pop_back();
            new_shape.pop_back();
            new_shape.push_back(this->shape_[1]);
            new_shape.push_back(this->shape_[0]);
            transposed_mat->reshape_in_place(new_shape);
        }
    }
    else {
        transposed_mat = new MatrixL<T>(*this);
    }
    return *transposed_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator+(const T& addend) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat += addend;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator+(const MatrixL<T>& addends) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat += addends;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator+=(const T& addend) {
    for (int i = 0; i < capacity; i++) {
        this->values[i] += addend;
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator+=(const MatrixL<T>& addends) {
    bool compatible_shape = true;
    for (int i = addends.shape_.size() - 1; i >= 0 && compatible_shape; i--) {
        compatible_shape = addends.shape_[i] == this->shape_[i];
    }
    if (!compatible_shape) {
        throw std::length_error("Shape not compatible for addition");
    }
    for (int i = 0; i < this->capacity / addends.capacity; i++) {
        for (int j = 0; j < addends.capacity; j++) {
            this->values[i * addends.capacity + j] += addends.values[j];
        }
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator-(const T& subtrahend) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat -= subtrahend;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator-(const MatrixL<T>& subtrahends) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat -= subtrahends;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator-=(const T& subtrahend) {
    for (int i = 0; i < capacity; i++) {
        this->values[i] -= subtrahend;
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator-=(const MatrixL<T>& subtrahends) {
    bool compatible_shape = true;
    for (int i = subtrahends.shape_.size() - 1; i >= 0 && compatible_shape; i--) {
        compatible_shape = subtrahends.shape_[i] == this->shape_[i];
    }
    if (!compatible_shape) {
        throw std::length_error("Shape not compatible for subtraction");
    }
    for (int i = 0; i < this->capacity / subtrahends.capacity; i++) {
        for (int j = 0; j < subtrahends.capacity; j++) {
            this->values[i * subtrahends.capacity + j] -= subtrahends.values[j];
        }
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator*(const T& multiplicand) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat *= multiplicand;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator*(const MatrixL<T>& multiplicands) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat *= multiplicands;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator*=(const T& multiplicand) {
    for (int i = 0; i < capacity; i++) {
        this->values[i] *= multiplicand;
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator*=(const MatrixL<T>& multiplicands) {
    bool compatible_shape = true;
    for (int i = multiplicands.shape_.size() - 1; i >= 0 && compatible_shape; i--) {
        compatible_shape = multiplicands.shape_[i] == this->shape_[i];
    }
    if (!compatible_shape) {
        throw std::length_error("Shape not compatible for multiplication");
    }
    for (int i = 0; i < this->capacity / multiplicands.capacity; i++) {
        for (int j = 0; j < multiplicands.capacity; j++) {
            this->values[i * multiplicands.capacity + j] *= multiplicands.values[j];
        }
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator/(const T& divisor) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat /= divisor;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator/(const MatrixL<T>& divisors) const {
    auto* ret_mat = new MatrixL<T>(*this);
    *ret_mat /= divisors;
    return *ret_mat;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator/=(const T& divisor) {
    for (int i = 0; i < capacity; i++) {
        this->values[i] /= divisor;
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator/=(const MatrixL<T>& divisors) {
    bool compatible_shape = true;
    for (int i = divisors.shape_.size() - 1; i >= 0 && compatible_shape; i--) {
        compatible_shape = divisors.shape_[i] == this->shape_[i];
    }
    if (!compatible_shape) {
        throw std::length_error("Shape not compatible for division");
    }
    for (int i = 0; i < this->capacity / divisors.capacity; i++) {
        for (int j = 0; j < divisors.capacity; j++) {
            this->values[i * divisors.capacity + j] /= divisors.values[j];
        }
    }
    return *this;
}

template<typename T>
MatrixL<T>& MatrixL<T>::operator-() const {
    auto* ret_mat = new MatrixL<T>(*this);
    for (int i = 0; i < ret_mat->capacity; i++) {
        ret_mat->values[i] = -ret_mat->values[i];
    }
    return *ret_mat;
}

template<typename T>
T& MatrixL<T>::sum() const {
    T* sum = new T;
    *sum = this->values[0];
    for (int i = 1; i < this->capacity; i++) {
        *sum += this->values[i];
    }
    return *sum;
}

template<typename T>
T& MatrixL<T>::product() const {
    T* product = new T;
    *product = this->values[0];
    for (int i = 1; i < this->capacity; i++) {
        *product *= this->values[i];
    }
    return *product;
}

template<typename T>
MatrixL<T>& MatrixL<T>::reg_matmul(const MatrixL<T>& that) const {
    if (this->dimensions_ != 2 || that.dimensions_ != 2) {
        throw std::length_error("Cannot matmul matrices without dimensions equal to 2");
    }
    if (this->shape_[1] != that.shape_[0]) {
        throw std::length_error("Cannot matmul matrices where lengths do not follow standard matmul rules");
    }
    auto* ret_mat = new MatrixL<T>({this->shape_[0], that.shape_[1]});
    T sum;
    for (size_t i = 0; i < this->shape_[0]; i++) {
        for (size_t j = 0; j < that.shape_[1]; j++) {
            sum = 0;
            for (size_t k = 0; k < this->shape_[1]; k++) {
                sum = sum + this->at({i, k}).values[0] * that.at({k, j}).values[0];
            }
            ret_mat->at({i, j}) = sum;
        }
    }
    return *ret_mat;
}

template<typename T>
bool MatrixL<T>::operator==(const MatrixL<T> &that) {
    bool equals = false;
    if (values) {
        for (int i = 0; i < capacity && !equals; i++) {
            equals = values[i] == that.values[i];
        }
    }
    else {
        equals = !that.values;
    }
    return equals;
}

template<typename T>
bool MatrixL<T>::operator!=(const MatrixL<T> &that) {
    bool not_equals = false;
    if (values) {
        for (int i = 0; i < capacity && !not_equals; i++) {
            not_equals = values[i] != that.values[i];
        }
    }
    else {
        not_equals = that.values;
    }
    return not_equals;
}

template<typename T>
MatrixL<T> &MatrixL<T>::arange(size_t n) {
    auto* ret_matrixL = new MatrixL<T>({n});
    for (int i = 0; i < n; i++) {
        ret_matrixL->values[i] = i + 1;
    }
    return *ret_matrixL;
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const MatrixL<T> &mat) {
    if (mat.values) {
        for (int i = 0; i < mat.capacity; i++) {
            for (int j = mat.dimensions_ - 1; j >= 0 && (!(i % mat.offsets[j]) || !i); j--) {
                out << '[';
            }
            out << mat.values[i];
            for (int j = mat.dimensions_ - 1; j >= 0 && (i && !((i + 1) % mat.offsets[j])); j--) {
                out << ']';
            }
            if (i - mat.capacity + 1) {
                out << ", ";
            }
        }
    }
    else {
        out << "NULL";
    }
    return out;
}

template<typename T>
void MatrixL<T>::update_offsets_and_capacity() {
    dimensions_ = shape_.size();
    offsets = std::vector<size_t>(dimensions_);
    offsets[dimensions_ - 1] = shape_[dimensions_ - 1];
    for (int i = dimensions_ - 2; i >= 0; i--) {
        offsets[i] = offsets[i + 1] * shape_[i];
    }
    if (capacity > offsets[0]) {
        T *new_vals = new T[offsets[0]];
        for (int i = 0; i < capacity; i++) {
            new_vals[i] = values[i];
        }
        delete[] values;
    }
    capacity = offsets[0];
}

template<typename T>
MatrixL<T>& MatrixL<T>::mixed_radix_perfect_shuffle(const MatrixL<T>& mat) {
    auto* vals = new T[mat.capacity];
    std::copy(mat.values, mat.values + mat.capacity, vals);
    std::vector<size_t> shape_reversed = mat.shape_;
    std::reverse(shape_reversed.begin(),  shape_reversed.end());
    auto* shuffled_mat = new MatrixL<T>(
            mat.reshape(shape_reversed));
    size_t mf = middle_factor(mat.shape_[0]);
    int offset = mat.shape_[1];
    size_t factors[] = {mf, mat.shape_[0] / mf};
    std::vector<int> entry_points;
    for (int twice = 0; twice < 2 && factors[twice] != 1; twice++) {
        entry_points.reserve(factors[twice]);
        for (int i = 0; i < factors[twice]; i++) {
            entry_points.push_back(i * factors[(twice + 1) % 2] * offset);
        }
        for (int i = 0; i < mat.capacity / factors[twice]; i++) {
            for (int j = 0; j < factors[twice]; j++) {
                shuffled_mat->values[(i * factors[twice]) + j] = vals[entry_points[j]];
                entry_points[j]++;
            }
        }
        std::copy(shuffled_mat->values, shuffled_mat->values + mat.capacity, vals);
        entry_points.clear();
    }
    delete[] vals;
    return *shuffled_mat;
}

template<typename T>
void MatrixL<T>::mixed_radix_perfect_shuffle(T* start, const std::vector<size_t>& shape) {
    size_t capacity = shape[0] * shape[1];
    auto* vals = new T[capacity];
    std::copy(start, start + capacity, vals);
    std::vector<size_t> shape_reversed = shape;
    std::reverse(shape_reversed.begin(),  shape_reversed.end());
    size_t mf = middle_factor(shape[0]);
    size_t offset = shape[1];
    size_t factors[] = {mf, shape[0] / mf};
    std::vector<size_t> entry_points;
    for (size_t twice = 0; twice < 2 && factors[twice] != 1; twice++) {
        entry_points.reserve(factors[twice]);
        for (size_t i = 0; i < factors[twice]; i++) {
            entry_points.push_back(i * factors[(twice + 1) % 2] * offset);
        }
        for (size_t i = 0; i < capacity / factors[twice]; i++) {
            for (size_t j = 0; j < factors[twice]; j++) {
                start[(i * factors[twice]) + j] = vals[entry_points[j]];
                entry_points[j]++;
            }
        }
        std::copy(start, start + capacity, vals);
        entry_points.clear();
    }
    delete[] vals;
}

template<typename T>
int MatrixL<T>::middle_factor(int n) {
    int factor = n;
    int sqrt_n = std::floor(std::sqrt(n));
    while (factor % 2 == 0 && factor > sqrt_n) {
        factor /= 2;
    }
    for (int odd = 3; odd <= sqrt_n; odd += 2) {
        while (factor % odd == 0 && factor > sqrt_n) {
            factor /= odd;
        }
    }
    return n == 2 ? 2 : factor;
}


#endif //MATRIXL_H
