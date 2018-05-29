/* Copyright (c) 2018 Baidu, Inc. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. 
*/

#ifndef ANAKIN_SABER_FUNCS_SPP_H
#define ANAKIN_SABER_FUNCS_SPP_H

#include "saber/funcs/base.h"
#include "saber/funcs/impl/impl_base.h"
#ifdef NVIDIA_GPU
#include "saber/funcs/impl/cuda/saber_spp.h"
#endif

#ifdef USE_X86_PLACE
//#include "saber/funcs/impl/x86/saber_activation.h"
#endif

namespace anakin {
namespace saber {

template <typename TargetType,
    DataType OpDtype,
    DataType inDtype = AK_FLOAT,
    DataType outDtype = AK_FLOAT,
    typename LayOutType_op = NCHW,
    typename LayOutType_in = NCHW,
    typename LayOutType_out = NCHW
    >
class Spp : public BaseFunc<
    Tensor<TargetType, inDtype, LayOutType_in>,
    Tensor<TargetType, outDtype, LayOutType_out>,
    Tensor<TargetType, OpDtype, LayOutType_op>,
    ImplBase,
    SPPParam
    >
{
public:
    using BaseFunc<
        Tensor<TargetType, inDtype, LayOutType_in>,
        Tensor<TargetType, outDtype, LayOutType_out>,
        Tensor<TargetType, OpDtype, LayOutType_op>,
        ImplBase,
        SPPParam >::BaseFunc;
    Spp() = default;

    typedef Tensor<TargetType, inDtype, LayOutType_in> InDataTensor;
    typedef Tensor<TargetType, outDtype, LayOutType_out> OutDataTensor;
    typedef Tensor<TargetType, OpDtype, LayOutType_op> OpTensor;
    typedef SPPParam<OpTensor> Param_t;
    typedef std::vector<InDataTensor *> Input_v;
    typedef std::vector<OutDataTensor *> Output_v;
    typedef std::vector<Shape> Shape_v;

    virtual SaberStatus compute_output_shape(const Input_v& input, \
        Output_v &output, Param_t& param) override {

        Shape output_shape = (input[0]->valid_shape());
        int out_height = 1;
        int out_width = (pow(4, param.pyramid_height) - 1) / 3;

        int height_idx = input[0]->height_index();
        int width_idx = input[0]->width_index();

        output_shape[height_idx] = out_height;
        output_shape[width_idx] = out_width;

        return output[0]->set_shape(output_shape);

    }

    virtual SaberStatus init_impl(ImplEnum implenum) override {
        switch (implenum) { 
            case VENDER_IMPL: 
                this->_impl.push_back(new VenderSpp <TargetType, OpDtype, inDtype, outDtype, 
                    LayOutType_op, LayOutType_in, LayOutType_out>); 
                return SaberSuccess; 
            case SABER_IMPL: 
                this->_impl.push_back(new SaberSpp <TargetType, OpDtype, inDtype, outDtype, 
                    LayOutType_op, LayOutType_in, LayOutType_out>); 
                return SaberSuccess; 
            default: 
                return SaberUnImplError;
        }        
    }

private:

    virtual void pick_best_static() override {
        if (true) // some condition?
            this->_best_impl = this->_impl[0];
    }

    virtual void pick_best_specify(ImplEnum implenum) override {
        this->_best_impl = this->_impl[0];
    }

};

}
}
#endif