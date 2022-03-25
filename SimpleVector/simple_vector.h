#pragma once

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include<algorithm>
#include <initializer_list>
#include <iterator>
#include <utility>
#include "array_ptr.h"
using namespace std;

class ReserveProxyObj{
public:
    ReserveProxyObj(const size_t capacity_to_reserve) 
        :capacity_new_(capacity_to_reserve)
    {}

    size_t GetSize() { return capacity_new_; }

private:
    size_t capacity_new_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    static void FillWithDefaultValue(Type* from, Type* to)
    {
        while (from != to) {
            *from = Type{};
            ++from;
        }
    }

    explicit SimpleVector(size_t size)
    :size_(size),
        capacity_(size)
    {
        ArrayPtr<Type> arr_ptr(size);
        FillWithDefaultValue(&arr_ptr[0], &arr_ptr[size]);
        simple_vector.swap(arr_ptr);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)//const 
        :simple_vector(size),
        size_(size),
        capacity_(size)       
    {
        fill(simple_vector.Get(), simple_vector.Get()+size_, value);  
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
        :size_(init.size()),
        capacity_(init.size())
    {
        ArrayPtr<Type> arr_ptr(init.size());
        int count_ = 0;
        for (auto i = init.begin(); i < init.end(); ++i)
        {
            auto n = *i;
            arr_ptr[count_] = n;
            ++count_;
        }
        simple_vector.swap(arr_ptr);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_)
        {
            ArrayPtr<Type> arr_ptr(new_capacity);
            move(make_move_iterator(cbegin()), make_move_iterator(cend()), &arr_ptr[0]);
            capacity_ = new_capacity;
            simple_vector.swap(arr_ptr);
        }
        else {}
    }

