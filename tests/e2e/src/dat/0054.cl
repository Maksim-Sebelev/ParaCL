func(n) : fibonacci
{
    if (n <= 0) return 0;
    if (n == 1) return 1;

    return fibonacci(n - 1) + fibonacci(n - 2);
}

//  function overloading test
func(begin, end) : fibonacci
{
    while (begin <= end)
    {
        print "fib(", begin, ") = ", fibonacci(begin);
        begin += 1;
    }
    0; // just chill
}

fibonacci(0, 40);
