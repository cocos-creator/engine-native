#include "cocos/bindings/auto/jsb_cocos_auto.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global.h"
#include "cocos2d.h"

#ifndef JSB_ALLOC
#define JSB_ALLOC(kls, ...) new (std::nothrow) kls(__VA_ARGS__)
#endif

#ifndef JSB_FREE
#define JSB_FREE(ptr) delete ptr
#endif
se::Object* __jsb_cc_FileUtils_proto = nullptr;
se::Class* __jsb_cc_FileUtils_class = nullptr;

static bool js_engine_FileUtils_writeDataToFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_writeDataToFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        cc::Data arg0;
        std::string arg1;
        ok &= seval_to_Data(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeDataToFile : Error processing arguments");
        bool result = cobj->writeDataToFile(arg0, arg1);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeDataToFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_writeDataToFile)

static bool js_engine_FileUtils_getStringFromFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getStringFromFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getStringFromFile : Error processing arguments");
        std::string result = cobj->getStringFromFile(arg0);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getStringFromFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getStringFromFile)

static bool js_engine_FileUtils_removeFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_removeFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_removeFile : Error processing arguments");
        bool result = cobj->removeFile(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_removeFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_removeFile)

static bool js_engine_FileUtils_getDataFromFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getDataFromFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getDataFromFile : Error processing arguments");
        cc::Data result = cobj->getDataFromFile(arg0);
        ok &= Data_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getDataFromFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getDataFromFile)

static bool js_engine_FileUtils_isAbsolutePath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_isAbsolutePath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_isAbsolutePath : Error processing arguments");
        bool result = cobj->isAbsolutePath(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_isAbsolutePath : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_isAbsolutePath)

static bool js_engine_FileUtils_renameFile(se::State& s)
{
    CC_UNUSED bool ok = true;
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2( cobj, false, "js_engine_FileUtils_renameFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    do {
        if (argc == 2) {
            std::string arg0;
            ok &= seval_to_std_string(args[0], &arg0);            
            if (!ok) { ok = true; break; }
            std::string arg1;
            ok &= seval_to_std_string(args[1], &arg1);            
            if (!ok) { ok = true; break; }
            bool result = cobj->renameFile(arg0, arg1);
            ok &= boolean_to_seval(result, &s.rval());
            SE_PRECONDITION2(ok, false, "js_engine_FileUtils_renameFile : Error processing arguments");
            return true;
        }
    } while(false);

    do {
        if (argc == 3) {
            std::string arg0;
            ok &= seval_to_std_string(args[0], &arg0);            
            if (!ok) { ok = true; break; }
            std::string arg1;
            ok &= seval_to_std_string(args[1], &arg1);            
            if (!ok) { ok = true; break; }
            std::string arg2;
            ok &= seval_to_std_string(args[2], &arg2);            
            if (!ok) { ok = true; break; }
            bool result = cobj->renameFile(arg0, arg1, arg2);
            ok &= boolean_to_seval(result, &s.rval());
            SE_PRECONDITION2(ok, false, "js_engine_FileUtils_renameFile : Error processing arguments");
            return true;
        }
    } while(false);

    SE_REPORT_ERROR("wrong number of arguments: %d", (int)argc);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_renameFile)

static bool js_engine_FileUtils_normalizePath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_normalizePath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_normalizePath : Error processing arguments");
        std::string result = cobj->normalizePath(arg0);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_normalizePath : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_normalizePath)

static bool js_engine_FileUtils_getDefaultResourceRootPath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getDefaultResourceRootPath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        const std::string& result = cobj->getDefaultResourceRootPath();
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getDefaultResourceRootPath : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getDefaultResourceRootPath)

static bool js_engine_FileUtils_getValueMapFromData(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getValueMapFromData : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
        do { int32_t tmp = 0; ok &= seval_to_int32(args[1], &tmp); arg1 = (int)tmp; } while(false);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getValueMapFromData : Error processing arguments");
        cc::ValueMap result = cobj->getValueMapFromData(arg0, arg1);
        ok &= ccvaluemap_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getValueMapFromData : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getValueMapFromData)

