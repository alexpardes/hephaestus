#pragma once

typedef std::vector<unsigned char>::iterator ByteIterator;

class Serialization {
  public:
    template<typename T> static void Add(std::vector<unsigned char> &bytes, T t) {
      for (size_t i = 0; i < sizeof(t); ++i) {
        bytes.push_back(((unsigned char *)&t)[i]);
      }
    }

    template<typename T> static T Deserialize(ByteIterator &start) {
      T t = *static_cast<T*>(static_cast<void*>(&*start));
      start += sizeof(T);
      return t;
    }
};