#pragma once
#include <concepts>

#include <pb/Error.h>

namespace PB {
template <typename T>
concept TaskManageableConcept = requires(T t) {
  {
    t.onFinished()
  } -> std::same_as<void>;

  {
    t.onStopped()
  } -> std::same_as<void>;
  {
    t.onStarted()
  } -> std::same_as<void>;

  {
    t.onPaused()
  } -> std::same_as<void>;

  {
    t.onResumed()
  } -> std::same_as<void>;

  {
    t.onProgressUpdate()
  } -> std::same_as<void>;

  {
    t.onError(PB::Error())
  } -> std::same_as<void>;
};

template <typename T, typename MonoidType>
concept RandomAccessibleConcept = requires(T t, unsigned index) {
  {
    t.size()
  } -> std::same_as<unsigned>;

  {
    t.access(index)
  } -> std::same_as<MonoidType>;
};

} // namespace PB