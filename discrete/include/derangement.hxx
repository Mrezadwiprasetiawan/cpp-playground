// Derangement.hpp – compact hybrid derangement calculator (integer‑only except cutoff logic)
#pragma once

#include <vector>
#include <type_traits>
#include <cstdint>
#include <functional>
#include <cmath>

namespace Discrete {

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>> 
class Derangement {
    static inline std::vector<T> cache = {1, 0};   // !0 = 1, !1 = 0

    // Compute cutoff based on k_min ≈ log_phi((3√5 / 5) * n)
    static T min_k_pie_faster(T n) {
        // We keep the 3/5 * sqrt(5) multiplier exactly
        // and use log base φ to stay faithful to the derivation
        // phi = (1 + sqrt(5)) / 2 ≈ 1.61803
        return static_cast<T>(
            std::log((3.0 * n * std::sqrt(5.0)) / 5.0) / std::log((1.0 + std::sqrt(5.0)) / 2.0)
        );
    }

    static T pie_calc(T n) {
    T k_max = min_k_pie_faster(n);
    if (k_max > n) k_max = n;

    T term = 1;
    T sum  = 1;
    int sign = -1;

    for (T i = 1; i <= k_max; ++i) {
        term *= (n - i + 1);     // P(n,i)
        sum += sign * term;
        sign = -sign;
    }

    if ((k_max == n) && (n & 1)) sum = -sum;
    return sum;
}


    static T recursive_calc(T n) {
        if (n < static_cast<T>(cache.size())) return cache[n];
        cache.resize(n + 1);
        for (T i = static_cast<T>(cache.size() - 1); i <= n; ++i)
            cache[i] = (i - 1) * (cache[i - 1] + cache[i - 2]);
        return cache[n];
    }

public:
    static T calc(T n) {
        return (n > min_k_pie_faster(n)) ? pie_calc(n) : recursive_calc(n);
    }

    template <typename U>
    std::vector<std::vector<U>> derange(const std::vector<U> &src) {
        std::vector<std::vector<U>> result;
        std::vector<U> current(src.size());
        std::vector<bool> used(src.size(), false);

        std::function<void(size_t)> dfs = [&](size_t i) {
            if (i == src.size()) {
                result.push_back(current);
                return;
            }
            for (size_t j = 0; j < src.size(); ++j) {
                if (used[j] || j == i) continue;
                used[j] = true;
                current[i] = src[j];
                dfs(i + 1);
                used[j] = false;
            }
        };

        dfs(0);
        return result;
    }
};

} // namespace Discrete
