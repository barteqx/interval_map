#include <iostream>
#include <iterator>
#include <limits>
#include <map>

template <typename K, typename V>
class interval_map {
  // The map keeps the beginnings of interval as the keys and the values for
  // given interval as the values. std::map is used because we need to keep the
  // intervals sorted.
  V begin_value_;
  std::map<K, V> map_;

 public:
  interval_map(V const& val) : begin_value_(val) {}

  void insert(K const& key_begin, K const& key_end, V const& val) {
    if (key_begin >= key_end) {
      // If key_begin >= key_end, then we have an empty interval.
      return;
    }
    // First we need to insert the end of the interval.
    auto it_end = map_.find(key_end);
    auto lower_bound = map_.lower_bound(key_end);
    if (it_end != map_.end()) {
      // If the end of the interval already exists, then we need to update the
      // value of the interval.
      it_end->second = lower_bound->second;
    } else {
      // If the end of the interval does not exist, then we need to insert it.
      // We insert it by adding the value of the interval before the end of the
      // interval.
      if (lower_bound == map_.begin()) {
        // If the found interval is the first interval, then we need to insert
        // the begin_value_.
        it_end = map_.insert(map_.end(), std::make_pair(key_end, begin_value_));
      } else {
        it_end = map_.insert(map_.end(),
                              std::make_pair(key_end, (--lower_bound)->second));
      }
    }

    // Now we need to insert the beginning of the interval.
    auto it_begin = map_.insert_or_assign(key_begin, val).first;

    // We need to remove the intervals that are inside the inserted interval.
    map_.erase(std::next(it_begin), it_end);

    // Making the map canonical.
    auto right_it = it_end;
    auto left_it = (it_begin != map_.begin() ? std::prev(it_begin) : it_begin);

    while (right_it != left_it) {
      // If the value of the interval is the same as the value of the interval
      // before, then we need to remove the interval.
      auto next_it = std::prev(right_it);
      if (right_it->second == next_it->second) {
        map_.erase(right_it);
      }
      right_it = next_it;
    }
    // Special case for merging the interval with the 'original' interval.
    if (left_it == map_.begin() && left_it->second == begin_value_) {
      // If the value of the leftmost interval is at the beginning of the map_
      // and has the value of the original interval, then we need to merge it
      // with the original interval.
      map_.erase(left_it);
    }
  }

  // erase/remove methods are not needed. They are not needed because we can
  // use the insert method to insert an interval that will remove intervals
  // that are inside the inserted interval and merge intervals that have the
  // same value that are adjacent.

  V const& at(K const& key) const {
    auto it = map_.upper_bound(key);
    if (it == map_.begin()) {
      return begin_value_;
    } else {
      return (--it)->second;
    }
  }

  V const& operator[](K const& key) const { return at(key); }

  void print() {
    for (auto&& [key, val] : map_) {
      std::cout << "[" << key << ':' << val << "]";
    }
    std::cout << std::endl;
  }
};

int main() {
  interval_map<int, char> map{'a'};
  map.print();

  map.insert(3, 5, 'b');
  map.print();  // [3:b][5:a]

  map.insert(2, 3, 'c');
  map.print();  // [2:c][3:b][5:a]

  map.insert(2, 3, 'd');
  map.print();  // [2:d][3:b][5:a]

  map.insert(2, 4, 'e');
  map.print();  // [2:e][4:b][5:a]

  map.insert(4, 18, 'f');
  map.print();  // [2:e][4:f][18:a]

  map.insert(2, 8, 'g');
  map.print();  // [2:g][8:f][18:a]

  std::cout << map.at(-100) << std::endl;  // a
  std::cout << map.at(2) << std::endl;     // g
  std::cout << map.at(4) << std::endl;     // g
  std::cout << map.at(5) << std::endl;     // g
  std::cout << map.at(12) << std::endl;    // f
  std::cout << map.at(18) << std::endl;    // a

  map.insert(0, 18, 'z');
  map.print();  // [10:f][18:a]

  map.insert(0, 20, 'a');
  map.print();  // 

  map.insert(0, 30, 'a');
  map.print();  // 

  map.insert(0, 3100, 'a');
  map.print();  // 
}