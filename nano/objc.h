/*
 * Nano Library
 *
 * Copyright (C) 2022, Meta-Sonic
 * All rights reserved.
 *
 * Proprietary and confidential.
 * Any unauthorized copying, alteration, distribution, transmission, performance,
 * display or other use of this material is strictly prohibited.
 *
 * Written by Alexandre Arsenault <alx.arsenault@gmail.com>
 */

#pragma once

/*!
 * @file      objc.h
 * @brief     nano objc
 * @copyright Copyright (C) 2022, Meta-Sonic
 * @author    Alexandre Arsenault alx.arsenault@gmail.com
 * @date      Created 15/07/2022
 */

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <iostream>

//
// MARK: - Macros -
//

#define NANO_INLINE inline

#define NANO_CXPR constexpr

#define NANO_NOEXCEPT noexcept

#define NANO_NODISCARD [[nodiscard]]

/// @def NANO_INLINE_CXPR
#define NANO_INLINE_CXPR NANO_INLINE NANO_CXPR

/// @def NANO_NODC_INLINE
#define NANO_NODC_INLINE NANO_NODISCARD inline

/// @def NANO_NODC_INLINE_CXPR
#define NANO_NODC_INLINE_CXPR NANO_NODISCARD NANO_INLINE_CXPR

#define NANO_STRINGIFY(X) NANO_STR(X)
#define NANO_STR(X) #X

#ifdef _MSC_VER
  #define NANO_MSVC_PRAGMA(X) __pragma(X)
#else
  #define NANO_MSVC_PRAGMA(X)
#endif

#ifdef __clang__
  #define NANO_CLANG_PRAGMA(X) _Pragma(X)
#else
  #define NANO_CLANG_PRAGMA(X)
#endif

#define NANO_MSVC_DIAGNOSTIC_PUSH() NANO_MSVC_PRAGMA(warning(push))
#define NANO_MSVC_DIAGNOSTIC_POP() NANO_MSVC_PRAGMA(warning(pop))
#define NANO_MSVC_DIAGNOSTIC(X) NANO_MSVC_PRAGMA(warning(disable : X))

#define NANO_MSVC_PUSH_WARNING(X) NANO_MSVC_DIAGNOSTIC_PUSH() NANO_MSVC_PRAGMA(warning(disable : X))
#define NANO_MSVC_POP_WARNING() NANO_MSVC_DIAGNOSTIC_POP()

#define NANO_CLANG_DIAGNOSTIC_PUSH() NANO_CLANG_PRAGMA("clang diagnostic push")
#define NANO_CLANG_DIAGNOSTIC_POP() NANO_CLANG_PRAGMA("clang diagnostic pop")
#define NANO_CLANG_DIAGNOSTIC(TYPE, X) NANO_CLANG_PRAGMA(NANO_STRINGIFY(clang diagnostic TYPE X))

#define NANO_CLANG_PUSH_WARNING(X)                                                                                     \
  NANO_CLANG_DIAGNOSTIC_PUSH() NANO_CLANG_PRAGMA(NANO_STRINGIFY(clang diagnostic ignored X))

#define NANO_CLANG_POP_WARNING() NANO_CLANG_DIAGNOSTIC_POP()

NANO_CLANG_DIAGNOSTIC_PUSH()
NANO_CLANG_DIAGNOSTIC(warning, "-Weverything")
NANO_CLANG_DIAGNOSTIC(ignored, "-Wc++98-compat")

//
//
//

struct objc_class;
struct objc_object;
struct objc_selector;
struct objc_super;
struct objc_ivar;

namespace nano {
/// A unique_ptr for CFTypes.
/// The deleter will call CFRelease().
///
/// @remarks The _CFType can either be a CFTypeRef of the underlying type (e.g. CFStringRef or __CFString).
///          In other words, as opposed to std::unique_ptr<>, _CFType can also be a pointer.
template <class _CFType>
struct cf_unique_ptr;
} // namespace nano.

