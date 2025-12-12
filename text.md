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

// below are valid and should write all args to the file test.txt
echo hello > test.txt hello test test
echo > test.txt hello test test

// below should ignore the one after pipe (probably test is treated as an invlaid command)
echo > test.txt hello test | test

//errors
```sh
echo hello >>> test.txt
# out: bash: syntax error near unexpected token `>'
echo hello > > test.txt
# out: bash: syntax error near unexpected token `>'
echo hello > | test.txt
# out: bash: syntax error near unexpected token `|'
# out: zsh: parse error near `|'
echo hello >
# out: bash: syntax error near unexpected token `newline'
# out: zsh: parse error near `\n'
echo hello >>>| test.txt
# out: bash: syntax error near unexpected token `>|'
# out: zsh: parse error near `>|'
```

//treat below as errors alothough shell doesn't?
```sh
echo hello |
# out: pipe>

# below are noclobber but should i treat this as error?
echo hello >| test.txt
echo hello >>| test.txt
```

// not necessary by subject but maybe print literal?
```sh
echo $-     (print shell flags)
echo $!     (print PID of last background job)
echo $@     (positional parameters / splits in args?)
echo $^     (invalid)
echo $=     (invalid)
```