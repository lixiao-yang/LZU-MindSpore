# Copyright 2021 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
"""Network evaluation config setting, will be used in eval.py."""

from easydict import EasyDict as edict
import mindspore.common.dtype as mstype
from .transformer_model import TransformerConfig

cfg = edict({
    'transformer_network': 'large',
    'data_file': '/your/path/evaluation.mindrecord',
    'model_file': '/your/path/checkpoint_file',
    'output_file': '/your/path/output',
})
'''
two kinds of transformer model version
'''
if cfg.transformer_network == 'large':
    transformer_net_cfg = TransformerConfig(
        batch_size=1,
        seq_length=128,
        vocab_size=36560,
        hidden_size=1024,
        num_hidden_layers=6,
        num_attention_heads=16,
        intermediate_size=4096,
        hidden_act="relu",
        hidden_dropout_prob=0.0,
        attention_probs_dropout_prob=0.0,
        max_position_embeddings=128,
        label_smoothing=0.1,
        beam_width=4,
        max_decode_length=80,
        length_penalty_weight=1.0,
        dtype=mstype.float32,
        compute_type=mstype.float16)
if cfg.transformer_network == 'base':
    transformer_net_cfg = TransformerConfig(
        batch_size=1,
        seq_length=128,
        vocab_size=36560,
        hidden_size=512,
        num_hidden_layers=6,
        num_attention_heads=8,
        intermediate_size=2048,
        hidden_act="relu",
        hidden_dropout_prob=0.0,
        attention_probs_dropout_prob=0.0,
        max_position_embeddings=128,
        label_smoothing=0.1,
        beam_width=4,
        max_decode_length=80,
        length_penalty_weight=1.0,
        dtype=mstype.float32,
        compute_type=mstype.float16)
