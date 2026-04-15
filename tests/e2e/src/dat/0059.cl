// testing global variables in functions

global = 10;

//-------------------------
foo = func() {
    return global;
}

print "foo() = ", foo();
print "global = ", global;

//-------------------------

foo = func() {
    global = 15;
    return global;
}

print "foo() = ", foo();
print "global = ", global;

//-------------------------

foo = func(x) {
    global += x;
    return global;
}

print "foo(5) = ", foo(5);
print "global = ", global;

//-------------------------

foo = func() {
    local = -666;
    global = local;
    return local;
}

print "foo() = ", foo();
print "global = ", global;

//-------------------------