    SimpleVector(ReserveProxyObj obj)
    {
        SimpleVector<Type> vec;
        vec.Reserve(obj.GetSize());
        swap(vec);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        if (size_ == 0)
        {
            return true;
        }
        else
            return false;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return simple_vector[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return simple_vector[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_)
        {
            throw std::out_of_range("AT!");
        }
        else
        {
            return simple_vector[index];
        }
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_)
        {
            throw out_of_range("AT!");
        }
        else
        {
            return simple_vector[index];
        }
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_)
        {
            size_ = new_size;
        }
        else
        {
            if (new_size <= capacity_)
            {
                for (auto it = &simple_vector[size_]; it != &simple_vector[new_size]; ++it)
                {
                    *(it) = move(Type{});
                }
                size_ = new_size;
            }
            else //> capacity
            {
                ArrayPtr<Type> arr_ptr(new_size);
               move(make_move_iterator(begin()), make_move_iterator(end()), arr_ptr.Get());
               for (auto it = &arr_ptr[size_]; it != &arr_ptr[new_size]; ++it)
                {
                    *(it) = move(Type{});
                }
                size_ = new_size;
                capacity_ = new_size;
                simple_vector.swap(arr_ptr);
            }
        }

    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return simple_vector.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return simple_vector.Get() + size_ ;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return simple_vector.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return simple_vector.Get() + size_ ;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return simple_vector.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return simple_vector.Get() + size_ ;
    }

    SimpleVector( SimpleVector&& other) 
        : simple_vector(other.size_)
         ,size_(other.size_)
        , capacity_(other.size_) 
    {
        std::move(make_move_iterator(other.begin()), make_move_iterator(other.end()), simple_vector.Get());
        other.size_ = 0;
    }

    SimpleVector(const SimpleVector& other)
        : simple_vector(other.size_)
        , size_(other.size_)
        , capacity_(other.size_) 
    {
        std::copy(other.begin(), other.end(), simple_vector.Get());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs)
        {
            SimpleVector rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }


    SimpleVector& operator=( SimpleVector&& other) {
        if (this != &other)
        {
            simple_vector = move(other.simple_vector);
            size_ = exchange(other.size_, 0);
            capacity_ = exchange(other.capacity_, 0);
        }
        return *this;
    }

   // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (capacity_ == 0)
        {
            SimpleVector<Type> new_vec(1);
            new_vec[0] = item;
            new_vec.capacity_ = 1;
            swap(new_vec);
        }
        else {
            if (size_ == capacity_)
            {
                ArrayPtr<Type> arr_ptr(2 * capacity_);
                copy(&simple_vector[0], &simple_vector[size_], &arr_ptr[0]);
                arr_ptr[size_] = item;
                capacity_ = 2 * capacity_;
                ++size_;
                simple_vector.swap(arr_ptr);
            }
            else
            {
                simple_vector[size_] = item;
                ++size_;
            }
        }
    }

    void PushBack(Type&& item) {
        if (capacity_ == 0)
        {
          SimpleVector<Type> new_vec(1);
             new_vec[0] = move(item);
            *this = move(new_vec);
        }
        else {
            if (size_ == capacity_)
            {
                SimpleVector<Type> arr_ptr(2 * capacity_);
                move(make_move_iterator(begin()), make_move_iterator(end()), arr_ptr.begin());
                arr_ptr.size_ = size_ + 1;
                arr_ptr[size_] = move(item);
                *this = move(arr_ptr);
            }
            else
            {
               simple_vector[size_] = move(item);
                ++size_;
            }
        }
    }

    
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1

    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t pos_distance = std::distance(cbegin(), pos);

        if (capacity_ == 0)
        {
            ArrayPtr<Type> arr_ptr(1);
            arr_ptr[0] = value;
            ++size_;
            ++capacity_;
            simple_vector.swap(arr_ptr);
            return &simple_vector[0];

        }

        else {
            if (size_ == capacity_)
            {
                ArrayPtr<Type> arr_ptr(capacity_ * 2);
                copy(cbegin(), pos, &arr_ptr[0]);
                arr_ptr[pos_distance] = value;
                copy_backward(pos, cend(), &arr_ptr[size_ + 1]);
                ++size_;
                capacity_ = size_;
                simple_vector.swap(arr_ptr);
                return &simple_vector[pos_distance];
            }
            else
            {
                copy_backward(pos, cend(), &simple_vector[size_ + 1]);
                simple_vector[pos_distance] = value;
                ++size_;
                return &simple_vector[pos_distance];
            }
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
       auto* p = const_cast<Iterator>(pos);
       size_t pos_distance = std::distance(begin(), p);

       if (capacity_ == 0)
       {
          ArrayPtr<Type> arr_ptr(1);
          arr_ptr[pos_distance] = std::move(value);
          ++size_;
          ++capacity_; 
          simple_vector.swap(arr_ptr);
          return &simple_vector[pos_distance]; 
       }

       else {
           if (size_ == capacity_)
           {
               SimpleVector<Type> arr_ptr(capacity_ * 2); 
               std::move(make_move_iterator(begin()), make_move_iterator(p), arr_ptr.begin());
               arr_ptr[pos_distance] = move(value);
               move_backward(make_move_iterator(p), make_move_iterator(end()), &arr_ptr[size_ + 1]);
               arr_ptr.size_ = size_ + 1; 
               swap(arr_ptr); 
               return &simple_vector[pos_distance];
           }
           else
           {
               move_backward(make_move_iterator(p), make_move_iterator(end()), &simple_vector[size_ + 1]);
               simple_vector[pos_distance] = move(value);
               ++size_;
               return &simple_vector[pos_distance];
           }
       }
    }
    
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept { 
        if (!IsEmpty())
        {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        auto* p = const_cast<Iterator>(pos);
        size_t pos_distance = std::distance(begin(), p); 
        move(make_move_iterator(p+1), make_move_iterator(end()), &simple_vector[pos_distance]);//+1
        --size_;
        return &simple_vector[pos_distance];

    }
  
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept { 
        simple_vector.swap(other.simple_vector);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_); 
    }
  
   

    private:
        ArrayPtr<Type> simple_vector;
        size_t size_ = 0;
        size_t capacity_  = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std:: equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 
    return (!(lhs == rhs));
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 
    return (std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()) || lhs==rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 
    return (!(lhs<rhs) && lhs!= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 
    return (!(lhs<rhs));
}
