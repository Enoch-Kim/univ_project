#include <string>
#include <atomic>
#include <thread>
#include <iostream>
#include <time.h>
#include <cinttypes>
#include <mutex>

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




struct List
{

    std::mutex mtx;
    
    struct Node
    {
        Node(){}
        
        Node(const int& value) : value(value), next(0) {}
        int              value;
        Node* next;
    };
    ~List() { while (pop()); }
    void push(const int& x) {
        Node* node = new Node(x);
        if(head) node -> next = head;
        head = node;
    }
    void push(Node* newNode){
        if(newNode == nullptr) return;
        std::lock_guard <std::mutex> lock(mtx);
        if(head) newNode -> next = head;
        head = newNode;
    }
    
    Node* pop() {
        std::lock_guard <std::mutex> lock(mtx);
        Node* node = head;
        if(head == nullptr) return nullptr;
        head = head -> next ? head->next : nullptr;
        node -> next = nullptr;
        return node;
    }
    
    Node* head;
};


int main(){
    std::cout << "Hello" << '\n';
    int m = 100, n=100;
    int bigLoop = 100000;
    int itemNumber = 1000;
    int threadNumber = 10;
    std::cout << "m is "<< m << ", and n is " << n << '\n';
    std::cout << "Big loop number is " << bigLoop << "\n";
    std::cout << "Item number is " << itemNumber <<  '\n';
    std::cout << "Thread number is " << threadNumber << '\n';

    std::clock_t start1, end1;
    double cpu_time_used_sec1;
    
    List Consumer1, Producer1;
    for(int i=0; i<itemNumber; ++i)
        Producer1.push(i);
    auto producer_consumer1 = [&Consumer1, &Producer1, &m, &n,&bigLoop](){
        int i=0;
        while(++i < bigLoop){
            for(int i=0; i<m; i++)
                Consumer1.push(Producer1.pop());

            for(int i=0; i<n; i++)
                Producer1.push(Consumer1.pop());
        }
    };
    std::thread* t = new std::thread[threadNumber-1];
    start1 = std::clock();
    for(int i=0; i<threadNumber-1; ++i)
        t[i] = std::thread(producer_consumer1);
    producer_consumer1();
    for(int i=0; i<threadNumber-1; ++i)
        t[i].join();
    end1 = std::clock();


    std::clock_t start2, end2;
    double cpu_time_used_sec2;
    LockFreeList Consumer2, Producer2;
    for(int i=0; i<itemNumber; ++i)
        Producer2.push(i);
    auto producer_consumer2 = [&Consumer2, &Producer2,&m, &n,&bigLoop](){
        int i=0;
        while(++i < bigLoop){
            for(int i=0; i<m; i++)
                Consumer2.push(Producer2.pop());

            for(int i=0; i<n; i++)
                Producer2.push(Consumer2.pop());
        }
    };
    
    
    std::thread* th = new std::thread[threadNumber-1];

    start2 = std::clock();
    for(int i=0; i<threadNumber-1; ++i)
        th[i] = std::thread(producer_consumer2);
    producer_consumer2();
    for(int i=0; i<threadNumber-1; ++i)
        th[i].join();
    end2 = std::clock();

    
    int cpu_time_used_ms1 = (int)(((double) (end1 - start1)) / CLOCKS_PER_SEC * 1000);
    int cpu_time_used_ms2 = (int)(((double) (end2 - start2)) / CLOCKS_PER_SEC * 1000);
    
    std::cout << "수행 결과" << '\n';
    std::cout << "linked-list using lock :      " << cpu_time_used_ms1 << "ms" << '\n';
    std::cout << "linked-list using lock-free : " << cpu_time_used_ms2 << "ms" << '\n';
    delete[] th;
    // delete[] t;
    return 0;
}


