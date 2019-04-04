#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

struct pointData {
  int x;
  int y;
  bool operator==(const pointData &other) const {
    return ((x==other.x && y==other.y));
  }
};

struct pointHash {
  size_t operator()(const pointData &data) const {
    size_t res = 17;
    res = res*31 + hash<int>()(data.x);
    res = res*31 + hash<int>()(data.y);
    return res;
  }
};

#endif