static bool js_engine_FileUtils_getValueVectorFromFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getValueVectorFromFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getValueVectorFromFile : Error processing arguments");
        cc::ValueVector result = cobj->getValueVectorFromFile(arg0);
        ok &= ccvaluevector_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getValueVectorFromFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getValueVectorFromFile)

static bool js_engine_FileUtils_getSearchPaths(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getSearchPaths : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        const std::vector<std::string>& result = cobj->getSearchPaths();
        ok &= std_vector_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getSearchPaths : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getSearchPaths)

static bool js_engine_FileUtils_writeToFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_writeToFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        cc::ValueMap arg0;
        std::string arg1;
        ok &= seval_to_ccvaluemap(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeToFile : Error processing arguments");
        bool result = cobj->writeToFile(arg0, arg1);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeToFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_writeToFile)

static bool js_engine_FileUtils_getOriginalSearchPaths(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getOriginalSearchPaths : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        const std::vector<std::string>& result = cobj->getOriginalSearchPaths();
        ok &= std_vector_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getOriginalSearchPaths : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getOriginalSearchPaths)

static bool js_engine_FileUtils_listFiles(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_listFiles : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_listFiles : Error processing arguments");
        std::vector<std::string> result = cobj->listFiles(arg0);
        ok &= std_vector_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_listFiles : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_listFiles)

static bool js_engine_FileUtils_getValueMapFromFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getValueMapFromFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getValueMapFromFile : Error processing arguments");
        cc::ValueMap result = cobj->getValueMapFromFile(arg0);
        ok &= ccvaluemap_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getValueMapFromFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getValueMapFromFile)

static bool js_engine_FileUtils_getFileSize(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getFileSize : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getFileSize : Error processing arguments");
        long result = cobj->getFileSize(arg0);
        ok &= long_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getFileSize : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getFileSize)

static bool js_engine_FileUtils_getFileDir(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getFileDir : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getFileDir : Error processing arguments");
        std::string result = cobj->getFileDir(arg0);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getFileDir : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getFileDir)

