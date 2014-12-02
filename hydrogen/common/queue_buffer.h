#pragma once
#include <cstdlib>
#include <utility>
#include <cassert>
#include <stdexcept>

namespace hy {
    /* queue_buffer implements a low level queue-like data structure where queue
     * elements are stored in a C-style array. Unlike other standard containers,
     * queue_buffer doesn't hide all details and provides the ability to manipulate
     * elements in the queue directly.
     * 
     * Element SHOULD be of POD types.
     * queue_buffer uses new/delete operator to manage memory and memcpy/memmove
     * to move elements.
     */
    template<typename T>
    class queue_buffer {
    public:
        queue_buffer()
            : _buf(nullptr), _size(0), _front(0), _tail(0){}
        explicit queue_buffer(size_t size)
            : _buf(new T[size]), _size(size), _front(0), _tail(0){}
        ~queue_buffer(){ release(); }

        queue_buffer(const queue_buffer&) = delete;
        queue_buffer& operator=(queue_buffer&) = delete;

        queue_buffer(queue_buffer&& q): queue_buffer(){
            swap(q);
        }
        
        queue_buffer& operator=(queue_buffer&& q) {
            release();
            swap(q);
            return *this;
        }

        /* Resize the capacity of the buffer.
         * Existing contents in the buffer will be dropped.
         */
        void resize(size_t size){
            delete[] _buf;
            _buf = size ? new T[size] : nullptr;
            _size = size;
            _front = 0;
            _tail = 0;
        }

        void release(){ resize(0); }

        bool   empty() const { return _front == _tail; }
        size_t capacity() const { return _size; }
        size_t length() const { return _tail - _front; }
        size_t free() const { return _size - _tail; }

        T* front() { return _buf + _front; }
        T* tail() { return _buf + _tail; }

        /* Trim the queue by moving elements to the head of the buffer. */
        void trim(){
            if (_front){
                if (length()){
                    memmove(_buf, front(), length());
                }
                _tail -= _front;
                _front = 0;
            }
        }

        /* Copy count elements from queue front to dst.
         * Returns number of elements actually copied.
         */
        size_t copy(T* dst, size_t count){
            if (count > length()){
                count = length();
            }
            memcpy(_buf, front(), length());
            return count;
        }

        /* Pop count elements from queue. */
        void pop(size_t count){
            if (count + _front > _tail){
                throw std::out_of_range("hy::queue_buffer::pop() out of range.");
            }
            _front += count;
        }

        /* Push count elements to queue. */
        void push(size_t count){
            if (count + _tail > _size){
                throw std::out_of_range("hy::queue_buffer::push() out of range.");
            }
            _tail += count;
        }

        void swap(queue_buffer& buf){
            std::swap(_buf, buf._buf);
            std::swap(_size, buf._size);
            std::swap(_front, buf._front);
            std::swap(_tail, buf._tail);
        }

    private:
        /* The buffer to store queue elements */
        T*  _buf;
        /* Capacity of queue */
        size_t _size;
        /* Offset to queue front */
        size_t _front;
        /* Offset to queue tail */
        size_t _tail;
    };
}
