# python_demo.py — a Python input specimen for fkwu's Form-native Python
# compiler: def, recursion, conditional expressions, arithmetic,
# comparisons, logic and calls. Its compiled .fk returns 40949 on fkwu;
# observe/python-native-compiler-witness.bml reads it from here.
#
# Usage:
#   From the repository root, send {"source":"observe/fixtures/python_demo.py"}
#   to ./fkwu observe/kernel-trace-run.bml on stdin.

def fact(n):
    return 1 if n < 2 else n * fact(n - 1)

def fib(n):
    return n if n < 2 else fib(n - 1) + fib(n - 2)

def ackermann(m, n):
    return n + 1 if m == 0 else ackermann(m - 1, 1) if n == 0 else ackermann(m - 1, ackermann(m, n - 1))

def is_prime_helper(n, i):
    return True if i * i > n else False if n % i == 0 else is_prime_helper(n, i + 1)

def is_prime(n):
    return False if n < 2 else is_prime_helper(n, 2)

def count_primes_helper(n, limit, count):
    return count if n >= limit else count_primes_helper(n + 1, limit, count + (1 if is_prime(n) else 0))

def count_primes(limit):
    return count_primes_helper(2, limit, 0)

count_primes(30) + fact(8) + fib(15) + ackermann(2, 3)
