#ifndef HNBUFFER_H
#define HNBUFFER_H

#include <Arduino.h>

#define QUEUE_SIZE 32

class HNBuffer {
private:
    volatile uint8_t data[QUEUE_SIZE];
    volatile uint8_t front;
    volatile uint8_t rear;
    volatile uint8_t count;

public:
    HNBuffer();
    bool isFull();
    bool isEmpty();
    bool enqueue(uint8_t value);
    bool dequeue(uint8_t &value);
};

#endif // HNBUFFER