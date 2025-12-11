echo "hello>me"
out: hello>me

echo "hello $name"
out: hellokiru

echo hello $name
out: hello kiru

echo "hello '$name'"
out: hello 'kiru

echo hello '$name'
out: hello $name

echo 'hello $name'
out: hello $name

echo 'hello' "$name"
out: hello kiru

echo 'hello'"$name"end
out: hellokiruend

echo $'name'
name
echo $"name"
$name

echo hello$name{me}
hellokiru{me}

// not necessary by subject but maybe print literal?
```sh
echo $-     (print shell flags)
echo $!     (print PID of last background job)
echo $@     (positional parameters / splits in args?)
echo $^     (invalid)
echo $=     (invalid)
```