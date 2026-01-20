# Report: Differentiation and Minimization

## Analytical background
The lab covers one-dimensional minimization on a closed interval and numerical
differentiation on a uniform grid. For minimization we assume f is continuous
and unimodal on [a, b], so interval-reduction methods converge to the global
minimum. If f or f' has poles/singularities inside the interval, the program
stops with an error and the interval must be adjusted.

Minimization methods:
- Golden section search keeps [a, b] and evaluates
  y = a + (1 - tau) * (b - a), z = a + tau * (b - a),
  tau = (sqrt(5) - 1) / 2. The interval is reduced toward the point with the
  smaller f(x). Each step shrinks the interval by factor tau ~ 0.618, so the
  iteration count is about log((b - a) / eps) / log(1 / tau).
  Termination: (b - a) <= eps.
- Dichotomy uses midpoint m = (a + b) / 2 and probes
  y = m - delta, z = m + delta. After comparing f(y) and f(z), it keeps the
  half containing the smaller value. Each step reduces the interval roughly by
  1/2 (slightly less because of delta), so iterations are about
  log<sub>2</sub>((b - a) / (2 * eps)). Termination: (b - a) <= 2 * eps.
  In the implementation, delta defaults to eps / 2 and is clamped to a safe
  range relative to (b - a) to keep y, z inside the interval.

Numerical differentiation methods (uniform grid step h):
Let x<sub>i</sub> = a + i * h, i = 0..n, with n = (b - a) / h (must be integer).
We precompute y<sub>i</sub> = f(x<sub>i</sub>) and then build derivatives.
- Right difference: f'(x<sub>i</sub>) ~= (f(x<sub>i+1</sub>) - f(x<sub>i</sub>)) / h, error O(h).
- Left difference: f'(x<sub>i</sub>) ~= (f(x<sub>i</sub>) - f(x<sub>i-1</sub>)) / h, error O(h).
- Central difference: f'(x<sub>i</sub>) ~= (f(x<sub>i+1</sub>) - f(x<sub>i-1</sub>)) / (2h), error O(h<sup>2</sup>).
At the boundaries, second-order one-sided formulas are used:
f'(x<sub>0</sub>) ~= (-3 f<sub>0</sub> + 4 f<sub>1</sub> - f<sub>2</sub>) / (2h),
f'(x<sub>n</sub>) ~= (f<sub>n-2</sub> - 4 f<sub>n-1</sub> + 3 f<sub>n</sub>) / (2h).

Quality metric:
- RMSE = sqrt((1 / n) * sum<sub>i</sub> (d<sub>i</sub> - d<sub>true,i</sub>)<sup>2</sup>), where d<sub>true</sub> is computed by
  the app using exprtk::derivative with an internal step size
  h<sub>true</sub> = max(1e-8, |x| * 1e-4 + 1e-6).
  RMSE is computed over the full grid and reported per method.

Implementation in code:
- Golden section: `core/src/GoldenSectionMinimizer.cc`
- Dichotomy: `core/src/DichotomyMinimizer.cc`
- Task 1 flow: `core/src/Task1.cc`
- Right/Left/Central differences: `core/src/RightDifference.cc`,
  `core/src/LeftDifference.cc`, `core/src/CentralDifference.cc`
- Grid building + boundary formulas: `core/src/DiffCommon.h`
- Task 2 flow: `core/src/Task2.cc`
- Expression parsing + true derivative: `core/src/Expression.cc`

## Contacts
- @SanceiLaks

## Functions and intervals
1) `sin(x) + x^3`, `[-2, 2]`  
2) `cos(x) - 0.5*x`, `[-3, 3]`  
3) `tan(x) + x^2`, `[-1, 1]`  
4) `1/(1 + x^2)`, `[-3, 3]`  
5) `1/(x + 2) + x`, `[-1, 2]`  
6) `sqrt(x + 3) + x^2`, `[-2, 2]`  
7) `exp(-x^2) + x`, `[-2, 2]`  
8) `log(x + 3) + x^2`, `[-2, 2]`  
9) `x^4 - 3*x^2 + 2`, `[-2, 2]`  
10) `sin(x)/x + 0.1*x`, `[0.5, 3]`

## Differentiation (RMSE vs true derivative)
Step sizes: `h = 0.1`, `0.05`, `0.025`.  

