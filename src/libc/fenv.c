/*
 * fenv.c
 *
 *
 */

#include "include/fenv.h"

int fesetenv(const fenv_t *env_ptr) {
    __asm__ ("fldenv %d0"
    :
    : "m"(env_ptr)
    :
    );
    return 0;
}

int fegetenv(fenv_t *env_ptr) {
    __asm__ ("fstenv %d0"
    : "=m"(env_ptr)
    :
    :);
    return 0;
}

int feholdexcept(fenv_t *env_ptr) {
    fenv_t env;
    fegetenv(&env);
    *env_ptr = env;
    env.status &= 0xFFC0;
    fesetenv(&env);
    return 0;
}

int feupdateenv(const fenv_t *env_ptr) {
    fenv_t cur_env;
    fegetenv(&cur_env);
    unsigned short cur_excepts = env_ptr->status & 0xFFC0;
    fenv_t new_env = *env_ptr;
    new_env.status |= cur_excepts;
    fesetenv(&new_env);
    return 0;
}

int fegetround() {
    fenv_t env;
    fegetenv(&env);
    return (env.control & 0xC00) >> 10;
}

int fesetround(int round) {
    if (round >= 4 || round < 0)
        return -1;
    fenv_t env;
    fegetenv(&env);
    env.control &= 0xF3FF;
    env.control |= round << 10;
    fesetenv(&env);
    return 0;
}

int fegetexceptflag(fexcept_t *flag_ptr, int excepts) {
    fenv_t env;
    fegetenv(&env);
    int all_excepts = env.status & 0x3F;
    *flag_ptr = all_excepts & excepts;
    return 0;
}

int fesetexceptflag(const fexcept_t *flag_ptr, int excepts) {
    int raise_excepts = *flag_ptr & excepts;
    if (raise_excepts >= 0b1000000 || raise_excepts < 0)
        return -1;
    fenv_t env;
    fegetenv(&env);
    env.status &= 0xFFC0;
    env.status |= raise_excepts;
    fesetenv(&env);
    return 0;
}

int fetestexcept(int excepts) {
    fenv_t env;
    fegetenv(&env);
    int raised_excepts = env.status & 0x3F;
    return raised_excepts & excepts;
}

int feraiseexcept(int excepts) {
    if (excepts >= 0b1000000 || excepts < 0)
        return -1;
    fenv_t env;
    fegetenv(&env);
    env.status |= excepts;
    fesetenv(&env);
    return 0;
}

int feclearexcepts(int excepts) {
    if (!excepts)
        return 0;
    if (excepts >= 0b1000000 || excepts < 0)
        return -1;
    fenv_t env;
    fegetenv(&env);
    env.status &= ~excepts;
    fesetenv(&env);
    return 0;
}