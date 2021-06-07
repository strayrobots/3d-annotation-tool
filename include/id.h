#pragma once

class IdFactory {
private:
  int id = 0;

public:
  static IdFactory& getInstance() {
    static IdFactory instance;
    return instance;
  }
  IdFactory(IdFactory const&) = delete;
  void operator=(IdFactory const&) = delete;
  int getId() {
    return ++id;
  }

private:
  IdFactory() {}
};
