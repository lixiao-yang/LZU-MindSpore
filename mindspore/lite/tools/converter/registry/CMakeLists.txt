set(KERNEL_REG_DIR ${TOP_DIR}/mindspore/lite/src/registry)
file(GLOB CONVERT_REG_SRC
        pass_registry.cc
        model_parser_registry.cc
        )
file(GLOB KERNEL_REG_SRC ${KERNEL_REG_DIR}/*.cc)
set(REG_SRC ${CONVERT_REG_SRC}
        ${KERNEL_REG_SRC}
        ${KERNEL_REG_DIR}/../cxx_api/types.cc
        ${KERNEL_REG_DIR}/../cxx_api/tensor/tensor_impl.cc
        ${KERNEL_REG_DIR}/../cxx_api/tensor_utils.cc
        ${KERNEL_REG_DIR}/../ms_tensor.cc
        ${KERNEL_REG_DIR}/../tensor.cc
        ${KERNEL_REG_DIR}/../runtime/inner_allocator.cc
        ${KERNEL_REG_DIR}/../common/string_util.cc
        ${CORE_DIR}/utils/log_adapter.cc
        ${CORE_DIR}/utils/status.cc
        ${CORE_DIR}/gvar/log_adapter_common.cc
        ${CORE_DIR}/gvar/logging_level.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../dump_graph.cc)
set_property(SOURCE ${REG_SRC} PROPERTY COMPILE_DEFINITIONS SUBMODULE_ID=mindspore::SubModuleId::SM_LITE)
add_library(mslite_converter_plugin SHARED ${REG_SRC})
target_link_libraries(mslite_converter_plugin mindspore::glog)
add_dependencies(mslite_converter_plugin fbs_src)
add_dependencies(mslite_converter_plugin fbs_inner_src)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND (NOT WIN32))
    add_custom_command(TARGET mslite_converter_plugin POST_BUILD COMMAND strip
            ${CMAKE_CURRENT_BINARY_DIR}/libmslite_converter_plugin.so)
endif()