| # | Function | Interval | h | RMSE right | RMSE left | RMSE central |
|---|----------|----------|---|------------|-----------|--------------|
| 1 | `sin(x) + x^3` | [-2.0, 2.0] | 0.1 | 0.304968 | 0.304968 | 0.0101458 |
| 1 | `sin(x) + x^3` | [-2.0, 2.0] | 0.05 | 0.153441 | 0.153441 | 0.00243254 |
| 1 | `sin(x) + x^3` | [-2.0, 2.0] | 0.025 | 0.076961 | 0.076961 | 0.000594177 |
| 2 | `cos(x) - 0.5*x` | [-3.0, 3.0] | 0.1 | 0.0342278 | 0.0342278 | 0.00120166 |
| 2 | `cos(x) - 0.5*x` | [-3.0, 3.0] | 0.05 | 0.0171884 | 0.0171884 | 0.000300677 |
| 2 | `cos(x) - 0.5*x` | [-3.0, 3.0] | 0.025 | 0.00861248 | 0.00861248 | 7.52484e-05 |
| 3 | `tan(x) + x^2` | [-1.0, 1.0] | 0.1 | 0.19397 | 0.202462 | 0.041766 |
| 3 | `tan(x) + x^2` | [-1.0, 1.0] | 0.05 | 0.0988648 | 0.101031 | 0.0100063 |
| 3 | `tan(x) + x^2` | [-1.0, 1.0] | 0.025 | 0.049982 | 0.0505279 | 0.0022639 |
| 4 | `1/(1 + x^2)` | [-3.0, 3.0] | 0.1 | 0.0309924 | 0.0309924 | 0.00281705 |
| 4 | `1/(1 + x^2)` | [-3.0, 3.0] | 0.05 | 0.0155844 | 0.0155844 | 0.000710838 |
| 4 | `1/(1 + x^2)` | [-3.0, 3.0] | 0.025 | 0.00781141 | 0.00781141 | 0.000178308 |
| 5 | `1/(x + 2) + x` | [-1.0, 2.0] | 0.1 | 0.0263719 | 0.0244221 | 0.00326464 |
| 5 | `1/(x + 2) + x` | [-1.0, 2.0] | 0.05 | 0.0130553 | 0.0125349 | 0.000743657 |
| 5 | `1/(x + 2) + x` | [-1.0, 2.0] | 0.025 | 0.00649098 | 0.00635797 | 0.000167483 |
| 6 | `sqrt(x + 3) + x^2` | [-2.0, 2.0] | 0.1 | 0.09535 | 0.0954405 | 0.000215278 |
| 6 | `sqrt(x + 3) + x^2` | [-2.0, 2.0] | 0.05 | 0.0479798 | 0.0480025 | 4.86637e-05 |
| 6 | `sqrt(x + 3) + x^2` | [-2.0, 2.0] | 0.025 | 0.0240671 | 0.0240728 | 1.11624e-05 |
| 7 | `exp(-x^2) + x` | [-2.0, 2.0] | 0.1 | 0.0476879 | 0.0476879 | 0.00358338 |
| 7 | `exp(-x^2) + x` | [-2.0, 2.0] | 0.05 | 0.0240146 | 0.0240146 | 0.000897014 |
| 7 | `exp(-x^2) + x` | [-2.0, 2.0] | 0.025 | 0.0120476 | 0.0120476 | 0.000224396 |
| 8 | `log(x + 3) + x^2` | [-2.0, 2.0] | 0.1 | 0.089316 | 0.0896428 | 0.00106555 |
| 8 | `log(x + 3) + x^2` | [-2.0, 2.0] | 0.05 | 0.0449741 | 0.0450555 | 0.000240572 |
| 8 | `log(x + 3) + x^2` | [-2.0, 2.0] | 0.025 | 0.022567 | 0.0225874 | 5.4692e-05 |
| 9 | `x^4 - 3*x^2 + 2` | [-2.0, 2.0] | 0.1 | 0.861947 | 0.861947 | 0.0555399 |
| 9 | `x^4 - 3*x^2 + 2` | [-2.0, 2.0] | 0.05 | 0.433707 | 0.433707 | 0.0128386 |
| 9 | `x^4 - 3*x^2 + 2` | [-2.0, 2.0] | 0.025 | 0.217545 | 0.217545 | 0.00305712 |
| 10 | `sin(x)/x + 0.1*x` | [0.5, 3.0] | 0.1 | 0.00839703 | 0.00827816 | 0.000348298 |
| 10 | `sin(x)/x + 0.1*x` | [0.5, 3.0] | 0.05 | 0.00422552 | 0.00419559 | 8.51147e-05 |
| 10 | `sin(x)/x + 0.1*x` | [0.5, 3.0] | 0.025 | 0.00211958 | 0.00211207 | 2.10668e-05 |