namespace nano::objc {

//
//
//

typedef objc_class class_t;
typedef objc_object obj_t;
typedef objc_object proto_t;
typedef objc_selector selector_t;
typedef objc_super super_t;
typedef objc_ivar ivar_t;

typedef void (*imp_ptr)();
typedef void (*send_super_ptr)();

extern send_super_ptr send_super_fct;

typedef long ns_int_t;
typedef unsigned long ns_uint_t;

struct obj_unique_ptr;

//
//
//

template <typename T>
constexpr bool is_basic_type = std::is_fundamental_v<T> //
    || std::is_fundamental_v<std::remove_pointer_t<T>> //
    || std::is_same_v<T, obj_t*> //
    || std::is_same_v<T, selector_t*> //
    || std::is_same_v<T, class_t*>;

template <typename T>
struct name_for_type {
  static constexpr bool value = false;
};

template <typename T>
constexpr bool has_name_for_type
    = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(name_for_type<T>::value)>>, bool>;

template <typename T, typename... Ts, std::enable_if_t<is_basic_type<T>, std::nullptr_t> = nullptr>
inline std::string get_encoding();

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_pointer_v<T>, std::nullptr_t> = nullptr>
inline std::string get_encoding(const char* name);

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_pointer_v<T> && has_name_for_type<std::remove_pointer_t<T>>,
        std::nullptr_t> = nullptr>
inline std::string get_encoding();

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_class_v<T> && std::is_trivial_v<T>, std::nullptr_t> = nullptr>
inline std::string get_encoding(const char* name);

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_class_v<T> && std::is_trivial_v<T> && has_name_for_type<T>,
        std::nullptr_t> = nullptr>
inline std::string get_encoding();

//
//
//

class_t* allocate_class(class_t* super, const char* name);
void register_class(class_t* c);
void dispose_class(class_t* c);
class_t* get_class(const char* name);
class_t* get_meta_class(const char* name);
const char* get_class_name(class_t* c);

imp_ptr get_class_method_implementation(class_t* c, selector_t* s);

bool add_class_method(class_t* c, selector_t* s, imp_ptr imp, const char* types);
bool add_class_pointer(class_t* c, const char* name, const char* className, std::size_t size, std::size_t align);
bool add_class_variable(class_t* c, const char* name, const char* encoding, std::size_t size, std::size_t align);

template <typename T>
inline bool add_class_variable(class_t* c, const char* name, const char* encoding);

proto_t* get_protocol(const char* name);
bool add_protocol(class_t* c, proto_t* protocol);
proto_t* allocate_protocol(const char* name);
void register_protocol(proto_t* protocol);

selector_t* get_selector(const char* name);
bool responds_to_selector(class_t* c, selector_t* sel);
bool conforms_to_protocol(class_t* c, proto_t* protocol);

obj_t* create_class_instance(class_t* c);
obj_t* create_class_instance(class_t* c, std::size_t extraBytes);
class_t* get_obj_class(obj_t* obj);

inline obj_t* get_class_property(const char* className, const char* propertyName);

void set_obj_pointer_variable(obj_t* obj, const char* name, void* value);
void* get_obj_pointer_variable(obj_t* obj, const char* name);

void set_obj_instance_variable(obj_t* obj, const char* name, const void* data, std::size_t size);
void* get_obj_instance_variable(obj_t* obj, const char* name);

template <typename T>
inline void set_obj_instance_variable(obj_t* obj, const char* name, const T& data);

template <typename T>
inline T* get_obj_instance_variable(obj_t* obj, const char* name);

void* get_obj_indexed_variables(obj_t* obj);

template <typename Type>
inline void set_ivar_pointer(obj_t* obj, const char* name, Type* value);

template <typename Type, std::enable_if_t<std::is_pointer_v<Type>, std::nullptr_t> = nullptr>
inline Type get_ivar_pointer(obj_t* obj, const char* name);

inline void retain(obj_t* obj);

inline unsigned long retain_count(obj_t* obj);

inline void release(obj_t* obj);

inline void reset(obj_t*& obj);

template <typename R = void, typename... Args, typename... Params>
inline obj_t* create_object(const char* classType, const char* initFct, Params&&... params);

//
//
//

template <class _Tp>
using null_to_obj = std::conditional_t<std::is_null_pointer_v<_Tp>, obj_t*, _Tp>;

template <typename R = void, typename... Args>
using method_ptr = R (*)(obj_t*, selector_t*, Args...);

template <typename R = void, typename... Args>
using class_method_ptr = R (*)(class_t*, selector_t*, Args...);

template <typename R = void, typename... Args>
using super_method_ptr = R (*)(super_t*, selector_t*, Args...);

//
//
//

template <typename R = void, typename... Params, typename SelectorType, typename IdType>
inline R call(IdType* optr, SelectorType selector, Params... params);

template <typename R, typename... Params, typename SelectorType>
inline R call(obj_t* obj, SelectorType selector, Params... params);

template <typename R = void, typename... Args, typename... Params, typename SelectorType, typename IdType>
inline R s_call(IdType* optr, SelectorType selector, Params&&... params);

template <typename SelectorType, typename IdType, typename... Params>
class r_calll;

template <typename IdType, typename... ObjType, typename SelectorType>
inline void icall(IdType* optr, SelectorType selector, ObjType... obj_type_ptr);

template <typename R = void, typename... Args, typename... Params>
inline R call_meta(const char* className, const char* selectorName, Params&&... params);

///
template <typename Descriptor>
class class_descriptor {
public:
  class_descriptor(const char* rootName);

