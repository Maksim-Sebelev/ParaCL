false = 0;
true = 1;

func(n) : is_prime
{
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    is_prime_help = func(n)
    {
        potential_divider = 3;
        while (potential_divider * potential_divider <= n)
        {
            if (n % potential_divider == 0) return false;
            potential_divider += 2;
        }
        return true;
    }

    return is_prime_help(n);   
}

func(n) : show_value_is_prime
{
    if (is_prime(n)) print n;
    return 0;
}

func(begin, end) : show_range_is_prime
{
    print "all prime numbers in range(",begin,", ",end,"):";
    while (begin < end)
    {
        show_value_is_prime(begin); 
        begin += 1;
    }

    return 0;
}

show_range_is_prime(1, 100);

