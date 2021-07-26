#ifndef HTCW_ALGO_HPP
#define HTCW_ALGO_HPP
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
namespace algo {
    enum struct algo_result {
        success = 0,
        invalid_argument = 1,
        out_of_memory = 2,
        item_not_found = 3,
        invalid_state = 4,
        duplicate_key=5
    };

    template<typename ValueType> class linked_list final {
        struct node {
            ValueType value;
            node* next;
        };
        size_t m_size;
        node* m_head;
        void*(*m_allocator)(size_t);
        void(*m_deallocator)(void*);
        linked_list(const linked_list& rhs)=delete;
        linked_list& operator=(const linked_list& rhs)=delete;
        node* node_at(size_t index) const {
            if(index>=m_size) return nullptr;
            node* n = m_head;
            while(nullptr!=n) {
                if(index==0) {
                    return n;
                }
                n=n->next;
                --index;
            }
            return nullptr;
        }
    public:
        using value_type = ValueType;
        class enumerator {
            friend class linked_list;
            node* m_current;
        public:
            inline enumerator() : m_current(nullptr) {}
            inline value_type operator*() {
                if(nullptr==m_current) {
                    return value_type();
                }
                return m_current->value;
            }
            inline value_type* value_pointer() {
                if(nullptr==m_current) {
                    return nullptr;
                }
                return &m_current->value;
            }
            inline enumerator& operator++() {
                if(nullptr!=m_current)
                    m_current=m_current->next;
                return *this;
            }
            inline enumerator operator++(int) {
                enumerator result;
                result.m_current=m_current;
                if(nullptr!=m_current) {
                    m_current=m_current->next;
                }
                return result;
            }
            inline bool operator==(const enumerator &rhs) const {
                return m_current == rhs.m_current;
            }
            inline bool operator!=(const enumerator &rhs) const {
                return m_current != rhs.m_current;
            }
            
            inline bool operator==(const enumerator &rhs) {
                return m_current == rhs.m_current;
            }
            inline bool operator!=(const enumerator &rhs) {
                return m_current != rhs.m_current;
            }
        };
        linked_list(void*(*allocator)(size_t)=malloc,void(*deallocator)(void*)=free) : 
                                                                        m_size(0),
                                                                        m_head(nullptr),
                                                                        m_allocator(allocator),
                                                                        m_deallocator(deallocator) {

        }
        linked_list(linked_list&& rhs)  : 
                            m_size(rhs.m_size),
                            m_head(rhs.m_head),
                            m_allocator(rhs.m_allocator),
                            m_deallocator(rhs.m_deallocator) {
            rhs.m_size=0;
            rhs.m_head = nullptr;
        }
        linked_list& operator=(linked_list&& rhs) {
            clear();
            m_size=rhs.m_size;
            m_head=rhs.m_head;
            m_allocator=rhs.m_allocator;
            m_deallocator=rhs.m_deallocator;
            rhs.m_size=0;
            rhs.m_head = nullptr;
            return *this;
        }
        ~linked_list() {
            clear();
        }
        size_t size() const {
            return m_size;
        }
        enumerator begin() {
            enumerator result;
            result.m_current = m_head;
            return result;
        }
        enumerator end() {
            enumerator result;
            result.m_current = nullptr;
            return result;
        }
        value_type* at(size_t index) const {
            node* n = node_at(index);
            if(nullptr!=n) {
                return &n->value;
            }
            return nullptr;
        }
        algo_result at(size_t index,value_type& value) {
            if(index>=m_size) return algo_result::item_not_found;
            node* n = node_at(index);
            if(nullptr==n) return algo_result::item_not_found;
            n->value = value;
            return algo_result::success;
        }
        algo_result copy_to(value_type* array,size_t source_index,size_t count) {
            if(nullptr==array) return algo_result::invalid_argument;
            if(source_index+count>m_size) return algo_result::invalid_argument;
            if(0==count) return algo_result::success;
            node* n = node_at(source_index);
            if(nullptr==n) return algo_result::invalid_argument;
            while(nullptr!=n) {
                *array=n->value;
                ++array;
                n=n->next;
            }
            return algo_result::success;
        }
        algo_result add(const value_type& value) {
            node* n = (node*)m_allocator(sizeof(node));
            if(nullptr==n) return algo_result::out_of_memory;
            n->value = value;
            n->next = nullptr;
            if(0==m_size) {
                m_head = n;
            } else {
                node* pn = node_at(m_size-1);
                pn->next = n;
            }
            ++m_size;
            return algo_result::success;
        }
        algo_result insert(size_t index,const value_type& value) {
            if(index>m_size) return algo_result::invalid_argument;
            node* n = (node*)m_allocator(sizeof(node));
            if(nullptr==n) return algo_result::out_of_memory;
            n->value = value;
            n->next = nullptr;
            if(0==m_size) {
                m_head = n;
            } else {
                if(0==index) {
                    n->next = m_head;
                    m_head = n;
                } else {
                    node* pn = node_at(index-1);
                    n->next = pn->next;
                    pn->next = n;
                }
            }
            ++m_size;
            return algo_result::success;
        }
        algo_result remove_at(size_t index) {
            if(0==m_size || index>=m_size) return algo_result::invalid_argument;
            if(0==index) {
                node* tmp = m_head;
                m_head = m_head->next;
                m_deallocator(tmp);
            } else {
                node* n = node_at(index-1);
                node* tmp = n->next;
                n->next=n->next->next;
                m_deallocator(tmp);
            }
            --m_size;
            return algo_result::success;
        }
        void clear() {
            m_size=0;
            node* n = m_head;
            while(nullptr!=n) {
                node* nn = n->next;
                m_deallocator(n);
                n=nn;
            }
            m_head = nullptr;
        }
    };
    template<typename KeyType,typename ValueType> struct key_value_pair final {
        using key_type = KeyType;
        using value_type = ValueType;
        key_type key;
        value_type value;
    };
    template<typename KeyType,typename ValueType,size_t(HashFunction(const KeyType&)),int(CompareFunction(const KeyType&,const KeyType&))> class linked_dictionary final {
    public:
        using key_type = KeyType;
        using value_type = ValueType;
        using entry_type = key_value_pair<KeyType,ValueType>;
        constexpr static const size_t minimum_buckets = 1;
        class enumerator {
            using buckets_type = linked_list<entry_type>;
            friend class linked_dictionary;
            linked_dictionary* m_parent;
            size_t m_index;
            typename buckets_type::enumerator m_bucket_it;
        public:
            inline entry_type operator*() {
                if(is_end()) {
                    return entry_type();
                }
                return *m_bucket_it;
            }
            inline bool is_end() const {
                return m_bucket_it==m_parent->m_buckets[0].end();
            }
            inline enumerator& operator++() {
                if(m_index<m_parent->m_buckets_size) {
                    auto it = ++m_bucket_it;
                    
                    if(it==m_parent->m_buckets[m_index].end()) {
                        while(++m_index<m_parent->m_buckets_size && 0==m_parent->m_buckets[m_index].size()); 
                        if(m_index<=m_parent->m_buckets_size) {
                            m_bucket_it = m_parent->m_buckets[m_index].begin();
                        } else  {
                            m_index = 0;
                            m_bucket_it=m_parent->m_buckets[0].end();
                        }
                    }
                } else { 
                    m_bucket_it=m_parent->m_buckets[0].end();
                    m_index=0;
                }
                return *this;
            }
            inline enumerator operator++(int) {
                enumerator result;
                result->m_parent = m_parent;
                result->m_index = m_index;
                result->m_bucket_it = m_bucket_it;
                ++*this;
                return result;
            }
            inline bool operator==(const enumerator &rhs) const {
                return m_bucket_it == rhs.m_bucket_it;
            }
            inline bool operator!=(const enumerator &rhs) const {
                return m_bucket_it != rhs.m_bucket_it;
            }
            inline bool operator==(const enumerator &rhs) {
                return m_bucket_it == rhs.m_bucket_it;
            }
            inline bool operator!=(const enumerator &rhs) {
                return m_bucket_it != rhs.m_bucket_it;
            }
        };
    private:
        using buckets_type = linked_list<entry_type>;
        size_t m_size;
        buckets_type* m_buckets;
        size_t m_buckets_size;
        void*(*m_allocator)(size_t);
        void(*m_deallocator)(void*);
        linked_dictionary(const linked_dictionary& rhs)=delete;
        linked_dictionary& operator=(const linked_dictionary& rhs)=delete;
    public:
        linked_dictionary(size_t buckets,void*(*allocator)(size_t)=malloc,void(*deallocator)(void*)=free) : m_size(0) {
            if(buckets<minimum_buckets) buckets=minimum_buckets;
            m_buckets = (buckets_type*)allocator(sizeof(buckets_type)*buckets);
            if(nullptr==m_buckets) {
                m_buckets_size = 0;
                return;
            }
            memset(m_buckets,0,sizeof(buckets_type)*buckets);
            for(int i = 0;i<buckets;++i) {
                m_buckets[i]=buckets_type(allocator,deallocator);
            }
            m_buckets_size = buckets;
            m_allocator = allocator;
            m_deallocator = deallocator;
        }
        linked_dictionary(linked_dictionary&& rhs) :
                                            m_size(rhs.m_size),
                                            m_buckets(rhs.m_buckets),
                                            m_buckets_size(rhs.m_buckets_size),
                                            m_allocator(rhs.m_allocator),
                                            m_deallocator(rhs.m_deallocator) {
            rhs.m_size = 0;
            rhs.m_buckets_size = 0;
            rhs.m_buckets = nullptr;
        }
        linked_dictionary& operator=(linked_dictionary&& rhs) {
            deinit();
            m_size=rhs.m_size;
            m_buckets=rhs.m_buckets;
            m_buckets_size=rhs.m_buckets_size;
            m_allocator=rhs.m_allocator;
            m_deallocator=rhs.m_deallocator;
            rhs.m_size = 0;
            rhs.m_buckets_size = 0;
            rhs.m_buckets = nullptr;
            return *this;
        }
        inline ~linked_dictionary() {
            deinit();
        }
        inline size_t size() const {
            return m_size;
        }
        bool contains_key(const key_type& key) {
            if(0==m_buckets_size) return false;
            size_t hash = HashFunction(key) % m_buckets_size;
            buckets_type* be=m_buckets[hash].end();
            for(auto it=m_buckets[hash].begin();it!=be;++it) {
                if(0==CompareFunction(it->key,key)) {
                    return true;
                }
            }
            return false;
        }
        value_type* item(const key_type& key) {
            if(0==m_buckets_size) return nullptr;
            size_t hash = HashFunction(key) % m_buckets_size;
            auto be=m_buckets[hash].end();
            for(auto it=m_buckets[hash].begin();it!=be;++it) {
                if(0==CompareFunction((*it).key,key)) {
                    return &it.value_pointer()->value;
                }
            }
            return nullptr;
        }
        algo_result item(const key_type& key,const value_type& value) {
            if(0==m_buckets_size) return algo_result::invalid_state;
            size_t hash = HashFunction(key) % m_buckets_size;
            auto be=m_buckets[hash].end();
            for(auto it=m_buckets[hash].begin();it!=be;++it) {
                if(0==CompareFunction((*it).key,key)) {
                    (*it).value=value;
                    return algo_result::success;
                }
            }
            return m_buckets[hash].add(key,value);
        }
        algo_result add(const key_type& key,const value_type& value) {
            if(0==m_buckets_size) return algo_result::invalid_state;
            size_t hash = HashFunction(key) % m_buckets_size;
            auto be=m_buckets[hash].end();
            for(auto it=m_buckets[hash].begin();it!=be;++it) {
                if(0==CompareFunction((*it).key,key)) {
                    return algo_result::duplicate_key;
                }
            }
            algo_result r=m_buckets[hash].add({key,value});
            if(algo_result::success!=r) {
                return r;
            }
            ++m_size;
            return algo_result::success;
        }
        algo_result remove(const key_type& key) {
            if(0==m_buckets_size) return algo_result::invalid_state;
            size_t hash = HashFunction(key) % m_buckets_size;
            auto be=m_buckets[hash].end();
            size_t i = 0;
            for(auto it=m_buckets[hash].begin();it!=be;++it) {
                if(0==CompareFunction((*it).key,key)) {
                    algo_result r = m_buckets[hash].remove_at(i);
                    if(algo_result::success!=r) {
                        return r;
                    }
                    --m_size;
                    return algo_result::success;
                }
                ++i;
            }
            return algo_result::success;
        }
        void clear() {
            m_size = 0;
            buckets_type* p = m_buckets;
            buckets_type* pe = m_buckets+m_buckets_size;
            while(p!=pe) {
                p->clear();
                ++p;
            }
        }
        enumerator begin() {
            enumerator result;
            if(0!=m_buckets_size) {
                result.m_parent = this;
                result.m_index = 0;
                if(0!=m_size) {
                    buckets_type* p = m_buckets;
                    while(0==p->size()) {
                        ++p;
                        ++result.m_index;
                    }
                    result.m_bucket_it = p->begin();
                }
            }
            return result;
        }
        enumerator end() {
            enumerator result;
            result.m_parent = this;
            result.m_index = 0;    
            if(0!=m_buckets_size) {
                result.m_bucket_it=m_buckets[0].end();
            }
            return result;
        }
        void deinit() {
            clear();
            if(nullptr!=m_buckets) {
                m_buckets_size = 0;
                m_deallocator(m_buckets);
                m_buckets = nullptr;
            }
        }
    };
    size_t string_hash(const char* const& sz) {   
        // from http://www.cse.yorku.ca/~oz/hash.html
        unsigned long hash = 5381;
        int c;
        const char* s=sz;
        while (c = *s++)
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        return hash;
    }
    size_t string_hash_ignore_case(const char* const& sz) {   
        // from http://www.cse.yorku.ca/~oz/hash.html
        unsigned long hash = 5381;
        int c;
        const char* s=sz;
        while (c = tolower(*s++))
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        return hash;
    }
    inline int string_compare(const char* const& lhs,const char* const& rhs) {
        return strcmp(lhs,rhs);
    }
    inline int string_compare_ignore_case(const char* const& lhs,const char* const& rhs) {
        return stricmp(lhs,rhs);
    }
}
#endif