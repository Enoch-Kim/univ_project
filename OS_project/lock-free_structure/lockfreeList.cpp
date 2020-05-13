#include <string>
#include <atomic>
#include <thread>
#include <iostream>
#include <time.h>
#include <cinttypes>


class LockFreeList
{
private:

    struct Node
    {
        int value;
        Node* next;
        Node( int const& data ) : value(data)
        {}
    };
    union HeadNode {
        struct {
            uint64_t  counter : 12,  
                nodePtr:52;
        };
        std::atomic<uint64_t>  headPtr; // for CAS
        
        HeadNode(HeadNode& r){
            headPtr.store(r.headPtr.load());
        }

        HeadNode() {
            headPtr.store(uint64_t(nullptr));
        }

        Node* getNode(){
            return (Node*)nodePtr;
        }
        
        HeadNode setNode(Node* p){
            counter++;
            nodePtr = (uint64_t)p;
            return *this;
        }
    };

    HeadNode head;

public:
    ~LockFreeList() {
        while(head.getNode() != nullptr){
            Node* oldHead = pop();
            delete oldHead;
        }
    }

    void push( int const & data )
    {
        Node* const newNode = new Node(data) ;
        if(head.getNode()) newNode -> next = head.getNode();
        head.setNode(newNode);
    }

    void push(Node* newNode) {
        if(newNode == nullptr) return;
        while (true) { 
            HeadNode newHead(head);
            uint64_t oldHeadPtr = newHead.headPtr.load();
            newNode->next = newHead.getNode();
            newHead.setNode(newNode);
            if(head.headPtr.compare_exchange_strong(oldHeadPtr, newHead.headPtr.load()))
                break;
        }
    }
    Node* pop() {
        if(head.getNode() == nullptr) return nullptr;
        while (true) { 
            HeadNode oldHead(head), newHead(oldHead);
            uint64_t oldHeadPtr = oldHead.headPtr.load();
            newHead.setNode(newHead.getNode() -> next);
            if(head.headPtr.compare_exchange_strong(oldHeadPtr, newHead.headPtr.load()))
                return oldHead.getNode();
            // race, try again
        }
    }
    
};



int main(){
    std::clock_t start, end;
    double cpu_time_used_sec;
    
    LockFreeList Consumer, Producer;
    for(int i=0; i<1000; ++i)
        Producer.push(i);

    auto producer_consumer = [&Consumer, &Producer](){
        int i=0;
        while(++i < 100000){
            // for(int i=0; i<100; i++)
                Consumer.push(Producer.pop());

            for(int i=0; i<200; i++)    
                Producer.push(Consumer.pop());
        }
    };

    start = std::clock();
    std::thread t1(producer_consumer);
    std::thread t2(producer_consumer);
    producer_consumer();

    t1.join();
    t2.join();

    auto node =Consumer.pop();
    while(node){
        std::cout << node->value << '\n';
        node = node -> next;
    }
    node = Producer.pop();
    while(node){
        std::cout << node->value << '\n';
        node = node -> next;
    }

    end = std::clock();
    cpu_time_used_sec = ((double) (end - start)) / CLOCKS_PER_SEC;
    int cpu_time_used_ms = (int)(cpu_time_used_sec * 1000);
    
    // std::cout << _val << '\n';
    std::cout << "수행하는데 " << cpu_time_used_ms << "ms 가 들었습니다." << '\n';
    return 0;
}

