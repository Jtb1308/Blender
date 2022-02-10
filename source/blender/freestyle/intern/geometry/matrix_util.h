/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is:
 *   GXML/Graphite: Geometry and Graphics Programming Library + Utilities
 *   Copyright 2000 Bruno Levy <levy@loria.fr>
 */

#pragma once

/** \file
 * \ingroup freestyle
 */

#include "../system/FreestyleConfig.h"

namespace Freestyle {

namespace OGF {

namespace MatrixUtil {

/**
 * computes the eigen values and eigen vectors of a semi definite symmetric matrix
 *
 * \param mat: The matrix stored in column symmetric storage, i.e.
 * <pre>
 * matrix = { m11, m12, m22, m13, m23, m33, m14, m24, m34, m44 ... }
 * size = n(n+1)/2
 * </pre>
 *
 * \param eigen_vec: (return) = { v1, v2, v3, ..., vn }
 *   where `vk = vk0, vk1, ..., vkn`
 *     `size = n^2`, must be allocated by caller.
 *
 * \param eigen_val: (return) are in decreasing order
 *     `size = n`,   must be allocated by caller.
 */
void semi_definite_symmetric_eigen(const double *mat, int n, double *eigen_vec, double *eigen_val);

}  // namespace MatrixUtil

}  // namespace OGF

} /* namespace Freestyle */