  class_descriptor(const class_descriptor&) = delete;

  ~class_descriptor();

  class_descriptor& operator=(const class_descriptor&) = delete;

  obj_t* create_instance() const;

  template <typename ReturnType, typename... Args, typename... Params>
  static ReturnType send_superclass_message(obj_t* obj, const char* selectorName, Params&&... params);

  template <typename Type>
  inline bool add_pointer(const char* name, const char* className);

  template <auto FunctionType>
  inline bool add_method(const char* selectorName, const char* signature);

  template <void (Descriptor::*MemberFunctionPointer)(obj_t*)>
  bool add_notification_method(const char* selectorName);

  inline bool add_protocol(const char* protocolName, bool force = false);

private:
  class_t* m_classObject;

  template <auto FunctionType, typename ReturnType, typename... Args>
  inline bool add_member_method_impl(ReturnType (Descriptor::*)(Args...), selector_t* selector, const char* signature);
};

} // namespace nano::objc.

//
//
//
//
//
namespace nano {
struct cf_object_deleter {
  void operator()(const void*) const noexcept;
};

namespace detail {
  template <class _CFType>
  using cf_unique_ptr_type = std::unique_ptr<std::remove_pointer_t<_CFType>, cf_object_deleter>;
} // namespace detail.

template <class _CFType>
struct cf_unique_ptr : detail::cf_unique_ptr_type<_CFType> {
  using base = detail::cf_unique_ptr_type<_CFType>;

  inline cf_unique_ptr(_CFType ptr)
      : base(ptr) {}
  cf_unique_ptr(cf_unique_ptr&&) = delete;
  cf_unique_ptr& operator=(cf_unique_ptr&&) = delete;

  inline operator _CFType() const { return base::get(); }

  template <class T, std::enable_if_t<std::is_convertible_v<_CFType, T>, std::nullptr_t> = nullptr>
  inline T as() const {
    return static_cast<T>(base::get());
  }
};
} // namespace nano.

//
//
//
//
//
namespace nano::objc {

struct obj_deleter {
  void operator()(obj_t* obj) const noexcept;
};

struct obj_unique_ptr : std::unique_ptr<obj_t, obj_deleter> {
  using base = std::unique_ptr<obj_t, obj_deleter>;

