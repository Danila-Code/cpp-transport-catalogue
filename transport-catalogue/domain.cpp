#include "domain.h"

namespace domain {

size_t StopHasher::operator()(std::pair<const Stop*, const Stop*> value) const noexcept{
    return (hasher(value.first) << 32) + hasher(value.second);
}

}  // namespace domain