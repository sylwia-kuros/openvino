// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "openvino/core/validation_util.hpp"
#include "openvino/op/scatter_elements_update.hpp"
#include "openvino/op/constant.hpp"

#include "intel_gpu/plugin/program_builder.hpp"
#include "intel_gpu/plugin/common_utils.hpp"
#include "intel_gpu/primitives/scatter_elements_update.hpp"

namespace ov {
namespace intel_gpu {

static void CreateScatterElementsUpdateOp(ProgramBuilder& p, const std::shared_ptr<ov::op::v3::ScatterElementsUpdate>& op) {
    validate_inputs_count(op, {4});
    auto inputs = p.GetInputInfo(op);
    std::string layerName = layer_type_name_ID(op);

    auto axes_constant = std::dynamic_pointer_cast<ov::op::v0::Constant>(op->get_input_node_shared_ptr(3));
    if (!axes_constant) {
        OPENVINO_ASSERT("Unsupported parameter nodes type in ", op->get_friendly_name(), " (", op->get_type_name(), ")");
    }
    OPENVINO_SUPPRESS_DEPRECATED_START
    int64_t axis = ov::normalize_axis(op.get(), axes_constant->cast_vector<int64_t>()[0], op->get_input_partial_shape(0).rank());
    OPENVINO_SUPPRESS_DEPRECATED_END

    auto primitive = cldnn::scatter_elements_update(layerName,
                                                    inputs[0],
                                                    inputs[1],
                                                    inputs[2],
                                                    axis);

    p.add_primitive(*op, primitive);
}

REGISTER_FACTORY_IMPL(v3, ScatterElementsUpdate);

}  // namespace intel_gpu
}  // namespace ov
