#include <stdio.h>
#include <stdlib.h>
#include "algo.hpp"
using namespace algo;
void demo_linked_list() {
    linked_list<const char*> ll;
    ll.add("foo");
    ll.add("baz");
    ll.insert(1,"bar");
    printf("size = %d\r\n",(int)ll.size());
    for(int i = 0;i<ll.size();++i) {
        printf("item #%d = %s\r\n",(int)i+1,*ll.at(i));
    }
    printf("\r\n");
    ll.remove_at(0);
    for(int i = 0;i<ll.size();++i) {
        printf("item #%d = %s\r\n",(int)i+1,*ll.at(i));
    }
    printf("\r\n");
    ll.insert(0,"fubar");
    for(int i = 0;i<ll.size();++i) {
        printf("item #%d = %s\r\n",(int)i+1,*ll.at(i));
    }
    printf("\r\n");
    for(auto it = ll.begin();it!=ll.end();++it) {
        printf("item = %s\r\n",*it);
    }
    printf("\r\n");
    
}
void demo_linked_dictionary() {

}

int main(int argc, char** argv) {
    static const size_t buckets=53;
    using string_int_dictionary = linked_dictionary<const char*,int,string_hash,string_compare>;
    
    string_int_dictionary sid(buckets);
    sid.add("apple",1);
    sid.add("pear",5);
    sid.add("banana",6);
    //printf("sid end is %sequal to sid begin\r\n",sid.begin()!=sid.end()?"not ":"");
    //return 0;
    printf("size() = %d\r\n",(int)sid.size());
    
    printf("sid.item(\"banana\") = %d\r\n",*sid.item("banana"));
    printf("sid.item(\"pear\") = %d\r\n",*sid.item("pear"));
    printf("sid.item(\"apple\") = %d\r\n",*sid.item("apple"));
    printf("\r\n");
    printf("string_hash(\"pear\") % buckets = %llu\r\n",(unsigned long long)string_hash("pear")%buckets);
    printf("string_hash(\"apple\") % buckets = %llu\r\n",(unsigned long long)string_hash("apple")%buckets);
    printf("string_hash(\"banana\") % buckets = %llu\r\n",(unsigned long long)string_hash("banana")%buckets);

    return 0;
}