  inline obj_unique_ptr(obj_t* obj)
      : base(obj) {}
  
  obj_unique_ptr(obj_unique_ptr&&) = delete;
  obj_unique_ptr& operator=(obj_unique_ptr&&) = delete;

  inline operator obj_t*() const { return base::get(); }
};

template <typename ReturnType>
inline ReturnType return_default_value() {
  return ReturnType{};
}

template <>
inline void return_default_value<void>() {}

template <typename T, typename... Ts, std::enable_if_t<is_basic_type<T>, std::nullptr_t>>
inline std::string get_encoding() {
  if constexpr (sizeof...(Ts) > 0) {
    return get_encoding<T>() + get_encoding<Ts...>();
  }
  else if constexpr (std::is_same_v<T, void>) {
    return "v";
  }
  else if constexpr (std::is_null_pointer_v<T>) {
    return "*";
  }
  else {
    using type = std::remove_cv_t<T>;

    if constexpr (std::is_same_v<type, bool>) {
      return "B";
    }

    else if constexpr (std::is_integral_v<type>) {
      constexpr const std::size_t size = sizeof(T);
      constexpr const bool is_signed = std::is_signed_v<type>;

      if constexpr (size == 1) {
        return is_signed ? "c" : "C";
      }
      else if constexpr (size == 2) {
        return is_signed ? "s" : "S";
      }
      else if constexpr (size == 4) {
        if constexpr (std::is_same_v<type, long> || std::is_same_v<type, unsigned long>) {
          return is_signed ? "l" : "L";
        }
        else {
          return is_signed ? "i" : "I";
        }
      }
      else if constexpr (size == 8) {
        return is_signed ? "q" : "Q";
      }
      else if constexpr (size == 16) {
        return is_signed ? "t" : "T";
      }
    }

    else if constexpr (std::is_same_v<type, float>) {
      return "f";
    }

    else if constexpr (std::is_same_v<type, double>) {
      return "d";
    }
    else if constexpr (std::is_same_v<type, long double>) {
      return "D";
    }

    else if constexpr (std::is_same_v<type, float*>) {
      return "^f";
    }
    else if constexpr (std::is_same_v<type, double*>) {
      return "^d";
    }

    else if constexpr (std::is_same_v<type, void*>) {
      return "^v";
    }

    else if constexpr (std::is_same_v<type, obj_t*>) {
      return "@";
    }

    else if constexpr (std::is_same_v<type, selector_t*>) {
      return ":";
    }

    else if constexpr (std::is_same_v<type, class_t*>) {
      return "#";
    }
  }

  return "?";
}

template <typename T, typename... Ts, std::enable_if_t<!is_basic_type<T> && std::is_pointer_v<T>, std::nullptr_t>>
inline std::string get_encoding(const char* name) {

  if constexpr (sizeof...(Ts) == 0) {
    return "^" + std::string(name);
  }
  else {
    return "^" + std::string(name) + get_encoding<Ts...>();
  }
}

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_pointer_v<T> && has_name_for_type<std::remove_pointer_t<T>>,
        std::nullptr_t>>
inline std::string get_encoding() {
  if constexpr (sizeof...(Ts) == 0) {
    return "^" + std::string(name_for_type<std::remove_pointer_t<T>>::value);
  }
  else {
    return "^" + std::string(name_for_type<std::remove_pointer_t<T>>::value) + get_encoding<Ts...>();
  }
}

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_class_v<T> && std::is_trivial_v<T>, std::nullptr_t>>
inline std::string get_encoding(const char* name) {
  static_assert(sizeof...(Ts) != 0, "");
  return "{" + std::string(name) + "=" + get_encoding<Ts...>() + "}";
}

template <typename T, typename... Ts,
    std::enable_if_t<!is_basic_type<T> && std::is_class_v<T> && std::is_trivial_v<T> && has_name_for_type<T>,
        std::nullptr_t>>
inline std::string get_encoding() {
  static_assert(sizeof...(Ts) != 0, "");
  return "{" + std::string(name_for_type<T>::value) + "=" + get_encoding<Ts...>() + "}";
}

template <typename T>
bool add_class_variable(class_t* c, const char* name, const char* encoding) {
  return add_class_variable(c, name, encoding, sizeof(T), alignof(T));
}

template <typename T>
void set_obj_instance_variable(obj_t* obj, const char* name, const T& data) {
  set_obj_instance_variable(obj, name, static_cast<const void*>(&data), sizeof(T));
}

template <typename T>
T* get_obj_instance_variable(obj_t* obj, const char* name) {
  return static_cast<T*>(get_obj_instance_variable(obj, name));
}

template <typename R, typename... Params, typename SelectorType, typename IdType>
R call(IdType* optr, SelectorType selector, Params... params) {

  static_assert(std::is_same_v<SelectorType, selector_t*> || std::is_constructible_v<std::string_view, SelectorType>, "");

  obj_t* obj = reinterpret_cast<obj_t*>(optr);

  selector_t* sel = [](SelectorType s) {
    if constexpr (std::is_same_v<SelectorType, selector_t*>) {
      return s;
    }
    else {
      return get_selector(s);
    }
  }(std::forward<SelectorType>(selector));

  imp_ptr fctImpl = get_class_method_implementation(get_obj_class(obj), sel);
  return reinterpret_cast<method_ptr<R, null_to_obj<Params>...>>(fctImpl)(obj, sel, params...);
}

template <typename R, typename... Params, typename SelectorType>
R call(obj_t* obj, SelectorType selector, Params... params) {
  static_assert(std::is_same_v<SelectorType, selector_t*> || std::is_constructible_v<std::string_view, SelectorType>, "");

  selector_t* sel = [](SelectorType s) {
    if constexpr (std::is_same_v<SelectorType, selector_t*>) {
      return s;
    }
    else {
      return get_selector(s);
    }
  }(std::forward<SelectorType>(selector));

  imp_ptr fctImpl = get_class_method_implementation(get_obj_class(obj), sel);
  return reinterpret_cast<method_ptr<R, null_to_obj<Params>...>>(fctImpl)(obj, sel, params...);
}

template <typename SelectorType, typename IdType, typename... Params>
class r_call {
public:
  inline r_call(IdType* optr, SelectorType selector, Params... params)
      : _params(optr, selector, params...) {}

