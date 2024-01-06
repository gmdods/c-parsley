#ifndef MACROS_H
#define MACROS_H

#define or_ :
#define break_case break

#define cast(type, expr) ((type) (expr))

#if __has_attribute(fallthrough)
#define fallthrough __attribute__((fallthrough))
#else
#define fallthrough ((void) 0)
#endif

#endif // !MACROS_H
