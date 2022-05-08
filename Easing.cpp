#include "Easing.h"
#include <DirectXMath.h>

float Easing::easeOutCubic(float t){ return 1.0f - pow(1.0f - t, 3); }
