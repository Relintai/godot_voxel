#ifndef HERMITE_VALUE_H
#define HERMITE_VALUE_H

#include "../../util/utility.h"
#include "../../voxel_buffer.h"
#include <core/math/vector3.h>

namespace dmc {

struct HermiteValue {
	// TODO Rename distance, or change usage to be a density
	float value; // Signed "distance" to surface
	Vector3 gradient; // Derivation of the density

	HermiteValue() :
			value(1.0) {
	}
};

inline float get_isolevel_clamped(const VoxelBuffer &voxels, unsigned int x, unsigned int y, unsigned int z) {

	x = x >= voxels.get_size().x ? voxels.get_size().x - 1 : x;
	y = y >= voxels.get_size().y ? voxels.get_size().y - 1 : y;
	z = z >= voxels.get_size().z ? voxels.get_size().z - 1 : z;

	return voxels.get_voxel(x, y, z, VoxelBuffer::CHANNEL_ISOLEVEL);
}

inline HermiteValue get_hermite_value(const VoxelBuffer &voxels, unsigned int x, unsigned int y, unsigned int z) {

	HermiteValue v;

	v.value = voxels.get_voxel_f(x, y, z, VoxelBuffer::CHANNEL_ISOLEVEL);

	Vector3 gradient;

	gradient.x = get_isolevel_clamped(voxels, x + 1, y, z) - get_isolevel_clamped(voxels, x - 1, y, z);
	gradient.y = get_isolevel_clamped(voxels, x, y + 1, z) - get_isolevel_clamped(voxels, x, y - 1, z);
	gradient.z = get_isolevel_clamped(voxels, x, y, z + 1) - get_isolevel_clamped(voxels, x, y, z - 1);

	v.gradient = gradient;

	return v;
}

inline HermiteValue get_interpolated_hermite_value(const VoxelBuffer &voxels, Vector3 pos) {

	int x0 = static_cast<int>(pos.x);
	int y0 = static_cast<int>(pos.y);
	int z0 = static_cast<int>(pos.z);

	int x1 = static_cast<int>(Math::ceil(pos.x));
	int y1 = static_cast<int>(Math::ceil(pos.y));
	int z1 = static_cast<int>(Math::ceil(pos.z));

	// TODO There are lots of hidden grid accesses here, could be optimized
	//
	//   x x     x: accessed once, only because of gradient computation
	// x X X x   X: accessed for both value and gradient, multiple times for gradient
	// x X X x
	//   x x     (and this, in 3D)

	HermiteValue v0 = get_hermite_value(voxels, x0, y0, z0);
	HermiteValue v1 = get_hermite_value(voxels, x1, y0, z0);
	HermiteValue v2 = get_hermite_value(voxels, x1, y0, z1);
	HermiteValue v3 = get_hermite_value(voxels, x0, y0, z1);

	HermiteValue v4 = get_hermite_value(voxels, x0, y1, z0);
	HermiteValue v5 = get_hermite_value(voxels, x1, y1, z0);
	HermiteValue v6 = get_hermite_value(voxels, x1, y1, z1);
	HermiteValue v7 = get_hermite_value(voxels, x0, y1, z1);

	Vector3 rpos = pos - Vector3(x0, y0, z0);

	HermiteValue v;
	v.value = ::interpolate(v0.value, v1.value, v2.value, v3.value, v4.value, v5.value, v6.value, v7.value, rpos);
	v.gradient = ::interpolate(v0.gradient, v1.gradient, v2.gradient, v3.gradient, v4.gradient, v5.gradient, v6.gradient, v7.gradient, rpos);

	return v;
}

} // namespace dmc

#endif // HERMITE_VALUE_H
