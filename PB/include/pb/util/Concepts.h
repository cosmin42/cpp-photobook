#pragma once

#include <concepts>
#include <optional>

#include <pb/Error.h>
#include <pb/util/Traits.h>

namespace PB {
template <typename T>
concept PhotoBookListenerConcept = requires(T t) {
  {
    t.onFinished(PB::Path())
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
    t.onProgressUpdate(PB::Path(), int(), int())
  } -> std::same_as<void>;

  {
    t.onError(PB::Error())
  } -> std::same_as<void>;
};

} // namespace PB