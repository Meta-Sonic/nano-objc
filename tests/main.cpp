#include <nano/test.h>
#include <nano/objc.h>
#include <fstream>

namespace {
namespace objc = nano::objc;
using id = objc::obj_t*;
using objc::call;
using objc::r_call;

inline const char* to_cstr(id ns_string) {
  // return [ns_string UTF8String];
  return r_call(ns_string, "UTF8String");
}

inline std::string to_stdstr(id ns_string) {
  // return [ns_string UTF8String];
  return call<const char*>(ns_string, "UTF8String");
}

inline id from_cstr(const char* str) {
  // return [NSString stringWithUTF8String:str];
  return objc::call_meta<id, const char*>("NSString", "stringWithUTF8String:", str);
}

TEST_CASE("nano.objc", ObjectiveC, "Call objc function") {
  // NSFileManager* fileManager = [NSFileManager defaultManager];
  // id fileManager = objc::get_class_property("NSFileManager", "defaultManager");
  objc::obj_unique_ptr fileManager = objc::create_object("NSFileManager", "init");

  // NSURL* tempDirectoryUrl = [fileManager temporaryDirectory];
  id tempDirectoryUrl = r_call(fileManager, "temporaryDirectory");

  // NSURL* testDirectoryUrl = [tempDirectoryUrl URLByAppendingPathComponent:comp];
  id testDirectoryUrl = r_call(tempDirectoryUrl, "URLByAppendingPathComponent:", from_cstr("abcdefg"));

  // [fileManager removeItemAtURL:testDirectoryUrl error:nullptr];
  call<bool>(fileManager, "removeItemAtURL:error:", testDirectoryUrl, nullptr);

  // BOOL didCreate = [fileManager createDirectoryAtURL:testDirectoryUrl
  //                        withIntermediateDirectories:false
  //                                         attributes:nullptr
  //                                              error:nullptr];
  objc::selector_t* sel = objc::get_selector("createDirectoryAtURL:withIntermediateDirectories:attributes:error:");
  bool didCreate = r_call(fileManager, sel, testDirectoryUrl, false, nullptr, nullptr);
  EXPECT_TRUE(didCreate);

  id dirPath = r_call(testDirectoryUrl, "path");
  std::ofstream(to_stdstr(dirPath) + "/bingo.txt") << "Bingo";

  // NSArray<NSString*>* fileArray = [fileManager contentsOfDirectoryAtPath:[testDirectoryUrl path] error:nullptr];
  id fileArray = r_call(fileManager, "contentsOfDirectoryAtPath:error:", dirPath, nullptr);

  EXPECT_EQ(call<objc::ns_uint_t>(fileArray, "count"), 1UL);

  EXPECT_STR_EQ("bingo.txt", to_cstr(r_call(fileArray, "objectAtIndex:", 0UL)));
}
} // namespace

NANO_TEST_MAIN()
