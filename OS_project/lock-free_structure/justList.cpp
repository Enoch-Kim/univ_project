#include <string>
#include <atomic>
#include <thread>
#include <iostream>
#include <time.h>



static volatile std::atomic<char*> _str = "abc";  // c++ 의 string은 + atomic에 사용할 수 없어 char* 사용 
static volatile std::atomic<int> _val = 0;
// static volatile int _val =0;

static void AppendStr(char* newstr){
    while(true){
        char* original = _str;
        char* newString = new char[10];
        strcat(newString, _str);
        strcat(newString, newstr);
        if(_str.compare_exchange_strong(original, newString))
            break;
        
    }
}

static void AddValue(int value)
{
    while (true)
    {
        int orgVal = _val;
        int newVal = orgVal + value;
        if(_val.compare_exchange_strong(orgVal, newVal))
            break;
        else
            std::cout << "exchange fail and _val is " << _val << '\n';
    }

}

static void ThreadBody()
{
    for (int i = 0; i < 100000; i++)  // Main 스레드와 병행 수행
        AddValue(1);
        // _val+=1;
}





struct List
{

    struct Node
    {
        Node(){}
        
        Node(const int& value) : value(value), next(0) {}
        int              value;
        Node* next;
    };

    ~List() {
        while (head){
            Node* oldHead = pop();
            delete oldHead;
        };
    }

    void push(const int& x) {
        Node* node = new Node(x);
        if(head) node -> next = head;
        head = node;
    }

    void push(Node* newNode){
        if(head) newNode -> next = head;
        head = newNode;
    }
    
    Node* pop() {
        Node* node = head;
        head = head -> next;
        return node;
    }
    
    Node* head;
};



int main(){
    std::clock_t start, end;
    double cpu_time_used_sec;
    
    List Consumer, Producer;
    for(int i=0; i<1000; ++i)
        Producer.push(i);

    auto producer_consumer = [&Consumer, &Producer](){
        int i=0;
        while(++i < 100000){
            // for(int i=0; i<100; i++)
                if(Producer.head)
                    Consumer.push(Producer.pop());

            // for(int i=0; i<200; i++)
                if(Consumer.head)
                    Producer.push(Consumer.pop());
        }
    };

    start = std::clock();
    std::thread t1(producer_consumer);
    std::thread t2(producer_consumer);
    producer_consumer();

    t1.join();
    t2.join();

    auto node = Consumer.head;
    while(node){
        std::cout << node->value << '\n';
        node = node -> next;
    }
    node = Producer.head;
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

