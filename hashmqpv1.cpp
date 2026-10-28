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
};

struct Map {
  std::vector<std::vector<Entry>> buckets{8};
  size_t count = 0;

  size_t slot(uint64_t k) const { return hash(k) % buckets.size(); }

  void put(uint64_t k, int v) {
    for (Entry& e : buckets[slot(k)])
      if (e.key == k) {
        e.value = v;
        return;
      }
    buckets[slot(k)].push_back({k, v});

    ++count;
    if (count > 0.75 * buckets.size())
      rehash(buckets.size() * 2);
  }

  const int *get(uint64_t k) const {
    const std::vector<Entry> &bucket = buckets[slot(k)];

    for (const Entry &e : bucket) {
      if (e.key == k)
        return &e.value;
    }
    
    return nullptr; // nothing in the bucket
  }

  void rehash(size_t n) {
    std::vector<std::vector<Entry>> nb(n);

    for (auto& b : buckets)
      for (auto& e : b)
        nb[hash(e.key) % n].push_back(e);

    buckets.swap(nb);
  }
};

int main() { 
  Map map1{};

  for (int i = 0; i < 8; ++i) {
    uint64_t key = (i+1)*2;
    int value = i;
    map1.put(key, value);

    std::cout << *map1.get(key) << std::endl;
    std::cout << map1.buckets.size() << '\n';
  }
  
  return 0;
}
