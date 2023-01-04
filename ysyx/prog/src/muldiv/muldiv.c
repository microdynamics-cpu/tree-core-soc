#include <am.h>
#include <klib.h>
#include <klib-macros.h>

const int32_t MOD = 1e4 + 7;

#ifdef MEM_LOAD
#define P (998244353)
#define G (3)
#define inv_G (332748118)
#define N ((1 << 3) + 5)
int32_t n = 1, m = 2, rev[N];
int64_t A[N] = {1, 2}, B[N] = {1, 2, 1}, C[N] = {1, 4, 5, 2}, inv_lim;
#endif

int32_t gcd(int32_t x, int32_t y)
{
    return (y == 0) ? x : gcd(y, x % y);
}

typedef struct Matrix
{
    int32_t v11, v12, v21, v22;
} Matrix;

Matrix Matrix_mul(Matrix x, Matrix y)
{
    Matrix res = {(x.v11 * y.v11 + x.v12 * y.v21) % MOD,
                  (x.v11 * y.v12 + x.v12 * y.v22) % MOD,
                  (x.v21 * y.v11 + x.v22 * y.v21) % MOD,
                  (x.v21 * y.v12 + x.v22 * y.v22) % MOD};
    return res;
}

Matrix fst_exp(Matrix x, int32_t y)
{
    Matrix res = {1, 0, 0, 1};
    while (y)
    {
        if (y & 1)
        {
            res = Matrix_mul(res, x);
        }
        y >>= 1;
        x = Matrix_mul(x, x);
    }
    return res;
}

#ifdef MEM_LOAD
inline int64_t FP(int64_t x, int64_t k)
{
    int64_t t = 1;
    for (; k; k >>= 1, x = x * x % P)
        if (k & 1)
            t = t * x % P;
    return t;
}

void NTT(int64_t *a, int32_t lim, int32_t type)
{
    for (int32_t i = 0; i < lim; ++i)
        if (i < rev[i])
        {
            int64_t tmp = a[i];
            a[i] = a[rev[i]];
            a[rev[i]] = tmp;
        }

    for (int32_t i = 2; i <= lim; i <<= 1)
    {
        int32_t mid = i >> 1;
        int64_t Wn = FP(~type ? G : inv_G, (P - 1) / i), t;
        for (int32_t j = 0; j < lim; j += i)
        {
            int32_t w = 1;
            for (int32_t k = 0; k < mid; ++k, w = w * Wn % P)
                a[j + k + mid] = (a[j + k] - (t = w * a[j + k + mid] % P) + P) % P,
                          a[j + k] = (a[j + k] + t) % P;
        }
    }
    if (type == -1)
        for (int32_t i = 0; i < lim; ++i)
            a[i] = a[i] * inv_lim % P;
}
#endif

int32_t main()
{
    assert(gcd(33031, 182845) == 29);
    putstr("gcd test pass!\n");

    Matrix val = {0, 1, 1, 1};
    Matrix ans = fst_exp(val, 50 - 1);
    assert((ans.v11 + ans.v12) % MOD == 4803);

    ans = fst_exp(val, 100 - 1);
    assert((ans.v11 + ans.v12) % MOD == 6545);
    putstr("fibonacci test pass!\n");

#ifdef MEM_LOAD
    int32_t lim = 1, len = 0;
    while (lim <= n + m)
        lim <<= 1, ++len;
    inv_lim = FP(lim, P - 2);
    for (int32_t i = 1; i < lim; ++i)
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (len - 1));
    NTT(A, lim, 1), NTT(B, lim, 1);
    for (int32_t i = 0; i < lim; ++i)
        A[i] = A[i] * B[i] % P;
    NTT(A, lim, -1);
    for (int32_t i = 0; i <= n + m; ++i)
        assert(A[i] == C[i]);
    putstr("polynomial multiplication test pass!\n");
#endif
    putstr("all tests passed!!\n");
    return 0;
}
