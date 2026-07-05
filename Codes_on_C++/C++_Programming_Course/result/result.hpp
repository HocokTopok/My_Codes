#pragma once

#include <concepts>
#include <exception>

template <typename Ok, typename Err>
class Result;

namespace result {
class OkClass {};
class ErrClass {};
struct Monostate {};

template <typename Ok = Monostate, typename Err = Monostate>
Result<std::decay_t<Ok>, std::decay_t<Err>> Value(
    Ok&& value, [[maybe_unused]] OkClass /*If Ok==Err*/ = OkClass());

template <typename Ok = Monostate, typename Err = Monostate>
Result<std::decay_t<Ok>, std::decay_t<Err>> Error(
    Err&& error, [[maybe_unused]] ErrClass /*If Ok==Err*/ = ErrClass());
}  // namespace result

template <typename Ok, typename Err>
class Result {
 public:
  using OkType = Ok;
  using ErrType = Err;

  ~Result();

  Result(const Result& other);

  Result& operator=(const Result& other);

  Result(Result&& other);

  Result& operator=(Result&& other);

  Ok&& Value() && { return std::move(ok); }

  const Ok& Value() const& { return ok; }

  Err&& Error() && { return std::move(err); }

  const Err& Error() const& { return err; }

  template <typename Default>
    requires std::convertible_to<Default, Ok> ||
             std::is_same_v<Ok, result::Monostate>
  auto ValueOr(Default backup) const& noexcept;

  template <typename Default>
    requires std::convertible_to<Default, Ok> ||
             std::is_same_v<Ok, result::Monostate>
  auto ValueOr(Default backup) && noexcept;

  template <typename Default>
    requires std::convertible_to<Default, Err> ||
             std::is_same_v<Err, result::Monostate>
  auto ErrorOr(Default backup) const& noexcept;

  template <typename Default>
    requires std::convertible_to<Default, Err> ||
             std::is_same_v<Err, result::Monostate>
  auto ErrorOr(Default backup) && noexcept;

  bool HasValue() const noexcept { return has_value_; }

  template <typename Func>
  auto AndThen(Func func) &&;

  template <typename Func>
  auto OrElse(Func func) &&;

  template <typename Func>
  auto Map(Func func) &&;

  template <typename Func>
  auto MapError(Func func) &&;

  template <typename OtherOk, typename OtherErr>
  friend class Result;

  template <typename OtherOk, typename OtherErr>
  friend Result<std::decay_t<OtherOk>, std::decay_t<OtherErr>> result::Value(
      OtherOk&& value, result::OkClass);

  template <typename OtherOk, typename OtherErr>
  friend Result<std::decay_t<OtherOk>, std::decay_t<OtherErr>> result::Error(
      OtherErr&& error, result::ErrClass);

  template <typename NewOk, typename NewErr>
    requires std::convertible_to<Ok&&, NewOk> &&
             std::convertible_to<Err&&, NewErr>
  operator Result<NewOk, NewErr>() &&;

  template <typename NewOk, typename NewErr>
    requires std::convertible_to<Ok&&, NewOk> &&
             std::is_same_v<Err, result::Monostate> &&
             (!std::convertible_to<Err &&, NewErr>)
  operator Result<NewOk, NewErr>() &&;

  template <typename NewOk, typename NewErr>
    requires std::convertible_to<Err&&, NewErr> &&
             std::is_same_v<Ok, result::Monostate> &&
             (!std::convertible_to<Ok &&, NewOk>)
  operator Result<NewOk, NewErr>() &&;

 private:
  Result(Ok value, result::OkClass /*If Ok==Err*/);

  Result(Err error, result::ErrClass /*If Ok==Err*/);

  void Clear() noexcept;

  union {
    Ok ok;
    Err err;
  };
  bool has_value_;
};

template <typename Ok, typename Err>
Result<std::decay_t<Ok>, std::decay_t<Err>> result::Value(
    Ok&& value, result::OkClass /*If Ok==Err*/) {
  return Result<std::decay_t<Ok>, std::decay_t<Err>>(std::move(value),
                                                     result::OkClass());
}

