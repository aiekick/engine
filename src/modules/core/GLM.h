#pragma once

#define GLM_FORCE_RADIANS

#include <functional>
#include <limits>
#include <math.h>

#include <glm/gtc/integer.hpp>
#include <glm/glm.hpp>

#include <glm/gtc/round.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/epsilon.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/rotate_vector.hpp>

template<int N, typename T>
struct VecLessThan {
	inline bool operator()(const glm::vec<N, T>& lhs, const glm::vec<N, T>& rhs) const {
		return glm::all(glm::lessThan(lhs, rhs));
	}
};

namespace glm {
extern const glm::vec3 forward;
extern const glm::vec3 backward;
extern const glm::vec3 right;
extern const glm::vec3 left;
extern const glm::vec3 up;
extern const glm::vec3 down;

GLM_FUNC_QUALIFIER vec3 transform(const mat4& mat, const vec3& v) {
	const mat4::col_type& c1 = column(mat, 0);
	const mat4::col_type& c2 = column(mat, 1);
	const mat4::col_type& c3 = column(mat, 2);
	vec3 r(c1.x * v.x + c1.y * v.y + c1.z * v.z + c1.w,
		c2.x * v.x + c2.y * v.y + c2.z * v.z + c2.w,
		c3.x * v.x + c3.y * v.y + c3.z * v.z + c3.w);
	return r;
}

GLM_FUNC_QUALIFIER vec3 rotate(const mat4& mat, const vec3& v) {
	const mat4::col_type& c1 = column(mat, 0);
	const mat4::col_type& c2 = column(mat, 1);
	const mat4::col_type& c3 = column(mat, 2);
	vec3 r(c1.x * v.x + c1.y * v.y + c1.z * v.z,
		c2.x * v.x + c2.y * v.y + c2.z * v.z,
		c3.x * v.x + c3.y * v.y + c3.z * v.z);
	return r;
}

GLM_FUNC_QUALIFIER vec3 project(const mat4& m, const vec3& p) {
	const vec4& r = m * vec4(p, 1);
	return vec3(r) / r.w;
}

}
