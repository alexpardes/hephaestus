#pragma once

class Serialization {
  public:
    template<typename T> static void Write(char *&bytes, const T &t) {
      for (size_t i = 0; i < sizeof(t); ++i) {
        *bytes++ = (((char *)&t)[i]);
      }
    }

    template<typename T> static T Read(char *&bytes) {
      T t = *static_cast<T*>(static_cast<void*>(bytes));
      bytes += sizeof(T);
      return t;
    }
};