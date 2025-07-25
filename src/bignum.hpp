#ifndef __BIGNUM_H
#define __BIGNUM_H

#include <format>
#include <cassert>
#include <stdexcept>
#include <cstring>


// The ending determiner could be a 0 unfollowed by another number, and the storage as little endian, as the
// leading zero in 01234 (represented as big endian cause I'm a human) doesn't make any sense normally
// ^ This doesn't work because there's a randum unallocated number after the 0 end marker

// Internally stored little endian
class BigNum {
    friend struct std::formatter<BigNum>;

    public:
        // Technically could reduce marginally on size by making this a char, since the terminating 0 would only be a byte instead of 8 bytes,
        // but then conversion in the constructor would be a lot harder because you have to convert generic number T to a variable amount of chars
        // Remember to change this back to ULL cause without it a BigNum char with size 4 is just an int with really expensive arithmetic
        // typedef unsigned long long DataType;
        typedef unsigned char DataType;
        constexpr static DataType LARGEST_DIGIT = ~((DataType) 0);

    private:
        size_t size;
        DataType *data;


        /**
        * Increases size of data on the heap by 1 (effectively increasing the number of available numbers by a factor of 2^sizeof(DataType))
        * Returns a pointer to the start of uninitialized memory (the end of the array)
        */
        DataType *increase_capacity() {
            data = (DataType *) realloc(data, ++size);
            assert(data && "bignum memory expansion error");
            return data + size - 1;
        }

        /**
        * Increases size of data on the heap by `amount` (effectively increasing the number of available numbers by a factor of 2^sizeof(DataType))
        * Returns a pointer to the start of uninitialized memory
        */
        DataType *increase_capacity(size_t amount) {
            size += amount;
            data = (DataType *) realloc(data, size);
            assert(data && "bignum memory expansion error");
            return data + size - amount;
        }
        
        /**
        * Decreases size of data on the heap by 1
        */
        void decrease_capacity() {
            data = (DataType *) realloc(data, --size);
            assert(data && "bignum memory reduction error");
        }
        
        /**
        * Decreases size of data on the heap by `amount`
        */
        void decrease_capacity(size_t amount) {
            size -= amount;
            data = (DataType *) realloc(data, size);
            assert(data && "bignum memory reduction error");
        }


        /**
        * Given the index `i`, this will assume carryover initially and carry out what is effectively assigning the rest of `bignum` over to this
        * `i` will be changed to reflect how far it got in the list
        * `bignum` will not be changed
        */ 
        void carry_over_rest(const BigNum &bignum, int &i) {
            bool carryover = true;

            for (; i < bignum.size; i++) {
                DataType result = bignum.data[i] + carryover;
                carryover = result < bignum.data[i];
                data[i] = result;
                if (!carryover) break; 
            }

            if (carryover) {
                *increase_capacity() = (DataType) 1; // This is funky
            }
        }


        /**
        * Given the index `i`, this will assume carryunder initially and remove all useless prefixed 0s
        * `i` will be changed to reflect how far it got in the list
        */ 
        void carry_down_rest(int &i) {
            int start = i;
            bool carryunder = true;

            for (; i < size; i++) {
                DataType result = data[i] - carryunder;
                carryunder = result >= data[i];
                data[i] = result;
                if (!carryunder) break; 
            }
        
            if (carryunder) {
                throw std::underflow_error("BigNum subtraction error: rhs was too big");
            }
        }

        void remove_extraneous_zeroes() {
            int zero_index = -1;
            for (int i = size - 1; i > 0; i--) {
                if (data[i] == 0) {
                    zero_index = i;
                } else break;
            }

            if (zero_index >= 0) {
                decrease_capacity(size - zero_index);
            }
        }

    public:
        unsigned long long get_head_as_ullong() const {
            // Only care about the allocated bits
            unsigned long long real_mask = 0;
            DataType *real_mask_pp = (DataType *) &real_mask;
            for (int i = 0; i < size && i < sizeof(real_mask) / sizeof(DataType); i++) {
                *(real_mask_pp + i) = LARGEST_DIGIT;
            }

            return *(unsigned long long *) data & real_mask;
        }

        template <typename T>
        BigNum(T arr[], size_t size) : size(size) {
            data = (DataType *) malloc(size * sizeof(DataType));
            std::reverse_copy(arr, arr + size, data);
        }

        template <typename T, std::size_t size>
        BigNum(std::array<T, size> arr) : BigNum(arr.data(), size) {}

        template <typename N>
        BigNum(N num) : size(sizeof(unsigned long long) / sizeof(DataType)) {
            // Convert and then interpret those bytes as if they were an array of DataType
            unsigned long long converted_num = num;
            DataType *arr = (DataType *) &converted_num;

            data = (DataType *) malloc(size * sizeof(DataType));
            memcpy(data, arr, size * sizeof(DataType));
            remove_extraneous_zeroes();
        }

        BigNum(const BigNum &bignum) : size(bignum.size) {
            data = (DataType *) malloc(size * sizeof(DataType));
            std::copy(bignum.data, bignum.data + size, data);
        }

