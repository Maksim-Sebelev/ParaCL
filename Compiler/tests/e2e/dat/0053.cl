// testing scope return value

t = { y = x = 15; x + y; };
print t; // 30

x = 666;
y = 228;

t = { y = x = 15; x + y; };
print t; // 30


x = 666;
y = -666;

t = { x + y; };
print  t; // 0
