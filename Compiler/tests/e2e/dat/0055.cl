foo = func() { 5; }
bar = func() { foo(); }
buz = func() { foo() + bar(); }

print buz(); // 10

foo = func(x) { x; }
bar = func(x) { foo(x); }
buz = func(x) { foo(x) + bar(x); }

it = -5;
ite = 5;
while ((it += 1) < ite)
    print buz(it); // -8, -6, ... , 6 , 8