template <typename Ok, typename Err>
Result<std::decay_t<Ok>, std::decay_t<Err>> result::Error(
    Err&& error, result::ErrClass /*If Ok==Err*/) {
  return Result<std::decay_t<Ok>, std::decay_t<Err>>(std::move(error),
                                                     result::ErrClass());
}

template <typename Ok, typename Err>
Result<Ok, Err>::Result(Ok value, result::OkClass /*If Ok==Err*/)
    : has_value_(true) {
  new (&ok) Ok(std::move(value));
}

template <typename Ok, typename Err>
Result<Ok, Err>::Result(Err error, result::ErrClass /*If Ok==Err*/)
    : has_value_(false) {
  new (&err) Err(std::move(error));
}

template <typename Ok, typename Err>
template <typename NewOk, typename NewErr>
  requires std::convertible_to<Ok&&, NewOk> &&
           std::convertible_to<Err&&, NewErr>
Result<Ok, Err>::operator Result<NewOk, NewErr>() && {
  if (HasValue()) {
    return Result<NewOk, NewErr>(std::move(*this).Value(), result::OkClass());
  }
  return Result<NewOk, NewErr>(std::move(*this).Error(), result::ErrClass());
}

template <typename Ok, typename Err>
template <typename NewOk, typename NewErr>
  requires std::convertible_to<Ok&&, NewOk> &&
           std::is_same_v<Err, result::Monostate> &&
           (!std::convertible_to<Err &&, NewErr>)
Result<Ok, Err>::operator Result<NewOk, NewErr>() && {
  if (HasValue()) {
    return Result<NewOk, NewErr>(std::move(*this).Value(), result::OkClass());
  }
  std::terminate();
}

template <typename Ok, typename Err>
template <typename NewOk, typename NewErr>
  requires std::convertible_to<Err&&, NewErr> &&
           std::is_same_v<Ok, result::Monostate> &&
           (!std::convertible_to<Ok &&, NewOk>)
Result<Ok, Err>::operator Result<NewOk, NewErr>() && {
  if (!HasValue()) {
    return Result<NewOk, NewErr>(std::move(*this).Error(), result::ErrClass());
  }
  std::terminate();
}

template <typename Ok, typename Err>
void Result<Ok, Err>::Clear() noexcept {
  if (HasValue()) {
    ok.~Ok();
  } else {
    err.~Err();
  }
}

template <typename Ok, typename Err>
Result<Ok, Err>::Result(const Result& other) : has_value_(other.HasValue()) {
  if (other.HasValue()) {
    new (&ok) Ok(other.Value());
  } else {
    new (&err) Err(other.Error());
  }
}

template <typename Ok, typename Err>
Result<Ok, Err>& Result<Ok, Err>::operator=(const Result& other) {
  Clear();

  has_value_ = other.HasValue();

  if (other.HasValue()) {
    new (&ok) Ok((other.Value()));
  } else {
    new (&err) Err(other.Error());
  }

  return *this;
}

template <typename Ok, typename Err>
Result<Ok, Err>::Result(Result&& other) : has_value_(other.HasValue()) {
  if (other.HasValue()) {
    new (&ok) Ok(std::move(other).Value());
  } else {
    new (&err) Err(std::move(other).Error());
  }
}

template <typename Ok, typename Err>
Result<Ok, Err>& Result<Ok, Err>::operator=(Result&& other) {
  Clear();

  has_value_ = other.HasValue();

  if (other.HasValue()) {
    new (&ok) Ok(std::move(other).Value());
  } else {
    new (&err) Err(std::move(other).Error());
  }

  return *this;
}

template <typename Ok, typename Err>
Result<Ok, Err>::~Result() {
  Clear();
}

template <typename Ok, typename Err>
template <typename Default>
  requires std::convertible_to<Default, Ok> ||
           std::is_same_v<Ok, result::Monostate>
auto Result<Ok, Err>::ValueOr(Default backup) const& noexcept {
  if constexpr (std::is_same_v<Ok, result::Monostate>) {
    return backup;
  } else {
    if (HasValue()) {
      return Value();
    }
    return static_cast<Ok>(backup);
  }
}

template <typename Ok, typename Err>
template <typename Default>
  requires std::convertible_to<Default, Ok> ||
           std::is_same_v<Ok, result::Monostate>
