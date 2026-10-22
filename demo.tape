Output docs/minishell-demo.gif
Set FontSize 18
Set Width 1200
Set Height 700
Set TypingSpeed 200ms
Set Padding 20

# Run minishell
Type "./minishell" Enter
Sleep 2s

# A. pipes
Type "ls | wc -l" Enter
Sleep 3s
Type "cat /etc/passwd | grep root | cut -d: -f1" Enter
Sleep 3s

# B. redirections
Type 'echo "note" > /tmp/t.txt' Enter
Sleep 2s
Type "cat < /tmp/t.txt" Enter
Sleep 3s

# C. heredoc feeding a pipe
Type "cat << EOF | grep -v skip" Enter
Sleep 2s
Type "keep" Enter
Sleep 2s
Type "skip" Enter
Sleep 2s
Type "EOF" Enter
Sleep 3s

# D. builtin + variable
Type "export V=works" Enter
Sleep 2s
Type "echo $V" Enter
Sleep 3s

# E. exit status after a failure
Type "ls /nope" Enter
Sleep 2s
Type "echo $?" Enter
Sleep 3s

# F. signal: Ctrl+C handling
Type "sleep 300" Enter
Sleep 2s
Ctrl+C
Sleep 3s
Type "ps aux | grep sleep | grep -v grep" Enter
Sleep 3s

# Exit
Type "exit" Enter
Sleep 5s
