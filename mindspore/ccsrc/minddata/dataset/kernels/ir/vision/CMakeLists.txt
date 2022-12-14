file(GLOB_RECURSE _CURRENT_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc")
set_property(SOURCE ${_CURRENT_SRC_FILES} PROPERTY COMPILE_DEFINITIONS SUBMODULE_ID=mindspore::SubModuleId::SM_MD)

set(DATASET_KERNELS_IR_VISION_SRC_FILES
        affine_ir.cc
        auto_contrast_ir.cc
        bounding_box_augment_ir.cc
        center_crop_ir.cc
        crop_ir.cc
        cutmix_batch_ir.cc
        cutout_ir.cc
        decode_ir.cc
        equalize_ir.cc
        gaussian_blur_ir.cc
        horizontal_flip_ir.cc
        hwc_to_chw_ir.cc
        invert_ir.cc
        mixup_batch_ir.cc
        normalize_ir.cc
        normalize_pad_ir.cc
        pad_ir.cc
        random_affine_ir.cc
        random_color_adjust_ir.cc
        random_color_ir.cc
        random_crop_decode_resize_ir.cc
        random_crop_ir.cc
        random_crop_with_bbox_ir.cc
        random_horizontal_flip_ir.cc
        random_horizontal_flip_with_bbox_ir.cc
        random_posterize_ir.cc
        random_resized_crop_ir.cc
        random_resized_crop_with_bbox_ir.cc
        random_resize_ir.cc
        random_resize_with_bbox_ir.cc
        random_rotation_ir.cc
        random_select_subpolicy_ir.cc
        random_sharpness_ir.cc
        random_solarize_ir.cc
        random_vertical_flip_ir.cc
        random_vertical_flip_with_bbox_ir.cc
        rescale_ir.cc
        resize_ir.cc
        resize_preserve_ar_ir.cc
        resize_with_bbox_ir.cc
        rgb_to_bgr_ir.cc
        rgb_to_gray_ir.cc
        rgba_to_bgr_ir.cc
        rgba_to_rgb_ir.cc
        rotate_ir.cc
        slice_patches_ir.cc
        softdvpp_decode_random_crop_resize_jpeg_ir.cc
        softdvpp_decode_resize_jpeg_ir.cc
        swap_red_blue_ir.cc
        uniform_aug_ir.cc
        vertical_flip_ir.cc
        )

if(ENABLE_ACL)
    set(DATASET_KERNELS_IR_VISION_SRC_FILES
            ${DATASET_KERNELS_IR_VISION_SRC_FILES}
            ascend_vision_ir.cc)
endif()

add_library(kernels-ir-vision OBJECT ${DATASET_KERNELS_IR_VISION_SRC_FILES})
