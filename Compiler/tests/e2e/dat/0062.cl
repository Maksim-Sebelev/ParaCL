func() : govno { 666; }
foo = func(x) {
    bar = func() { x; /* using not global variable from older scope in function */ }
    bar();
}

arg = 666;
print "foo(", arg, ") = ", foo(arg);
