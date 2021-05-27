/*
 * Copyright 2011-2013 Blender Foundation
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

#ifdef WITH_OPTIX

#  include "device/optix/queue.h"
#  include "device/optix/device_impl.h"

#  include "util/util_time.h"

#  undef __KERNEL_CPU__
#  define __KERNEL_OPTIX__
#  include "kernel/device/optix/globals.h"

CCL_NAMESPACE_BEGIN

/* CUDADeviceQueue */

OptiXDeviceQueue::OptiXDeviceQueue(OptiXDevice *device) : CUDADeviceQueue(device)
{
}

void OptiXDeviceQueue::init_execution()
{
  CUDADeviceQueue::init_execution();
}

static bool is_optix_specific_kernel(DeviceKernel kernel)
{
  return (kernel == DEVICE_KERNEL_INTEGRATOR_SHADE_SURFACE_RAYTRACE ||
          kernel == DEVICE_KERNEL_INTEGRATOR_INTERSECT_CLOSEST ||
          kernel == DEVICE_KERNEL_INTEGRATOR_INTERSECT_SHADOW ||
          kernel == DEVICE_KERNEL_INTEGRATOR_INTERSECT_SUBSURFACE);
}

bool OptiXDeviceQueue::enqueue(DeviceKernel kernel, const int work_size, void *args[])
{
  /* TODO: Handle shading kernels when shader raytracing feature is requested */

  if (!is_optix_specific_kernel(kernel)) {
    return CUDADeviceQueue::enqueue(kernel, work_size, args);
  }

  if (cuda_device_->have_error()) {
    return false;
  }

  debug_enqueue(kernel, work_size);

  const CUDAContextScope scope(cuda_device_);

  OptiXDevice *const optix_device = static_cast<OptiXDevice *>(cuda_device_);

  const device_ptr sbt_data_ptr = optix_device->sbt_data.device_pointer;
  const device_ptr launch_params_ptr = optix_device->launch_params.device_pointer;

  cuda_device_assert(
      cuda_device_,
      cuMemcpyHtoDAsync(launch_params_ptr + offsetof(KernelParamsOptiX, path_index_array),
                        args[0],  // &d_path_index
                        sizeof(device_ptr),
                        cuda_stream_));

  if (kernel == DEVICE_KERNEL_INTEGRATOR_SHADE_SURFACE_RAYTRACE) {
    cuda_device_assert(
        cuda_device_,
        cuMemcpyHtoDAsync(launch_params_ptr + offsetof(KernelParamsOptiX, render_buffer),
                          args[1],  // &d_render_buffer
                          sizeof(device_ptr),
                          cuda_stream_));
  }

  cuda_device_assert(cuda_device_, cuStreamSynchronize(cuda_stream_));

  OptixPipeline pipeline = nullptr;
  OptixShaderBindingTable sbt_params = {};

  switch (kernel) {
    case DEVICE_KERNEL_INTEGRATOR_SHADE_SURFACE_RAYTRACE:
      pipeline = optix_device->pipelines[PIP_SHADE_RAYTRACE];
      sbt_params.raygenRecord = sbt_data_ptr + PG_RGEN_SHADE_SURFACE_RAYTRACE * sizeof(SbtRecord);
      break;
    case DEVICE_KERNEL_INTEGRATOR_INTERSECT_CLOSEST:
      pipeline = optix_device->pipelines[PIP_INTERSECT];
      sbt_params.raygenRecord = sbt_data_ptr + PG_RGEN_INTERSECT_CLOSEST * sizeof(SbtRecord);
      break;
    case DEVICE_KERNEL_INTEGRATOR_INTERSECT_SHADOW:
      pipeline = optix_device->pipelines[PIP_INTERSECT];
      sbt_params.raygenRecord = sbt_data_ptr + PG_RGEN_INTERSECT_SHADOW * sizeof(SbtRecord);
      break;
    case DEVICE_KERNEL_INTEGRATOR_INTERSECT_SUBSURFACE:
      pipeline = optix_device->pipelines[PIP_INTERSECT];
      sbt_params.raygenRecord = sbt_data_ptr + PG_RGEN_INTERSECT_SUBSURFACE * sizeof(SbtRecord);
      break;

    case DEVICE_KERNEL_INTEGRATOR_INIT_FROM_CAMERA:
    case DEVICE_KERNEL_INTEGRATOR_SHADE_BACKGROUND:
    case DEVICE_KERNEL_INTEGRATOR_SHADE_LIGHT:
    case DEVICE_KERNEL_INTEGRATOR_SHADE_SURFACE:
    case DEVICE_KERNEL_INTEGRATOR_SHADE_VOLUME:
    case DEVICE_KERNEL_INTEGRATOR_SHADE_SHADOW:
    case DEVICE_KERNEL_INTEGRATOR_MEGAKERNEL:
    case DEVICE_KERNEL_INTEGRATOR_QUEUED_PATHS_ARRAY:
    case DEVICE_KERNEL_INTEGRATOR_QUEUED_SHADOW_PATHS_ARRAY:
    case DEVICE_KERNEL_INTEGRATOR_ACTIVE_PATHS_ARRAY:
    case DEVICE_KERNEL_INTEGRATOR_TERMINATED_PATHS_ARRAY:
    case DEVICE_KERNEL_INTEGRATOR_SORTED_PATHS_ARRAY:
    case DEVICE_KERNEL_INTEGRATOR_RESET:
    case DEVICE_KERNEL_SHADER_EVAL_DISPLACE:
    case DEVICE_KERNEL_SHADER_EVAL_BACKGROUND:
    case DEVICE_KERNEL_FILM_CONVERT_DEPTH_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_MIST_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_SAMPLE_COUNT_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_FLOAT_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_SHADOW3_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_DIVIDE_EVEN_COLOR_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_FLOAT3_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_SHADOW4_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_MOTION_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_CRYPTOMATTE_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_DENOISING_COLOR_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_SHADOW_CATCHER_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_SHADOW_CATCHER_MATTE_WITH_SHADOW_HALF_RGBA:
    case DEVICE_KERNEL_FILM_CONVERT_FLOAT4_HALF_RGBA:
    case DEVICE_KERNEL_ADAPTIVE_SAMPLING_CONVERGENCE_CHECK:
    case DEVICE_KERNEL_ADAPTIVE_SAMPLING_CONVERGENCE_FILTER_X:
    case DEVICE_KERNEL_ADAPTIVE_SAMPLING_CONVERGENCE_FILTER_Y:
    case DEVICE_KERNEL_FILTER_CONVERT_TO_RGB:
    case DEVICE_KERNEL_FILTER_CONVERT_FROM_RGB:
    case DEVICE_KERNEL_PREFIX_SUM:
    case DEVICE_KERNEL_NUM:
      LOG(ERROR) << "Invalid kernel is attempted to be enqueued.";
      return false;
  }

  sbt_params.missRecordBase = sbt_data_ptr + MISS_PROGRAM_GROUP_OFFSET * sizeof(SbtRecord);
  sbt_params.missRecordStrideInBytes = sizeof(SbtRecord);
  sbt_params.missRecordCount = NUM_MIS_PROGRAM_GROUPS;
  sbt_params.hitgroupRecordBase = sbt_data_ptr + HIT_PROGAM_GROUP_OFFSET * sizeof(SbtRecord);
  sbt_params.hitgroupRecordStrideInBytes = sizeof(SbtRecord);
  sbt_params.hitgroupRecordCount = NUM_HIT_PROGRAM_GROUPS;
  sbt_params.callablesRecordBase = sbt_data_ptr + CALLABLE_PROGRAM_GROUPS_BASE * sizeof(SbtRecord);
  sbt_params.callablesRecordCount = NUM_CALLABLE_PROGRAM_GROUPS;
  sbt_params.callablesRecordStrideInBytes = sizeof(SbtRecord);

  /* Launch the ray generation program. */
  optix_device_assert(optix_device,
                      optixLaunch(pipeline,
                                  cuda_stream_,
                                  launch_params_ptr,
                                  optix_device->launch_params.data_elements,
                                  &sbt_params,
                                  work_size,
                                  1,
                                  1));

  return !(optix_device->have_error());
}

CCL_NAMESPACE_END

#endif /* WITH_OPTIX */
