#pragma once
#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "converters.hpp"

struct GlobalSettingsNamespace {};

template <typename SettingsNamespace = GlobalSettingsNamespace>
class Configurator;

class UnknownParameter : public std::logic_error {
 public:
  explicit UnknownParameter(
      const std::string& k_msg = "Error: Unknown parameter is given")
      : std::logic_error(k_msg) {}
};

class AlreadyRegistered : public std::logic_error {
 public:
  explicit AlreadyRegistered(
      const std::string& k_msg = "Error: parameter already registered")
      : std::logic_error(k_msg) {}
};

class InvalidType : public std::logic_error {
 public:
  explicit InvalidType(const std::string& k_msg = "Error: Wrong type is given")
      : std::logic_error(k_msg) {}
};

class InvalidConfig : public std::logic_error {
 public:
  explicit InvalidConfig(
      const std::string& k_msg = "Error: Invalid configuration is given")
      : std::logic_error(k_msg) {}
};

class BadSettingAccess : public std::logic_error {
 public:
  explicit BadSettingAccess(
      const std::string& k_msg = "Error: There is no value")
      : std::logic_error(k_msg) {}
};

class BaseSetting {
 public:
  virtual ~BaseSetting() = default;

  virtual const std::type_info& Type() const noexcept = 0;

  virtual bool HasValue() const noexcept = 0;

  virtual const std::string& GetHelp() const noexcept = 0;

  virtual void Drop() noexcept = 0;

  virtual void SetValueFromString(const std::string& raw) = 0;
};

template <typename T, typename SettingsNamespace = GlobalSettingsNamespace>
class Setting : public BaseSetting {
 public:
  using Callback = std::function<void(const T&)>;

  Setting(std::string name) : name_(std::move(name)) {
    Configurator<SettingsNamespace>::GetInstance().Register(name_, this);
  }

  Setting(std::string name, T value)
      : name_(std::move(name)), value_(std::move(value)) {
    Configurator<SettingsNamespace>::GetInstance().Register(name_, this);
  }

  Setting(std::string name, T value, std::string help)
      : name_(std::move(name)),
        value_(std::move(value)),
        help_(std::move(help)) {
    Configurator<SettingsNamespace>::GetInstance().Register(name_, this);
  }

  Setting(std::string name, T value, std::string help, Callback func)
      : name_(std::move(name)),
        value_(std::move(value)),
        help_(std::move(help)),
        func_(std::move(func)) {
    Configurator<SettingsNamespace>::GetInstance().Register(name_, this);
  }

  const std::type_info& Type() const noexcept override { return typeid(T); }

  bool HasValue() const noexcept override { return value_ != std::nullopt; }

  const T& GetValue() const;

  const std::string& GetHelp() const noexcept override { return help_; }

  void SetValue(T new_value);

  void Drop() noexcept override { value_ = std::nullopt; }

  void SetValueFromString(const std::string& raw) override {
    SetValue(ConvertFromString<T>(raw));
  }

  const std::optional<T>& RawGetValue() const noexcept { return value_; }

 private:
  std::string name_;
  std::optional<T> value_;
  std::string help_;
  Callback func_;
};

template <typename SettingsNamespace>
class Configurator {
 public:
  static Configurator& GetInstance() noexcept;

  template <typename T>
  void SetValue(const std::string& name, T new_value);

  template <typename T>
  const std::optional<T>& GetValue(const std::string& name) const;

  void Drop(const std::string& name);

  std::unordered_map<std::string, std::string> GetHelp() const;

  std::string GetHelp(const std::string& name) const;

  void Init(const std::unordered_map<std::string, std::string>& config);

  void Register(const std::string& name, BaseSetting* setting);

 private:
  std::unordered_map<std::string, BaseSetting*> storage_;
};

template <typename T, typename SettingsNamespace>
const T& Setting<T, SettingsNamespace>::GetValue() const {
  if (value_ == std::nullopt) {
    throw BadSettingAccess();
  }

  return *value_;
}

template <typename T, typename SettingsNamespace>
void Setting<T, SettingsNamespace>::SetValue(T new_value) {
  value_ = std::move(new_value);

  if (func_) {
    func_(*value_);
  }
}

template <typename SettingsNamespace>
void Configurator<SettingsNamespace>::Register(const std::string& name,
                                               BaseSetting* setting) {
  if (storage_.contains(name)) {
    throw AlreadyRegistered();
  }

  storage_[name] = setting;
}

template <typename SettingsNamespace>
Configurator<SettingsNamespace>&
Configurator<SettingsNamespace>::GetInstance() noexcept {
  static Configurator instance;
  return instance;
}

template <typename SettingsNamespace>
template <typename T>
void Configurator<SettingsNamespace>::SetValue(const std::string& name,
                                               T new_value) {
  auto it = storage_.find(name);

  if (it == storage_.end()) {
    throw UnknownParameter();
  }

  BaseSetting* setting = it->second;

  if (setting->Type() != typeid(T)) {
    throw InvalidType();
  }

  static_cast<Setting<T, SettingsNamespace>*>(setting)->SetValue(
      std::move(new_value));
}

template <typename SettingsNamespace>
template <typename T>
const std::optional<T>& Configurator<SettingsNamespace>::GetValue(
    const std::string& name) const {
  auto it = storage_.find(name);

  if (it == storage_.end()) {
    throw UnknownParameter();
  }

  BaseSetting* setting = it->second;

  if (setting->Type() != typeid(T)) {
    throw InvalidType();
  }

  return static_cast<Setting<T, SettingsNamespace>*>(setting)->RawGetValue();
}

template <typename SettingsNamespace>
void Configurator<SettingsNamespace>::Drop(const std::string& name) {
  auto it = storage_.find(name);

  if (it == storage_.end()) {
    throw UnknownParameter();
  }

  BaseSetting* setting = it->second;

  setting->Drop();
}

template <typename SettingsNamespace>
std::unordered_map<std::string, std::string>
Configurator<SettingsNamespace>::GetHelp() const {
  std::unordered_map<std::string, std::string> help_storage;

  for (const auto& [name, setting] : storage_) {
    help_storage[name] = setting->GetHelp();
  }

  return help_storage;
}

template <typename SettingsNamespace>
std::string Configurator<SettingsNamespace>::GetHelp(
    const std::string& name) const {
  auto it = storage_.find(name);

  if (it == storage_.end()) {
    throw UnknownParameter();
  }

  BaseSetting* setting = it->second;

  return setting->GetHelp();
}

template <typename SettingsNamespace>
void Configurator<SettingsNamespace>::Init(
    const std::unordered_map<std::string, std::string>& config) {
  for (const auto& [name, new_value] : config) {
    auto it = storage_.find(name);

    if (it == storage_.end()) {
      throw InvalidConfig();
    }

    BaseSetting* setting = it->second;

    setting->SetValueFromString(new_value);
  }
}
