// function in function test

//-------------------------

foo = func(x)
{
    func(x, y) : bar { x + y + 1; }
    func(x) : bar { x + 1; }

    return bar(x, x) - bar(x);
}

arg = 1337666;
print "foo(", arg, ") = ", foo(arg);

//-------------------------
