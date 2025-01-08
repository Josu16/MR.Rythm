#include <Arduino.h>
#include "HNBuffer.h"

HNBuffer::HNBuffer() : front(0), rear(0), count(0) {}

bool HNBuffer::isFull() {
    return count == QUEUE_SIZE;
}

bool HNBuffer::isEmpty() {
    return count == 0;
}

bool HNBuffer::enqueue(uint8_t value) {
    if (isFull()) {
        return false; // La cola está llena
    }
    data[rear] = value;
    rear = (rear + 1) % QUEUE_SIZE;
    count++;
    return true;
}

bool HNBuffer::dequeue(uint8_t &value) {
    if (isEmpty()) {
        return false; // La cola está vacía
    }
    value = data[front];
    front = (front + 1) % QUEUE_SIZE;
    count--;
    return true;
}