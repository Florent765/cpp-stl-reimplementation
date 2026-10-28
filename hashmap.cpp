#include <iostream>
#include <string>
#include <vector>

unsigned hash(const std::string &s) {
  unsigned h = 0;
  for (char c : s)
    h += c; // add up the numbers of the letters
  return h;
}

struct Entry {
  std::string key;
  int value;
  bool used;
};

struct Map {
  std::vector<Entry> buckets{8};

  size_t slot(const std::string &k) const { return hash(k) % buckets.size(); }

  void put(const std::string &k, int v) { buckets[slot(k)] = {k, v, true}; }

  const int *get(const std::string &k) const {
    const Entry &e = buckets[slot(k)];
    if (e.used && e.key == k)
      return &e.value;

    return nullptr; // nothing in the bucket
  }
};

int main() {
  std::string s = "hello";
  std::cout << hash(s) << '\n';
  Map map1{};
  Entry bucket1 = {s, 42, true};

  auto index = map1.slot(s);

  map1.put(s, 42);
  unsigned test = 0;
  std::cout << test + s[0] << '\n';

  std::cout << map1.buckets[index].key << map1.buckets[index].value << '\n';
  return 0;
}
