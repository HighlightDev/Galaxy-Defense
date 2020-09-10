function foo(host, a, b, c)
	
	_foo1(host, a * b)
	
	_foo2(host, a, b)

	functiontest(host, a, b)

	_foo3(host, a / b + c)
end

function functiontest(host, a, b)
	_foo1(host, a, b)
end