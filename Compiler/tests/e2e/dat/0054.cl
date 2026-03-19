fib = func(n)
{
    if (n <= 0) return 0;
    if (n == 1) return 1;

    return fib(n - 1) + fib(n - 2);
}

// function overloading test
fib = func(begin, end)
{
    while (begin <= end)
    {
        print "fib(", begin, ") = ", fib(begin);
        begin += 1;
    }
    0;
}

fib(0, 40);
