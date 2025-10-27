#ifndef ITERATION_H
#define ITERATION_H
#include <cstddef>

namespace Iteration {
inline volatile std::size_t counter = 0;
inline void reset() { counter = 0; }
inline void step(std::size_t n = 1) { counter += n; }
inline std::size_t value() { return counter; }
}
#define ITER_RESET()  ::Iteration::reset()
#define ITER_STEP(n)  ::Iteration::step(n)
#define ITER_VALUE()  ::Iteration::value()

#endif // ITERATION_H