  template <typename R>
  inline operator R() const {
    return std::apply([](IdType* optr, SelectorType selector,
                          Params... args) { return call<R, null_to_obj<Params>...>(optr, selector, args...); },
        _params);
  }

private:
  std::tuple<IdType*, SelectorType, Params...> _params;
};

template <typename SelectorType, typename IdType, typename... Params>
r_call(IdType* optr, SelectorType selector, Params... params) -> r_call<SelectorType, IdType, Params...>;

template <typename SelectorType, typename... Params>
r_call(obj_unique_ptr optr, SelectorType selector, Params... params) -> r_call<SelectorType, obj_t, Params...>;

template <typename R, typename... Args, typename... Params, typename SelectorType, typename IdType>
R s_call(IdType* optr, SelectorType selector, Params&&... params) {

  static_assert(std::is_same_v<SelectorType, selector_t*> || std::is_constructible_v<std::string_view, SelectorType>, "");

  obj_t* obj = reinterpret_cast<obj_t*>(optr);

  selector_t* sel = [](SelectorType s) {
    if constexpr (std::is_same_v<SelectorType, selector_t*>) {
      return s;
    }
    else {
      return get_selector(s);
    }
  }(std::forward<SelectorType>(selector));

  imp_ptr fctImpl = get_class_method_implementation(get_obj_class(obj), sel);
  return reinterpret_cast<method_ptr<R, Args...>>(fctImpl)(obj, sel, std::forward<Params>(params)...);
}

template <typename R, typename... Args, typename... Params>
R call_meta(const char* className, const char* selectorName, Params&&... params) {
  class_t* objClass = get_class(className);
  class_t* meta = get_meta_class(className);
  selector_t* selector = get_selector(selectorName);
  imp_ptr fctImpl = get_class_method_implementation(meta, selector);
  return reinterpret_cast<class_method_ptr<R, Args...>>(fctImpl)(objClass, selector, std::forward<Params>(params)...);
}

template <typename IdType, typename... ObjType, typename SelectorType>
void icall(IdType* optr, SelectorType selector, ObjType... obj_type_ptr) {
  static_assert(sizeof...(ObjType) < 2, "obj_type_ptr must either be an objc id or nullptr");
  if constexpr (sizeof...(ObjType)) {
    return call<void, obj_t*>(reinterpret_cast<obj_t*>(optr), selector, reinterpret_cast<obj_t*>(obj_type_ptr)...);
  }
  else {
    return call<void, obj_t*>(reinterpret_cast<obj_t*>(optr), selector, nullptr);
  }
}

template <typename R, typename... Args, typename... Params>
obj_t* create_object(const char* classType, const char* initFct, Params&&... params) {
  obj_t* obj = create_class_instance(get_class(classType));
  nano::objc::call<R, Args...>(obj, initFct, std::forward<Params>(params)...);
  return obj;
}

obj_t* get_class_property(const char* className, const char* propertyName) {
  return call_meta<obj_t*>(className, propertyName);
}

template <typename Type>
void set_ivar_pointer(obj_t* obj, const char* name, Type* value) {
  set_obj_pointer_variable(obj, name, static_cast<void*>(value));
}

template <typename Type, std::enable_if_t<std::is_pointer_v<Type>, std::nullptr_t>>
Type get_ivar_pointer(obj_t* obj, const char* name) {
  return static_cast<Type>(get_obj_pointer_variable(obj, name));
}

void retain(obj_t* obj) { call(obj, "retain"); }

unsigned long retain_count(obj_t* obj) { return call<unsigned long>(obj, "retainCount"); }

void release(obj_t* obj) { call(obj, "release"); }

void reset(obj_t*& obj) {
  call(obj, "release");
  obj = nullptr;
}

std::string generate_random_alphanum_string(std::size_t length);

NANO_CLANG_PUSH_WARNING("-Wold-style-cast")

template <typename Descriptor>
class_descriptor<Descriptor>::class_descriptor(const char* rootName)
    : m_classObject(
        allocate_class(get_class(Descriptor::baseName), (rootName + generate_random_alphanum_string(10)).c_str())) {

  if (!add_pointer<Descriptor>(Descriptor::valueName, Descriptor::className)) {
    std::cout << "ERROR" << std::endl;
    return;
  }

  /**
   * Creates a new class and metaclass.
   *
   * @param superclass The class to use as the new class's superclass, or \c Nil to create a new root class.
   * @param name The string to use as the new class's name. The string will be copied.
   * @param extraBytes The number of bytes to allocate for indexed ivars at the end of
   *  the class and metaclass objects. This should usually be \c 0.
   *
   * @return The new class, or Nil if the class could not be created (for example, the desired name is already in
   * use).
   *
   * @note You can get a pointer to the new metaclass by calling \c object_getClass(newClass).
   * @note To create a new class, start by calling \c objc_allocateClassPair.
   *  Then set the class's attributes with functions like \c class_addMethod and \c class_addIvar.
   *  When you are done building the class, call \c objc_registerClassPair. The new class is now ready for use.
   * @note Instance methods and instance variables should be added to the class itself.
   *  Class methods should be added to the metaclass.
   */
  //        OBJC_EXPORT Class _Nullable
  //        objc_allocateClassPair(Class _Nullable superclass, const char * _Nonnull name,
  //                               size_t extraBytes)

  // TODO: Should not be here.
  register_class(m_classObject);
}

template <typename Descriptor>
class_descriptor<Descriptor>::~class_descriptor() {
  std::string kvoSubclassName = std::string("NSKVONotifying_") + get_class_name(m_classObject);

  if (get_class(kvoSubclassName.c_str()) == nullptr) {
    dispose_class(m_classObject);
  }
}

template <typename Descriptor>
obj_t* class_descriptor<Descriptor>::create_instance() const {
  return create_class_instance(m_classObject);
}

template <typename Descriptor>
template <typename ReturnType, typename... Args, typename... Params>
ReturnType class_descriptor<Descriptor>::send_superclass_message(
    obj_t* obj, const char* selectorName, Params&&... params) {

  if (class_t* objClass = get_class(Descriptor::baseName)) {
    using FctType = super_method_ptr<ReturnType, Args...>;

    std::pair<obj_t*, class_t*> s = { obj, objClass };
    return reinterpret_cast<FctType>(send_super_fct)(&s, get_selector(selectorName), std::forward<Params>(params)...);
  }

  //    assert(false"Could not create objc class");
  return return_default_value<ReturnType>();
}

template <typename Descriptor>
template <typename Type>
inline bool class_descriptor<Descriptor>::add_pointer(const char* name, const char* className) {
  return add_class_pointer(m_classObject, name, className, sizeof(Type*), alignof(Type*));
}

template <typename Descriptor>
template <auto FunctionType>
inline bool class_descriptor<Descriptor>::add_method(const char* selectorName, const char* signature) {
  selector_t* selector = get_selector(selectorName);

  if constexpr (std::is_member_function_pointer_v<decltype(FunctionType)>) {
    return add_member_method_impl<FunctionType>(FunctionType, selector, signature);
  }
  else {
    return add_class_method(m_classObject, selector, (imp_ptr)FunctionType, signature);
  }
}

template <typename Descriptor>
template <void (Descriptor::*MemberFunctionPointer)(obj_t*)>
bool class_descriptor<Descriptor>::add_notification_method(const char* selectorName) {
  return add_class_method(
      m_classObject, get_selector(selectorName),
      (imp_ptr)(method_ptr<void, obj_t*>)[](obj_t* obj, selector_t*, obj_t* notification) {
        if (auto* p = nano::objc::get_ivar_pointer<Descriptor*>(obj, Descriptor::valueName)) {
          (p->*MemberFunctionPointer)(notification);
        }
      },
      "v@:@");
}

template <typename Descriptor>
inline bool class_descriptor<Descriptor>::add_protocol(const char* protocolName, bool force) {

  if (proto_t* protocol = get_protocol(protocolName)) {
    return nano::objc::add_protocol(m_classObject, protocol);
  }

  if (!force) {
    return false;
  }

  // Force protocol allocation.
  if (proto_t* protocol = allocate_protocol(protocolName)) {
    register_protocol(protocol);
    return nano::objc::add_protocol(m_classObject, protocol);
  }

  return false;
}

template <typename Descriptor>
template <auto FunctionType, typename ReturnType, typename... Args>
inline bool class_descriptor<Descriptor>::add_member_method_impl(
    ReturnType (Descriptor::*)(Args...), selector_t* selector, const char* signature) {
  return add_class_method(
      m_classObject, selector,
      (imp_ptr)(method_ptr<ReturnType, Args...>)[](obj_t* obj, selector_t*, Args... args) {
        auto* p = nano::objc::get_ivar_pointer<Descriptor*>(obj, Descriptor::valueName);
        return p ? (p->*FunctionType)(args...) : return_default_value<ReturnType>();
      },
      signature);
}

NANO_CLANG_POP_WARNING()

} // namespace nano::objc.

NANO_CLANG_DIAGNOSTIC_POP()
