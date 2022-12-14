add_compile_definitions(USE_ANDROID_LOG)
if(ENABLE_V0)
    add_definitions(-DENABLE_V0)
endif()
include_directories(${CCSRC_DIR}/backend/kernel_compiler/cpu)
set(LITE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${LITE_DIR}/nnacl/)
include_directories(${LITE_DIR}/nnacl/optimize)

if(PLATFORM_ARM32 OR PLATFORM_ARM64)
    #for performance
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND NOT APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fomit-frame-pointer -fstrict-aliasing -ffunction-sections \
        -fdata-sections -ffast-math -fno-rtti -fno-exceptions")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fomit-frame-pointer -fstrict-aliasing -ffunction-sections \
        -fdata-sections -ffast-math -fno-rtti -fno-exceptions")
        if(TARGET_HIMIX200)
            string(REPLACE "-fno-rtti " "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
        endif()
    endif()
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -ffunction-sections \
        -fdata-sections -ffast-math -fno-rtti -fno-exceptions -Wno-shorten-64-to-32 \
        -fno-aligned-allocation -DTARGET_OS_OSX")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing -ffunction-sections \
        -fdata-sections -ffast-math -fno-rtti -fno-exceptions -Wno-shorten-64-to-32 \
        -fno-aligned-allocation -DTARGET_OS_OSX")
    endif()
else()
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fomit-frame-pointer -fstrict-aliasing -ffunction-sections \
        -fdata-sections -ffast-math -fno-rtti")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fomit-frame-pointer -fstrict-aliasing -ffunction-sections \
        -fdata-sections -ffast-math -fno-rtti")
    endif()
endif()

