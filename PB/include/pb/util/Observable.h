#pragma once

#include <unordered_set>

namespace PBDev {
class ObservableSubject;

class Observer {
public:
  Observer(Observer &&other) noexcept { mOnChanged(&other, this); }
  virtual ~Observer() = default;

  Observer &operator=(Observer &&other) noexcept
  {
    mOnChanged(&other, this);
    return *this;
  }

  virtual void update(ObservableSubject &subject) = 0;

  void setChangeFunction(std::function<void(Observer *, Observer *)> changed)
  {
    mOnChanged = changed;
  }

protected:
  Observer() = default;

private:
  Observer(Observer const &other) = delete;
  Observer &operator=(Observer const &other) = delete;

  std::function<void(Observer *, Observer *)> mOnChanged;
};

class ObservableSubject {
public:
  virtual ~ObservableSubject() = default;
  virtual void notify() = 0;

  void attach(Observer *observer) { mObservers.insert(observer); }

  void dettach(Observer *observer)
  {
    if (mObservers.contains(observer)) {
      mObservers.erase(observer);
    }
  }

protected:
  ObservableSubject() = default;

  std::unordered_set<Observer *> mObservers;

private:
};

} // namespace PB::Util