        void reset() {
            decrease_capacity(size - 1);
            *data = 0;
        }

        ~BigNum() {
            free(data);
        }

        // -- Addition --

        BigNum& operator+=(const BigNum &other) {
            // if (size == other.size) then biggest and smallest will both equal &other
            const BigNum *biggest = size > other.size ? this : &other;
            const BigNum *smallest = size < other.size ? this : &other;

            bool carryover = false;
            int i;
            for (i = 0; i < smallest->size; i++) {
                DataType result = biggest->data[i] + smallest->data[i] + carryover;
                carryover = result < biggest->data[i] || result < smallest->data[i];
                data[i] = result;
            }

            if (biggest != this) {
               size_t size_difference = biggest->size - size;
               std::memcpy(increase_capacity(size_difference), biggest->data + i, sizeof(DataType) * size_difference);
            }

            if (carryover) carry_over_rest(*biggest, i);

            return *this;
        }

        BigNum operator+(BigNum const& other) {
            BigNum bignum(*this);
            bignum += other;
            return bignum;
        }

        template <typename T>
            requires std::integral<T>
        BigNum& operator+=(T num) {
            DataType result = data[0] + num;
            bool carryover = result < data[0] || result < num;
            data[0] = result;

            int i = 1;
            if (carryover) carry_over_rest(*this, i);
            return *this;
        }

        template <typename T>
            requires std::integral<T>
        BigNum operator+(T num) {
            BigNum bignum(*this);
            bignum += num;
            return bignum;
        }


        BigNum operator++() {
            *this += 1;
            return *this;
        }

        BigNum operator++(int dummy) {
            BigNum temp = *this;
            *this += 1;
            return temp;
        }

        // -- Subtraction --

        BigNum& operator-=(BigNum const& other) {
            assert(size >= other.size && "BigNum subtraction error: rhs was too big");

            bool carryunder = false;
            int i;
            for (i = 0; i < other.size; i++) {
                DataType result = data[i] - other.data[i] - carryunder;
                if (result >= data[i]) { // Underflow
                    result += LARGEST_DIGIT;
                    carryunder = true;
                } else {
                    carryunder = false;
                }
                data[i] = result;
            }

            if (carryunder) carry_down_rest(i);

            remove_extraneous_zeroes();

            return *this;
        }

        BigNum operator-(BigNum const& other) {
            BigNum bignum(*this);
            bignum -= other;
            return bignum;
        }

        template <typename T>
            requires std::integral<T>
        BigNum& operator-=(T num) {
            bool carryunder = false;
            DataType result = data[0] - num - carryunder;
            if (result > data[0]) { // Underflow
                result += LARGEST_DIGIT;
                carryunder = true;
            } else {
                carryunder = false;
            }
            data[0] = result;

            int i = 1;
            if (carryunder) carry_down_rest(i);

            remove_extraneous_zeroes();

            return *this;
        }

        template <typename T>
            requires std::integral<T>
        BigNum operator -(T num) {
            BigNum bignum(*this);
            bignum -= num;
            return bignum;
        }

        BigNum operator--() {
            *this -= 1;
            return *this;
        }

        BigNum operator--(int dummy) {
            BigNum temp = *this;
            *this -= 1;
            return temp;
        }

        // -- Comparison --

        bool operator==(BigNum const& other) const {
            if (size != other.size) return false; 
            return memcmp(data, other.data, size) == 0;
        }

        template <typename N>
        bool operator==(N const& num) const {
            if (size != 1) return false;
            return get_head_as_ullong() == num;
        }

        bool operator<(BigNum const& other) const {
            if (size < other.size) return true; 
            if (size > other.size) return false;

            for (int i = size - 1; i >= 0; i--) {
                if (data[i] != other.data[i]) {
                    return data[i] < other.data[i];
                }
            }

            return false;
        }

        template <typename N>
        bool operator<(N const& num) const {
            if (size != 1) return false; 
            return get_head_as_ullong() < num;
        }

        bool operator>(BigNum const& other) const {
            if (size > other.size) return true; 
            if (size < other.size) return false;

            for (int i = size - 1; i >= 0; i--) {
                if (data[i] != other.data[i]) {
                    return data[i] > other.data[i];
                }
            }

            return false;
        }

        template <typename N>
        bool operator>(N const& num) const {
            if (size > 1) return false;
            return get_head_as_ullong() > num;
        }
};



template <>
struct std::formatter<BigNum> {
    constexpr auto parse(auto &ctx) {
        return ctx.begin();
    }

    auto format(BigNum &bignum, auto &ctx) const {
        auto out = ctx.out();
        std::format_to(out, "[");

        int i;
        for (i = bignum.size - 1; i > 0; i--) {
            std::format_to(out, "{}, ", bignum.data[i]);
        }
        std::format_to(out, "{}", bignum.data[i]);

        std::format_to(out, "]");
        return out;
    }
};

#endif