file(GLOB CXX_API_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/model/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/graph/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/tensor/*.cc
        )

set(API_SRC
        ${CORE_DIR}/utils/status.cc
        ${CXX_API_SRCS}
        )

file(GLOB CXX_API_TRAIN_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/train/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/metrics/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_api/callback/*.cc
        )
set(API_TRAIN_SRC
        ${CXX_API_TRAIN_SRCS}
        )

if(SUPPORT_NPU)
    include_directories(${DDK_PATH})
endif()

set(LITE_SRC
        ${API_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/common/context_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/file_utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/graph_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/dynamic_library_loader.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/log_adapter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/string_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/prim_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/tensor_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/common/dynamic_library_loader.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/delegate/delegate.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime/inner_allocator.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime/infer_manager.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/tensor.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/ms_tensor.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/tensorlist.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/executor.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/inner_context.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/lite_model.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/kernel_registry.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/inner_kernel.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/lite_kernel.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/lite_kernel_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sub_graph_kernel.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sub_graph_split.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/scheduler.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/lite_session.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/errorcode.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/weight_decoder.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/huffman_decode.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/cpu_info.cc
        )

file(GLOB KERNEL_REG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/registry/*.cc)
set(LITE_SRC ${LITE_SRC} ${KERNEL_REG_SRC})

if(MSLITE_GPU_BACKEND STREQUAL opencl)
    file(GLOB_RECURSE OPENCL_RUNTIME_SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/gpu/opencl/*.cc
            )
    set(LITE_SRC
            ${LITE_SRC}
            ${OPENCL_RUNTIME_SRC}
            )
endif()

if(MSLITE_GPU_BACKEND STREQUAL vulkan)
    file(GLOB VULKAN_RUNTIME_SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/gpu/*.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/gpu/vulkan/*.cc
            )
    set(LITE_SRC
            ${LITE_SRC}
            ${VULKAN_RUNTIME_SRC}
            )
endif()

if(MSLITE_GPU_BACKEND STREQUAL cuda)
    file(GLOB CUDA_RUNTIME_SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/gpu/*.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/runtime/gpu/cuda/*.cc
            )
    set(LITE_SRC
            ${LITE_SRC}
            ${CUDA_RUNTIME_SRC}
            )
endif()
set(TRAIN_SRC
        ${API_TRAIN_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/common/quant_utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/train_populate_parameter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/train_session.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/transfer_session.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/train_loop.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/train_utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/loss_monitor.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/lr_scheduler.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/accuracy_metrics.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/accuracy_monitor.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/classification_train_accuracy_monitor.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/train/train_export.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../tools/common/storage.cc
        )
if(ENABLE_V0)
    set(TRAIN_SRC
            ${TRAIN_SRC}
            ${CMAKE_CURRENT_SOURCE_DIR}/train/train_populate_parameter_v0.cc
            )
endif()

if(ENABLE_MINDRT)
    add_subdirectory(${CORE_DIR}/mindrt mindspore_mindrt)
    set(LITE_SRC
        ${LITE_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/lite_mindrt.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/mindrt_executor.cc
        )
elseif(TARGET_HIMIX200)
    file(GLOB MINDRT_ACTOR ${CORE_DIR}/mindrt/src/actor/*.cc)
    set(LITE_SRC
        ${LITE_SRC}
        ${MINDRT_ACTOR}
        ${CORE_DIR}/mindrt/src/thread/core_affinity.cc
        ${CORE_DIR}/mindrt/src/thread/actor_threadpool.cc
        ${CORE_DIR}/mindrt/src/thread/threadpool.cc
        )
endif()

add_subdirectory(ops)
add_subdirectory(runtime/kernel/arm)

add_library(lite_src_mid OBJECT ${LITE_SRC})
add_dependencies(lite_src_mid fbs_src)

add_library(mindspore-lite SHARED $<TARGET_OBJECTS:lite_src_mid>)
set_target_properties(mindspore-lite PROPERTIES CLEAN_DIRECT_OUTPUT 1)

if(DEFINED ARCHS)
    set(MINDSPORE_LITE_PUB_HDRS
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/context.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/errorcode.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/lite_session.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/lite_types.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/lite_utils.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/model.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/ms_tensor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/version.h
            )
    set(MINDSPORE_LITE_PUB_HDRS_IR_HDRS
            ${CMAKE_CURRENT_SOURCE_DIR}/../../core/ir/dtype/type_id.h
            )
    add_library(mindspore-lite_static STATIC
            ${LITE_SRC}
            ${MINDSPORE_LITE_PUB_HDRS}
            ${MINDSPORE_LITE_PUB_HDRS_IR_HDRS}
            )
    add_dependencies(mindspore-lite_static fbs_inner_src)
else()
    add_library(mindspore-lite_static STATIC $<TARGET_OBJECTS:lite_src_mid>)
endif()
set_target_properties(mindspore-lite_static PROPERTIES OUTPUT_NAME "mindspore-lite")
set_target_properties(mindspore-lite_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")

if(ENABLE_MINDRT)
    target_link_libraries(mindspore-lite mindrt_mid)
    target_link_libraries(mindspore-lite_static mindrt_mid)
endif()

if(SUPPORT_TENSORRT)
    add_compile_definitions(GPU_TENSORRT)
    set(TENSORRT_PATH $ENV{TENSORRT_PATH})
    set(CUDA_PATH $ENV{CUDA_HOME})
    set(TENSORRT_LIB_PATH ${TENSORRT_PATH}/lib)
    set(CUDA_LIB_PATH ${CUDA_PATH}/lib64)
    include_directories(${TENSORRT_PATH}/include)
    include_directories(${CUDA_PATH}/include)
    add_subdirectory(delegate/tensorrt)
    target_link_libraries(mindspore-lite tensorrt_kernel_mid)
    target_link_libraries(mindspore-lite_static tensorrt_kernel_mid)
endif()

if(MSLITE_GPU_BACKEND STREQUAL opencl)
    add_subdirectory(runtime/kernel/opencl)
    target_link_libraries(mindspore-lite cpu_kernel_mid cpu_kernel_random_standard_normal
                          opencl_kernel_mid nnacl_mid cpu_ops_mid)
    target_link_libraries(mindspore-lite_static cpu_kernel_mid cpu_kernel_random_standard_normal
                          opencl_kernel_mid nnacl_mid cpu_ops_mid)
elseif(MSLITE_GPU_BACKEND STREQUAL cuda)
    add_subdirectory(runtime/kernel/cuda)
    target_link_libraries(mindspore-lite cpu_kernel_mid cpu_kernel_random_standard_normal
                          cuda_kernel_mid nnacl_mid cpu_ops_mid)
    target_link_libraries(mindspore-lite_static cpu_kernel_mid cpu_kernel_random_standard_normal
                          cuda_kernel_mid nnacl_mid cpu_ops_mid)
else()
    target_link_libraries(mindspore-lite cpu_kernel_mid cpu_kernel_random_standard_normal
                          nnacl_mid cpu_ops_mid)
    target_link_libraries(mindspore-lite_static cpu_kernel_mid cpu_kernel_random_standard_normal
                          nnacl_mid cpu_ops_mid)
endif()
if(SUPPORT_NPU)
    add_subdirectory(delegate/npu)
    target_link_libraries(mindspore-lite npu_kernel_mid)
    target_link_libraries(mindspore-lite_static npu_kernel_mid)
endif()
if(PLATFORM_ARM32 OR PLATFORM_ARM64 AND NOT TARGET_HIMIX200)
    target_link_libraries(mindspore-lite log)
    target_link_libraries(mindspore-lite_static log)
endif()
if(BUILD_MINDDATA STREQUAL "lite")
    target_link_libraries(mindspore-lite minddata_eager_mid minddata-lite)
    target_link_libraries(mindspore-lite_static minddata_eager_mid)
endif()

if(SUPPORT_TRAIN)
  add_library(mindspore-lite-train SHARED ${TRAIN_SRC})
  set_target_properties(mindspore-lite-train PROPERTIES OUTPUT_NAME "mindspore-lite-train")
  add_dependencies(mindspore-lite-train fbs_src fbs_inner_src)
  set_target_properties(mindspore-lite-train PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  target_link_libraries(mindspore-lite-train minddata-lite mindspore-lite)

  add_library(mindspore-lite-train_static STATIC ${TRAIN_SRC})
  set_target_properties(mindspore-lite-train_static PROPERTIES OUTPUT_NAME "mindspore-lite-train")
  add_dependencies(mindspore-lite-train_static fbs_inner_src)
  set_target_properties(mindspore-lite-train_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  target_link_libraries(mindspore-lite-train_static minddata-lite mindspore-lite)
endif()

if(NOT APPLE AND PLATFORM_ARM)
    set(NDK_STRIP
        "${ANDROID_NDK}/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/aarch64-linux-android/bin/strip")
    if(TARGET_HIMIX200)
        set(NDK_STRIP "arm-himix200-linux-strip")
    endif()
endif()

if(NOT APPLE AND "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    if(PLATFORM_ARM)
        add_custom_command(TARGET mindspore-lite POST_BUILD COMMAND ${NDK_STRIP}
                ${CMAKE_BINARY_DIR}/src/libmindspore-lite*.so)
    elseif(NOT WIN32)
        add_custom_command(TARGET mindspore-lite POST_BUILD COMMAND strip ${CMAKE_BINARY_DIR}/src/libmindspore-lite*.so)
    endif()
endif()

########################## build optimize and float16 library #################################
if(PLATFORM_ARM)
    if(PLATFORM_ARM64)
        target_link_libraries(mindspore-lite cpu_opt_kernel_mid nnacl_optimize_mid)
        target_link_libraries(mindspore-lite_static cpu_opt_kernel_mid nnacl_optimize_mid)
    endif()
    if(ENABLE_FP16)
        target_link_libraries(mindspore-lite cpu_fp16_kernel_mid nnacl_fp16_mid)
        target_link_libraries(mindspore-lite_static cpu_fp16_kernel_mid nnacl_fp16_mid)
    endif()
endif()

if(DEFINED ARCHS)
    set_target_properties(mindspore-lite_static PROPERTIES FRAMEWORK TRUE)
    set_target_properties(mindspore-lite_static PROPERTIES
            OUTPUT_NAME "mindspore-lite"
            FRAMEWORK_VERSION C
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
            XCODE_ATTRIBUTE_INSTALL_PAYH "@rpath"
            )
    FOREACH(HDR ${MINDSPORE_LITE_PUB_HDRS})
        SET_SOURCE_FILES_PROPERTIES(${HDR} PROPERTIES MACOSX_PACKAGE_LOCATION Headers/include/)
    ENDFOREACH()
    FOREACH(HDR ${MINDSPORE_LITE_PUB_HDRS_IR_HDRS})
        SET_SOURCE_FILES_PROPERTIES(${HDR} PROPERTIES MACOSX_PACKAGE_LOCATION Headers/include/ir/dtype/)
    ENDFOREACH()
    target_link_libraries(mindspore-lite_static)
endif()

if(NOT WIN32)
    target_link_libraries(mindspore-lite dl)
endif()

if(ENABLE_MODEL_OBF)
    target_link_libraries(mindspore-lite ${OBF_LIB_DIR}/libmsdeobfuscator-lite.so)
    target_link_libraries(mindspore-lite_static ${OBF_LIB_DIR}/libmsdeobfuscator-lite.so)
endif()