## Minimization
Methods: `golden` and `dichotomy`.  

### eps = 1e-4

| # | Function | Interval | golden Iterations | dichotomy Iterations | x_min | f_min |
|---|----------|----------|-------------------|-----------------------|-------|-------|
| 1 | `sin(x) + x^3` | [-2.0, 2.0] | 24 | 16 | -1.99997 | -8.90894 |
| 2 | `cos(x) - 0.5*x` | [-3.0, 3.0] | 24 | 16 | 2.99995 | -2.48996 |
| 3 | `tan(x) + x^2` | [-1.0, 1.0] | 22 | 15 | -0.999959 | -0.557349 |
| 4 | `1/(1 + x^2)` | [-3.0, 3.0] | 24 | 16 | -2.99995 | 0.100003 |
| 5 | `1/(x + 2) + x` | [-1.0, 2.0] | 23 | 15 | -0.999962 | 1.43453e-09 |
| 6 | `sqrt(x + 3) + x^2` | [-2.0, 2.0] | 24 | 16 | -0.148039 | 1.71069 |
| 7 | `exp(-x^2) + x` | [-2.0, 2.0] | 24 | 16 | -1.99997 | -1.98165 |
| 8 | `log(x + 3) + x^2` | [-2.0, 2.0] | 24 | 16 | -0.177124 | 1.06913 |
| 9 | `x^4 - 3*x^2 + 2` | [-2.0, 2.0] | 24 | 16 | -1.22473 | -0.25 |
| 10 | `sin(x)/x + 0.1*x` | [0.5, 3.0] | 23 | 15 | 2.99997 | 0.347048 |

### eps = 1e-6

| # | Function | Interval | golden Iterations | dichotomy Iterations | x_min | f_min |
|---|----------|----------|-------------------|-----------------------|-------|-------|
| 1 | `sin(x) + x^3` | [-2.0, 2.0] | 33 | 22 | -2 | -8.90929 |
| 2 | `cos(x) - 0.5*x` | [-3.0, 3.0] | 34 | 23 | 3 | -2.48999 |
| 3 | `tan(x) + x^2` | [-1.0, 1.0] | 32 | 21 | -1 | -0.557407 |
| 4 | `1/(1 + x^2)` | [-3.0, 3.0] | 34 | 23 | -3 | 0.1 |
| 5 | `1/(x + 2) + x` | [-1.0, 2.0] | 32 | 22 | -1 | 2.48246e-13 |
| 6 | `sqrt(x + 3) + x^2` | [-2.0, 2.0] | 33 | 22 | -0.148036 | 1.71069 |
| 7 | `exp(-x^2) + x` | [-2.0, 2.0] | 33 | 22 | -2 | -1.98168 |
| 8 | `log(x + 3) + x^2` | [-2.0, 2.0] | 33 | 22 | -0.177124 | 1.06913 |
| 9 | `x^4 - 3*x^2 + 2` | [-2.0, 2.0] | 33 | 22 | -1.22474 | -0.25 |
| 10 | `sin(x)/x + 0.1*x` | [0.5, 3.0] | 32 | 22 | 3 | 0.34704 |

### eps = 1e-8

| # | Function | Interval | golden Iterations | dichotomy Iterations | x_min | f_min |
|---|----------|----------|-------------------|-----------------------|-------|-------|
| 1 | `sin(x) + x^3` | [-2.0, 2.0] | 43 | 29 | -2 | -8.9093 |
| 2 | `cos(x) - 0.5*x` | [-3.0, 3.0] | 44 | 30 | 3 | -2.48999 |
| 3 | `tan(x) + x^2` | [-1.0, 1.0] | 41 | 28 | -1 | -0.557408 |
| 4 | `1/(1 + x^2)` | [-3.0, 3.0] | 44 | 30 | -3 | 0.1 |
| 5 | `1/(x + 2) + x` | [-1.0, 2.0] | 42 | 29 | -1 | 0 |
| 6 | `sqrt(x + 3) + x^2` | [-2.0, 2.0] | 43 | 29 | -0.148036 | 1.71069 |
| 7 | `exp(-x^2) + x` | [-2.0, 2.0] | 43 | 29 | -2 | -1.98168 |
| 8 | `log(x + 3) + x^2` | [-2.0, 2.0] | 43 | 29 | -0.177124 | 1.06913 |
| 9 | `x^4 - 3*x^2 + 2` | [-2.0, 2.0] | 43 | 29 | -1.22474 | -0.25 |
| 10 | `sin(x)/x + 0.1*x` | [0.5, 3.0] | 42 | 28 | 3 | 0.34704 |
