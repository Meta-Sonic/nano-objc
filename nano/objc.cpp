#include "nano/objc.h"
#include <CoreFoundation/CoreFoundation.h>
#include <objc/message.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <random>



namespace nano {
void cf_object_deleter::operator()(const void* obj) const noexcept { CFRelease(obj); }
} // namespace nano.

//
//
namespace nano::objc {

template <class _CFType>
using cf_ptr = cf_unique_ptr<_CFType>;

send_super_ptr send_super_fct = &objc_msgSendSuper;

proto_t* get_protocol(const char* name) { return objc_getProtocol(name); }

bool add_protocol(class_t* c, proto_t* protocol) { return class_addProtocol(c, protocol); }

void register_protocol(proto_t* protocol) { objc_registerProtocol(protocol); }

proto_t* allocate_protocol(const char* name) { return objc_allocateProtocol(name); }

class_t* allocate_class(class_t* super, const char* name) { return objc_allocateClassPair(super, name, 0); }

void register_class(class_t* c) { objc_registerClassPair(c); }

void dispose_class(class_t* c) { objc_disposeClassPair(c); }

const char* get_class_name(class_t* c) { return class_getName(c); }


bool responds_to_selector(class_t* c, selector_t* sel) {
  return class_respondsToSelector(c, sel); 
}

bool conforms_to_protocol(class_t* c, proto_t* protocol) {
  return class_conformsToProtocol(c, protocol);
}

obj_t* create_class_instance(class_t* c) { return class_createInstance(c, 0); }

obj_t* create_class_instance(class_t* c, std::size_t extraBytes) { return class_createInstance(c, extraBytes); }

bool add_class_pointer(class_t* c, const char* name, const char* className, std::size_t size, std::size_t align) {
  std::string enc = "^{" + std::string(className) + "=}";
  return class_addIvar(c, name, size, align, enc.c_str());
}

bool add_class_variable(class_t* c, const char* name, const char* encoding, std::size_t size, std::size_t align) {
  return class_addIvar(c, name, size, align, encoding);
}

class_t* get_class(const char* name) { return objc_getClass(name); }

class_t* get_meta_class(const char* name) { return objc_getMetaClass(name); }

selector_t* get_selector(const char* name) { return sel_registerName(name); }

imp_ptr get_class_method_implementation(class_t* c, selector_t* s) { return class_getMethodImplementation(c, s); }

bool add_class_method(class_t* c, selector_t* s, imp_ptr imp, const char* types) {
  return class_addMethod(c, s, imp, types);
}

class_t* get_obj_class(obj_t* obj) { return object_getClass(obj); }

void set_obj_pointer_variable(obj_t* obj, const char* name, void* value) {
  object_setInstanceVariable(obj, name, value);
}

void* get_obj_pointer_variable(obj_t* obj, const char* name) {
  void* v = nullptr;
  object_getInstanceVariable(obj, name, &v);
  return v;
}

// https://gist.github.com/mikeash/7603035
void* get_obj_instance_variable(obj_t* obj, const char* name) {
  class_t* c = get_obj_class(obj);
  assert(c);
  if (!c) {
    return nullptr;
  }

  ivar_t* iv = class_getInstanceVariable(c, name);
  assert(iv);
  if (!iv) {
    return nullptr;
  }

  return obj + ivar_getOffset(iv);
}

void set_obj_instance_variable(obj_t* obj, const char* name, const void* data, std::size_t size) {
  void* buffer = get_obj_instance_variable(obj, name);
  assert(buffer);

  if (!buffer) {
    return;
  }

  std::memcpy(buffer, (const void*)data, size);
}

void* get_obj_indexed_variables(obj_t* obj) { return object_getIndexedIvars(obj); }

void obj_deleter::operator()(obj_t* obj) const noexcept { 
  nano::objc::release(obj); }

std::string generate_random_alphanum_string(std::size_t length) {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";

  static const int alphanumSize = sizeof(alphanum);

  std::string str;
  str.resize(length, 0);

  std::mt19937 rng;
  std::uniform_int_distribution<int> dist(0, alphanumSize - 1);

  for (std::size_t i = 0; i < str.size(); i++) {
    str[i] = alphanum[dist(rng)];
  }

  return str;
}

} // namespace nano::objc.
