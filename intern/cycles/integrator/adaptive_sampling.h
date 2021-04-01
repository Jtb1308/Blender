/*
 * Copyright 2011-2021 Blender Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

CCL_NAMESPACE_BEGIN

class AdaptiveSampling {
 public:
  AdaptiveSampling();

  /* Align number of samples so that they align with the adaptive filtering.
   *
   * Returns the new value for the `num_samples` so that after rendering so many samples on top
   * of `sample` filtering is required.
   *
   * The alignment happens in a way that allows to render as many samples as possible without
   * missing any filtering point. This means that the result is "clamped" by the nearest sample
   * at which filtering is needed. This is part of mechanism which ensures that all devices will
   * perform same exact filtering and adaptive sampling, regardless of their performance.
   *
   * `sample` is the 0-based index of sample. */
  int align_samples(int sample, int num_samples) const;

  /* Check whether adaptive sampling filter should happen at this sample.
   * Returns false if the adaptive sampling is not use.
   *
   * `sample` is the 0-based index of sample. */
  bool need_filter(int sample) const;

  bool use = false;
  int adaptive_step = 0;
  int min_samples = 0;
};

CCL_NAMESPACE_END
