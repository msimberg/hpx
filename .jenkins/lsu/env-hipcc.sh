# Copyright (c) 2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

module load rocm/3.7.0
export CXX=hipcc
export CC=clang
# Tmp: needed until rocsolver is installed in the rocm module
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/rauriane/install/rocsolver/lib

export HPXRUN_RUNWRAPPER=srun

configure_extra_options="-DCMAKE_BUILD_TYPE=${build_type}"
configure_extra_options+=" -DHPX_WITH_MALLOC=system"
configure_extra_options+=" -DHPX_WITH_DEPRECATION_WARNINGS=OFF"
configure_extra_options+=" -DHPX_WITH_COMPILER_WARNINGS=ON"
configure_extra_options+=" -DHPX_WITH_COMPILER_WARNINGS_AS_ERRORS=ON"
configure_extra_options+=" -DHPX_WITH_ASYNC_CUDA=ON"
configure_extra_options+=" -DHPX_WITH_CUDA_COMPUTE=ON"
