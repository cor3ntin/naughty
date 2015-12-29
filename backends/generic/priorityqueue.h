#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <QQueue>

template<class Priority, class T>
class PriorityQueue
{
public:
    void enqueue(Priority priority,T value) {
        Item<T> item(priority,value);
        for(int i = 0 ; i < queue.count() ; ++i ) {
            const Item<T>& otherItem = queue[i];
            if( priority > otherItem.priority )  {
                queue.insert(i,item);
                return;
            }
        }
        queue.append(item);
    }

    T dequeue() {
        const Item<T>& item = queue.dequeue();
        return item.value;
    }

    int count() {
        return queue.count();
    }

private:

    template<class C>
    struct Item
    {
        Priority priority;
        C value;

        Item(Priority priority, C value) {
            priority = priority;
            value = value;
        }
    };

    QQueue< Item<T > > queue;

};

#endif // PRIORITYQUEUE_H
