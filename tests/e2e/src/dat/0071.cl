a = 10;
print a;
{
a = func() { 11; }
print a();
a = 777;
}
print a;
func() : foo { return a; }
print foo();

a = 12;
print a;