auto Result<Ok, Err>::ValueOr(Default backup) && noexcept {
  if constexpr (std::is_same_v<Ok, result::Monostate>) {
    return backup;
  } else {
    if (HasValue()) {
      return std::move(*this).Value();
    }
    return static_cast<Ok>(std::move(backup));
  }
}

template <typename Ok, typename Err>
template <typename Default>
  requires std::convertible_to<Default, Err> ||
           std::is_same_v<Err, result::Monostate>
auto Result<Ok, Err>::ErrorOr(Default backup) const& noexcept {
  if constexpr (std::is_same_v<Err, result::Monostate>) {
    return backup;
  } else {
    if (!HasValue()) {
      return Error();
    }
    return static_cast<Err>(backup);
  }
}

template <typename Ok, typename Err>
template <typename Default>
  requires std::convertible_to<Default, Err> ||
           std::is_same_v<Err, result::Monostate>
auto Result<Ok, Err>::ErrorOr(Default backup) && noexcept {
  if constexpr (std::is_same_v<Err, result::Monostate>) {
    return backup;
  } else {
    if (!HasValue()) {
      return std::move(*this).Error();
    }
    return static_cast<Err>(std::move(backup));
  }
}

template <typename Ok, typename Err>
template <typename Func>
auto Result<Ok, Err>::AndThen(Func func) && {
  if constexpr (std::is_same_v<Ok, result::Monostate>) {
    return result::Error<Ok, Err>(std::move(*this).Error());
  } else {
    using ReturnType = decltype(std::declval<Func>()(std::declval<Ok&&>()));
    using NewOk = ReturnType::OkType;
    using NewErr = ReturnType::ErrType;
    using ActualErr =
        std::conditional_t<std::is_same_v<Err, result::Monostate>, NewErr, Err>;

    if (!HasValue()) {
      if constexpr (std::is_same_v<Err, result::Monostate>) {
        return std::move(func)(std::move(*this).Value());
      } else {
        if constexpr (std::is_convertible_v<Err, ActualErr>) {
          return result::Error<NewOk, ActualErr>(std::move(*this).Error());
        }
      }
    }
    return static_cast<Result<NewOk, ActualErr>>(
        std::move(func)(std::move(*this).Value()));
  }
}

template <typename Ok, typename Err>
template <typename Func>
auto Result<Ok, Err>::OrElse(Func func) && {
  if constexpr (std::is_same_v<Err, result::Monostate>) {
    return result::Value<Ok, Err>(std::move(*this).Value());
  } else {
    using ReturnType = decltype(std::declval<Func>()(std::declval<Err&&>()));
    using NewOk = ReturnType::OkType;
    using NewErr = ReturnType::ErrType;
    using ActualOk =
        std::conditional_t<std::is_same_v<Ok, result::Monostate>, NewOk, Ok>;

    if (HasValue()) {
      if constexpr (std::is_same_v<Ok, result::Monostate>) {
        return std::move(func)(std::move(*this).Error());
      } else {
        if constexpr (std::is_same_v<Ok, ActualOk>) {
          return result::Value<ActualOk, NewErr>(std::move(*this).Value());
        }
      }
    }
    return static_cast<Result<ActualOk, NewErr>>(
        std::move(func)(std::move(*this).Error()));
  }
}

template <typename Ok, typename Err>
template <typename Func>
auto Result<Ok, Err>::Map(Func func) && {
  if constexpr (std::is_same_v<Ok, result::Monostate>) {
    return result::Error<Ok, Err>(std::move(*this).Error());
  } else {
    using NewOk = decltype(std::declval<Func>()(std::declval<Ok>()));

    if (HasValue()) {
      return result::Value<NewOk, Err>(std::move(func)(std::move(ok)));
    }
    return result::Error<NewOk, Err>(std::move(err));
  }
}
template <typename Ok, typename Err>
template <typename Func>
auto Result<Ok, Err>::MapError(Func func) && {
  if constexpr (std::is_same_v<Err, result::Monostate>) {
    return result::Value<Ok, Err>(std::move(*this).Value());
  } else {
    using NewErr = decltype(std::declval<Func>()(std::declval<Err>()));

    if (!HasValue()) {
      return result::Error<Ok, NewErr>(std::move(func)(std::move(err)));
    }
    return result::Value<Ok, NewErr>(std::move(ok));
  }
}