#include <cstdint>
#include <iostream>
#include <vector>

uint64_t hash(uint64_t x) {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}

struct Entry {
  uint64_t key;
  int value;
  int32_t next;
};

struct Map {
  std::vector<Entry> entries;
  std::vector<int32_t> first;
  Map () : first(8, -1) {}

  size_t count = 0;

  size_t slot(uint64_t k) const { return hash(k) % first.size(); }

  void put(uint64_t k, int v) {
    size_t b = slot(k);

    for (int32_t i = first[b]; i != -1; i = entries[i].next) {
      if (entries[i].key == k) {
        entries[i].value = v;
        return;
      }
    }

    int32_t i = static_cast<int32_t>(entries.size());
    entries.push_back({k, v, first[b]});
    first[b] = i;

    ++count;
    if (count > 0.75 * first.size())
      rehash(first.size() * 2);
  }

  const int *get(uint64_t k) const {
    int32_t i = first[slot(k)];

    for (; i != -1; i = entries[i].next) {
      if (entries[i].key == k)
        return &entries[i].value;
    }
    
    return nullptr; // nothing in the bucket
  }

  void rehash(size_t n) {
    first.assign(n, -1);

    for (int i = 0; i < static_cast<int32_t>(entries.size()); ++i) {
      size_t b = hash(entries[i].key) % n;
      entries[i].next = first[b];
      first[b] = i;
    }
  }
};

int main() { 
  Map map1{};

  for (int i = 0; i < 8; ++i) {
    uint64_t key = (i+1)*2;
    int value = i;
    map1.put(key, value);

    std::cout << *map1.get(key) << std::endl;
    std::cout << map1.first.size() << '\n';
  }
  
  return 0;
}
