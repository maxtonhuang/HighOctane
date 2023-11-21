#include "Components.h"

Transform operator+ (const Transform& lhs, const Transform& rhs) {
	Transform output{ lhs };
	output.position += rhs.position;
	output.rotation += rhs.rotation;
	return output;
}