static bool js_engine_FileUtils_removeDirectory(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_removeDirectory : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_removeDirectory : Error processing arguments");
        bool result = cobj->removeDirectory(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_removeDirectory : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_removeDirectory)

static bool js_engine_FileUtils_setSearchPaths(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_setSearchPaths : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::vector<std::string> arg0;
        ok &= seval_to_std_vector(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_setSearchPaths : Error processing arguments");
        cobj->setSearchPaths(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_setSearchPaths)

static bool js_engine_FileUtils_writeStringToFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_writeStringToFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        std::string arg0;
        std::string arg1;
        ok &= seval_to_std_string(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeStringToFile : Error processing arguments");
        bool result = cobj->writeStringToFile(arg0, arg1);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeStringToFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_writeStringToFile)

static bool js_engine_FileUtils_fullPathForFilename(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_fullPathForFilename : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_fullPathForFilename : Error processing arguments");
        std::string result = cobj->fullPathForFilename(arg0);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_fullPathForFilename : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_fullPathForFilename)

static bool js_engine_FileUtils_addSearchPath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_addSearchPath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_addSearchPath : Error processing arguments");
        cobj->addSearchPath(arg0);
        return true;
    }
    if (argc == 2) {
        std::string arg0;
        bool arg1;
        ok &= seval_to_std_string(args[0], &arg0);
        ok &= seval_to_boolean(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_addSearchPath : Error processing arguments");
        cobj->addSearchPath(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_addSearchPath)

static bool js_engine_FileUtils_writeValueVectorToFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_writeValueVectorToFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        cc::ValueVector arg0;
        std::string arg1;
        ok &= seval_to_ccvaluevector(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeValueVectorToFile : Error processing arguments");
        bool result = cobj->writeValueVectorToFile(arg0, arg1);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeValueVectorToFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_writeValueVectorToFile)

static bool js_engine_FileUtils_isFileExist(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_isFileExist : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_isFileExist : Error processing arguments");
        bool result = cobj->isFileExist(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_isFileExist : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_isFileExist)

static bool js_engine_FileUtils_purgeCachedEntries(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_purgeCachedEntries : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->purgeCachedEntries();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_purgeCachedEntries)

static bool js_engine_FileUtils_fullPathFromRelativeFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_fullPathFromRelativeFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        std::string arg0;
        std::string arg1;
        ok &= seval_to_std_string(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_fullPathFromRelativeFile : Error processing arguments");
        std::string result = cobj->fullPathFromRelativeFile(arg0, arg1);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_fullPathFromRelativeFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_fullPathFromRelativeFile)

static bool js_engine_FileUtils_getSuitableFOpen(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getSuitableFOpen : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getSuitableFOpen : Error processing arguments");
        std::string result = cobj->getSuitableFOpen(arg0);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getSuitableFOpen : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getSuitableFOpen)

static bool js_engine_FileUtils_writeValueMapToFile(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_writeValueMapToFile : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        cc::ValueMap arg0;
        std::string arg1;
        ok &= seval_to_ccvaluemap(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeValueMapToFile : Error processing arguments");
        bool result = cobj->writeValueMapToFile(arg0, arg1);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_writeValueMapToFile : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_writeValueMapToFile)

static bool js_engine_FileUtils_getFileExtension(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getFileExtension : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getFileExtension : Error processing arguments");
        std::string result = cobj->getFileExtension(arg0);
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getFileExtension : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getFileExtension)

static bool js_engine_FileUtils_setWritablePath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_setWritablePath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_setWritablePath : Error processing arguments");
        cobj->setWritablePath(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_setWritablePath)

static bool js_engine_FileUtils_isDirectoryExist(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_isDirectoryExist : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_isDirectoryExist : Error processing arguments");
        bool result = cobj->isDirectoryExist(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_isDirectoryExist : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_isDirectoryExist)

static bool js_engine_FileUtils_setDefaultResourceRootPath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_setDefaultResourceRootPath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_setDefaultResourceRootPath : Error processing arguments");
        cobj->setDefaultResourceRootPath(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_setDefaultResourceRootPath)

static bool js_engine_FileUtils_createDirectory(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_createDirectory : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_createDirectory : Error processing arguments");
        bool result = cobj->createDirectory(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_createDirectory : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_createDirectory)

static bool js_engine_FileUtils_getWritablePath(se::State& s)
{
    cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_FileUtils_getWritablePath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        std::string result = cobj->getWritablePath();
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getWritablePath : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getWritablePath)

static bool js_engine_FileUtils_setDelegate(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        cc::FileUtils* arg0 = nullptr;
        ok &= seval_to_native_ptr(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_setDelegate : Error processing arguments");
        cc::FileUtils::setDelegate(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_setDelegate)

static bool js_engine_FileUtils_getInstance(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cc::FileUtils* result = cc::FileUtils::getInstance();
        ok &= native_ptr_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_FileUtils_getInstance : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_FileUtils_getInstance)



static bool js_cc_FileUtils_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(s.nativeThisObject());
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        cc::FileUtils* cobj = (cc::FileUtils*)s.nativeThisObject();
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_FileUtils_finalize)

bool js_register_engine_FileUtils(se::Object* obj)
{
    auto cls = se::Class::create("FileUtils", obj, nullptr, nullptr);

    cls->defineFunction("writeDataToFile", _SE(js_engine_FileUtils_writeDataToFile));
    cls->defineFunction("getStringFromFile", _SE(js_engine_FileUtils_getStringFromFile));
    cls->defineFunction("removeFile", _SE(js_engine_FileUtils_removeFile));
    cls->defineFunction("getDataFromFile", _SE(js_engine_FileUtils_getDataFromFile));
    cls->defineFunction("isAbsolutePath", _SE(js_engine_FileUtils_isAbsolutePath));
    cls->defineFunction("renameFile", _SE(js_engine_FileUtils_renameFile));
    cls->defineFunction("normalizePath", _SE(js_engine_FileUtils_normalizePath));
    cls->defineFunction("getDefaultResourceRootPath", _SE(js_engine_FileUtils_getDefaultResourceRootPath));
    cls->defineFunction("getValueMapFromData", _SE(js_engine_FileUtils_getValueMapFromData));
    cls->defineFunction("getValueVectorFromFile", _SE(js_engine_FileUtils_getValueVectorFromFile));
    cls->defineFunction("getSearchPaths", _SE(js_engine_FileUtils_getSearchPaths));
    cls->defineFunction("writeToFile", _SE(js_engine_FileUtils_writeToFile));
    cls->defineFunction("getOriginalSearchPaths", _SE(js_engine_FileUtils_getOriginalSearchPaths));
    cls->defineFunction("listFiles", _SE(js_engine_FileUtils_listFiles));
    cls->defineFunction("getValueMapFromFile", _SE(js_engine_FileUtils_getValueMapFromFile));
    cls->defineFunction("getFileSize", _SE(js_engine_FileUtils_getFileSize));
    cls->defineFunction("getFileDir", _SE(js_engine_FileUtils_getFileDir));
    cls->defineFunction("removeDirectory", _SE(js_engine_FileUtils_removeDirectory));
    cls->defineFunction("setSearchPaths", _SE(js_engine_FileUtils_setSearchPaths));
    cls->defineFunction("writeStringToFile", _SE(js_engine_FileUtils_writeStringToFile));
    cls->defineFunction("fullPathForFilename", _SE(js_engine_FileUtils_fullPathForFilename));
    cls->defineFunction("addSearchPath", _SE(js_engine_FileUtils_addSearchPath));
    cls->defineFunction("writeValueVectorToFile", _SE(js_engine_FileUtils_writeValueVectorToFile));
    cls->defineFunction("isFileExist", _SE(js_engine_FileUtils_isFileExist));
    cls->defineFunction("purgeCachedEntries", _SE(js_engine_FileUtils_purgeCachedEntries));
    cls->defineFunction("fullPathFromRelativeFile", _SE(js_engine_FileUtils_fullPathFromRelativeFile));
    cls->defineFunction("getSuitableFOpen", _SE(js_engine_FileUtils_getSuitableFOpen));
    cls->defineFunction("writeValueMapToFile", _SE(js_engine_FileUtils_writeValueMapToFile));
    cls->defineFunction("getFileExtension", _SE(js_engine_FileUtils_getFileExtension));
    cls->defineFunction("setWritablePath", _SE(js_engine_FileUtils_setWritablePath));
    cls->defineFunction("isDirectoryExist", _SE(js_engine_FileUtils_isDirectoryExist));
    cls->defineFunction("setDefaultResourceRootPath", _SE(js_engine_FileUtils_setDefaultResourceRootPath));
    cls->defineFunction("createDirectory", _SE(js_engine_FileUtils_createDirectory));
    cls->defineFunction("getWritablePath", _SE(js_engine_FileUtils_getWritablePath));
    cls->defineStaticFunction("setDelegate", _SE(js_engine_FileUtils_setDelegate));
    cls->defineStaticFunction("getInstance", _SE(js_engine_FileUtils_getInstance));
    cls->defineFinalizeFunction(_SE(js_cc_FileUtils_finalize));
    cls->install();
    JSBClassType::registerClass<cc::FileUtils>(cls);

    __jsb_cc_FileUtils_proto = cls->getProto();
    __jsb_cc_FileUtils_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

se::Object* __jsb_cc_Device_proto = nullptr;
se::Class* __jsb_cc_Device_class = nullptr;

static bool js_engine_Device_getDevicePixelRatio(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        float result = cc::Device::getDevicePixelRatio();
        ok &= float_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getDevicePixelRatio : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getDevicePixelRatio)

static bool js_engine_Device_setAccelerometerEnabled(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        bool arg0;
        ok &= seval_to_boolean(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_Device_setAccelerometerEnabled : Error processing arguments");
        cc::Device::setAccelerometerEnabled(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_Device_setAccelerometerEnabled)

static bool js_engine_Device_setAccelerometerInterval(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        float arg0 = 0;
        ok &= seval_to_float(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_Device_setAccelerometerInterval : Error processing arguments");
        cc::Device::setAccelerometerInterval(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_Device_setAccelerometerInterval)

static bool js_engine_Device_vibrate(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        float arg0 = 0;
        ok &= seval_to_float(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_Device_vibrate : Error processing arguments");
        cc::Device::vibrate(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_Device_vibrate)

static bool js_engine_Device_setKeepScreenOn(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        bool arg0;
        ok &= seval_to_boolean(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_Device_setKeepScreenOn : Error processing arguments");
        cc::Device::setKeepScreenOn(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_Device_setKeepScreenOn)

static bool js_engine_Device_getNetworkType(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        int result = (int)cc::Device::getNetworkType();
        ok &= int32_to_seval((int)result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getNetworkType : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getNetworkType)

static bool js_engine_Device_getBatteryLevel(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        float result = cc::Device::getBatteryLevel();
        ok &= float_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getBatteryLevel : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getBatteryLevel)

static bool js_engine_Device_getDeviceRotation(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        int result = (int)cc::Device::getDeviceRotation();
        ok &= int32_to_seval((int)result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getDeviceRotation : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getDeviceRotation)

static bool js_engine_Device_getDPI(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        int result = cc::Device::getDPI();
        ok &= int32_to_seval((int)result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getDPI : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getDPI)

static bool js_engine_Device_getSafeAreaEdge(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cc::Vec4 result = cc::Device::getSafeAreaEdge();
        ok &= Vec4_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getSafeAreaEdge : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getSafeAreaEdge)

static bool js_engine_Device_getDeviceModel(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        std::string result = cc::Device::getDeviceModel();
        ok &= std_string_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_Device_getDeviceModel : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_Device_getDeviceModel)




bool js_register_engine_Device(se::Object* obj)
{
    auto cls = se::Class::create("Device", obj, nullptr, nullptr);

    cls->defineStaticFunction("getDevicePixelRatio", _SE(js_engine_Device_getDevicePixelRatio));
    cls->defineStaticFunction("setAccelerometerEnabled", _SE(js_engine_Device_setAccelerometerEnabled));
    cls->defineStaticFunction("setAccelerometerInterval", _SE(js_engine_Device_setAccelerometerInterval));
    cls->defineStaticFunction("vibrate", _SE(js_engine_Device_vibrate));
    cls->defineStaticFunction("setKeepScreenOn", _SE(js_engine_Device_setKeepScreenOn));
    cls->defineStaticFunction("getNetworkType", _SE(js_engine_Device_getNetworkType));
    cls->defineStaticFunction("getBatteryLevel", _SE(js_engine_Device_getBatteryLevel));
    cls->defineStaticFunction("getDeviceRotation", _SE(js_engine_Device_getDeviceRotation));
    cls->defineStaticFunction("getDPI", _SE(js_engine_Device_getDPI));
    cls->defineStaticFunction("getSafeAreaEdge", _SE(js_engine_Device_getSafeAreaEdge));
    cls->defineStaticFunction("getDeviceModel", _SE(js_engine_Device_getDeviceModel));
    cls->install();
    JSBClassType::registerClass<cc::Device>(cls);

    __jsb_cc_Device_proto = cls->getProto();
    __jsb_cc_Device_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

se::Object* __jsb_cc_SAXParser_proto = nullptr;
se::Class* __jsb_cc_SAXParser_class = nullptr;

static bool js_engine_SAXParser_init(se::State& s)
{
    cc::SAXParser* cobj = (cc::SAXParser*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_SAXParser_init : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
        SE_PRECONDITION2(ok, false, "js_engine_SAXParser_init : Error processing arguments");
        bool result = cobj->init(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_SAXParser_init : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_SAXParser_init)




bool js_register_engine_SAXParser(se::Object* obj)
{
    auto cls = se::Class::create("PlistParser", obj, nullptr, nullptr);

    cls->defineFunction("init", _SE(js_engine_SAXParser_init));
    cls->install();
    JSBClassType::registerClass<cc::SAXParser>(cls);

    __jsb_cc_SAXParser_proto = cls->getProto();
    __jsb_cc_SAXParser_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

se::Object* __jsb_cc_CanvasGradient_proto = nullptr;
se::Class* __jsb_cc_CanvasGradient_class = nullptr;

static bool js_engine_CanvasGradient_addColorStop(se::State& s)
{
    cc::CanvasGradient* cobj = (cc::CanvasGradient*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasGradient_addColorStop : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        float arg0 = 0;
        std::string arg1;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_std_string(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasGradient_addColorStop : Error processing arguments");
        cobj->addColorStop(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasGradient_addColorStop)

SE_DECLARE_FINALIZE_FUNC(js_cc_CanvasGradient_finalize)

static bool js_engine_CanvasGradient_constructor(se::State& s)
{
    cc::CanvasGradient* cobj = JSB_ALLOC(cc::CanvasGradient);
    s.thisObject()->setPrivateData(cobj);
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
    return true;
}
SE_BIND_CTOR(js_engine_CanvasGradient_constructor, __jsb_cc_CanvasGradient_class, js_cc_CanvasGradient_finalize)




static bool js_cc_CanvasGradient_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(s.nativeThisObject());
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        cc::CanvasGradient* cobj = (cc::CanvasGradient*)s.nativeThisObject();
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_CanvasGradient_finalize)

bool js_register_engine_CanvasGradient(se::Object* obj)
{
    auto cls = se::Class::create("CanvasGradient", obj, nullptr, _SE(js_engine_CanvasGradient_constructor));

    cls->defineFunction("addColorStop", _SE(js_engine_CanvasGradient_addColorStop));
    cls->defineFinalizeFunction(_SE(js_cc_CanvasGradient_finalize));
    cls->install();
    JSBClassType::registerClass<cc::CanvasGradient>(cls);

    __jsb_cc_CanvasGradient_proto = cls->getProto();
    __jsb_cc_CanvasGradient_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

se::Object* __jsb_cc_CanvasRenderingContext2D_proto = nullptr;
se::Class* __jsb_cc_CanvasRenderingContext2D_class = nullptr;

static bool js_engine_CanvasRenderingContext2D_restore(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_restore : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->restore();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_restore)

static bool js_engine_CanvasRenderingContext2D_moveTo(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_moveTo : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        float arg0 = 0;
        float arg1 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_moveTo : Error processing arguments");
        cobj->moveTo(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_moveTo)

static bool js_engine_CanvasRenderingContext2D_lineTo(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_lineTo : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        float arg0 = 0;
        float arg1 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_lineTo : Error processing arguments");
        cobj->lineTo(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_lineTo)

static bool js_engine_CanvasRenderingContext2D_setTransform(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_setTransform : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 6) {
        float arg0 = 0;
        float arg1 = 0;
        float arg2 = 0;
        float arg3 = 0;
        float arg4 = 0;
        float arg5 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        ok &= seval_to_float(args[3], &arg3);
        ok &= seval_to_float(args[4], &arg4);
        ok &= seval_to_float(args[5], &arg5);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_setTransform : Error processing arguments");
        cobj->setTransform(arg0, arg1, arg2, arg3, arg4, arg5);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 6);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_setTransform)

static bool js_engine_CanvasRenderingContext2D_stroke(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_stroke : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->stroke();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_stroke)

static bool js_engine_CanvasRenderingContext2D_fill(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_fill : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->fill();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_fill)

static bool js_engine_CanvasRenderingContext2D__fillImageData(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D__fillImageData : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 5) {
        cc::Data arg0;
        float arg1 = 0;
        float arg2 = 0;
        float arg3 = 0;
        float arg4 = 0;
        ok &= seval_to_Data(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        ok &= seval_to_float(args[3], &arg3);
        ok &= seval_to_float(args[4], &arg4);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D__fillImageData : Error processing arguments");
        cobj->_fillImageData(arg0, arg1, arg2, arg3, arg4);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 5);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D__fillImageData)

static bool js_engine_CanvasRenderingContext2D_scale(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_scale : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        float arg0 = 0;
        float arg1 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_scale : Error processing arguments");
        cobj->scale(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_scale)

static bool js_engine_CanvasRenderingContext2D_transform(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_transform : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 6) {
        float arg0 = 0;
        float arg1 = 0;
        float arg2 = 0;
        float arg3 = 0;
        float arg4 = 0;
        float arg5 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        ok &= seval_to_float(args[3], &arg3);
        ok &= seval_to_float(args[4], &arg4);
        ok &= seval_to_float(args[5], &arg5);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_transform : Error processing arguments");
        cobj->transform(arg0, arg1, arg2, arg3, arg4, arg5);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 6);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_transform)

static bool js_engine_CanvasRenderingContext2D_save(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_save : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->save();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_save)

static bool js_engine_CanvasRenderingContext2D_rotate(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_rotate : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        float arg0 = 0;
        ok &= seval_to_float(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_rotate : Error processing arguments");
        cobj->rotate(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_rotate)

static bool js_engine_CanvasRenderingContext2D_beginPath(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_beginPath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->beginPath();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_beginPath)

static bool js_engine_CanvasRenderingContext2D_clearRect(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_clearRect : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 4) {
        float arg0 = 0;
        float arg1 = 0;
        float arg2 = 0;
        float arg3 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        ok &= seval_to_float(args[3], &arg3);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_clearRect : Error processing arguments");
        cobj->clearRect(arg0, arg1, arg2, arg3);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_clearRect)

static bool js_engine_CanvasRenderingContext2D_rect(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_rect : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 4) {
        float arg0 = 0;
        float arg1 = 0;
        float arg2 = 0;
        float arg3 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        ok &= seval_to_float(args[3], &arg3);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_rect : Error processing arguments");
        cobj->rect(arg0, arg1, arg2, arg3);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_rect)

static bool js_engine_CanvasRenderingContext2D_translate(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_translate : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        float arg0 = 0;
        float arg1 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_translate : Error processing arguments");
        cobj->translate(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_translate)

static bool js_engine_CanvasRenderingContext2D_createLinearGradient(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_createLinearGradient : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 4) {
        float arg0 = 0;
        float arg1 = 0;
        float arg2 = 0;
        float arg3 = 0;
        ok &= seval_to_float(args[0], &arg0);
        ok &= seval_to_float(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        ok &= seval_to_float(args[3], &arg3);
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_createLinearGradient : Error processing arguments");
        cc::CanvasGradient* result = cobj->createLinearGradient(arg0, arg1, arg2, arg3);
        ok &= native_ptr_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_createLinearGradient : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_createLinearGradient)

static bool js_engine_CanvasRenderingContext2D_closePath(se::State& s)
{
    cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
    SE_PRECONDITION2(cobj, false, "js_engine_CanvasRenderingContext2D_closePath : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->closePath();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_engine_CanvasRenderingContext2D_closePath)

SE_DECLARE_FINALIZE_FUNC(js_cc_CanvasRenderingContext2D_finalize)

static bool js_engine_CanvasRenderingContext2D_constructor(se::State& s)
{
    CC_UNUSED bool ok = true;
    const auto& args = s.args();
    float arg0 = 0;
    float arg1 = 0;
    ok &= seval_to_float(args[0], &arg0);
    ok &= seval_to_float(args[1], &arg1);
    SE_PRECONDITION2(ok, false, "js_engine_CanvasRenderingContext2D_constructor : Error processing arguments");
    cc::CanvasRenderingContext2D* cobj = JSB_ALLOC(cc::CanvasRenderingContext2D, arg0, arg1);
    s.thisObject()->setPrivateData(cobj);
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
    return true;
}
SE_BIND_CTOR(js_engine_CanvasRenderingContext2D_constructor, __jsb_cc_CanvasRenderingContext2D_class, js_cc_CanvasRenderingContext2D_finalize)




static bool js_cc_CanvasRenderingContext2D_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(s.nativeThisObject());
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        cc::CanvasRenderingContext2D* cobj = (cc::CanvasRenderingContext2D*)s.nativeThisObject();
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_CanvasRenderingContext2D_finalize)

bool js_register_engine_CanvasRenderingContext2D(se::Object* obj)
{
    auto cls = se::Class::create("CanvasRenderingContext2D", obj, nullptr, _SE(js_engine_CanvasRenderingContext2D_constructor));

    cls->defineFunction("restore", _SE(js_engine_CanvasRenderingContext2D_restore));
    cls->defineFunction("moveTo", _SE(js_engine_CanvasRenderingContext2D_moveTo));
    cls->defineFunction("lineTo", _SE(js_engine_CanvasRenderingContext2D_lineTo));
    cls->defineFunction("setTransform", _SE(js_engine_CanvasRenderingContext2D_setTransform));
    cls->defineFunction("stroke", _SE(js_engine_CanvasRenderingContext2D_stroke));
    cls->defineFunction("fill", _SE(js_engine_CanvasRenderingContext2D_fill));
    cls->defineFunction("_fillImageData", _SE(js_engine_CanvasRenderingContext2D__fillImageData));
    cls->defineFunction("scale", _SE(js_engine_CanvasRenderingContext2D_scale));
    cls->defineFunction("transform", _SE(js_engine_CanvasRenderingContext2D_transform));
    cls->defineFunction("save", _SE(js_engine_CanvasRenderingContext2D_save));
    cls->defineFunction("rotate", _SE(js_engine_CanvasRenderingContext2D_rotate));
    cls->defineFunction("beginPath", _SE(js_engine_CanvasRenderingContext2D_beginPath));
    cls->defineFunction("clearRect", _SE(js_engine_CanvasRenderingContext2D_clearRect));
    cls->defineFunction("rect", _SE(js_engine_CanvasRenderingContext2D_rect));
    cls->defineFunction("translate", _SE(js_engine_CanvasRenderingContext2D_translate));
    cls->defineFunction("createLinearGradient", _SE(js_engine_CanvasRenderingContext2D_createLinearGradient));
    cls->defineFunction("closePath", _SE(js_engine_CanvasRenderingContext2D_closePath));
    cls->defineFinalizeFunction(_SE(js_cc_CanvasRenderingContext2D_finalize));
    cls->install();
    JSBClassType::registerClass<cc::CanvasRenderingContext2D>(cls);

    __jsb_cc_CanvasRenderingContext2D_proto = cls->getProto();
    __jsb_cc_CanvasRenderingContext2D_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

bool register_all_engine(se::Object* obj)
{
    // Get the ns
    se::Value nsVal;
    if (!obj->getProperty("jsb", &nsVal))
    {
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        obj->setProperty("jsb", nsVal);
    }
    se::Object* ns = nsVal.toObject();

    js_register_engine_FileUtils(ns);
    js_register_engine_Device(ns);
    js_register_engine_CanvasGradient(ns);
    js_register_engine_CanvasRenderingContext2D(ns);
    js_register_engine_SAXParser(ns);
    return true;
}
