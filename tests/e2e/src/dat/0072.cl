// test continue and break


// break
print "break:";
i = 0; n = 10;
while (1)
{
	print i;
	if (i >= n) break;
	i += 1;
}

// continue
print "continue:";
i = 0; n = 10;
while (i += 1)
{
	print i;
	if (i < n) continue;
	i = -1;
}

// nested loop
print "nested loop:";
i = 0; n = 10;
j = 0; m = 10;
while (1)
{
	while (1)
	{
		print i, " ", j;
		if (j >= m) break;
		j += 1;
	}
	j = 0;
	i += 1;
	if (i >= n) break;
	else continue;
}
