#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "core/key_codes.h"
#include "core/mouse_codes.h"
#include "core/defines.h"
#include "events/built_in_events.h"

#ifdef AVO_PLATFORM_WINDOWS
#include <Windows.h>
#endif