#pragma once

#include <vector>

namespace PB {
class ObservableSubject;

class Observer {
public:
  virtual ~Observer() = default;

  virtual void update(ObservableSubject& subject) = 0;

protected:
  Observer() = default;

private:
  Observer(Observer const &other) = delete;
  Observer &operator=(Observer const &other) = delete;
};

class ObservableSubject {
public:
  virtual ~ObservableSubject() = default;
  virtual void notify() = 0;

protected:
  ObservableSubject() = default;

private:
  std::vector<Observer*> mObservers;
};

} // namespace PB