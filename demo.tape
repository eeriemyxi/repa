Set Theme "GruvboxDark"
Require "repa"
Output demo.gif

Set FontFamily "JetBrainsMono Nerd Font Mono,JetBrainsMono NFM"
Set FontSize 16
Set Width 1280
Set Height 720

Type "repa txts/shakespeare.txt"
Sleep 500ms
Enter
Sleep 5s

Type 'echo "Hello Repa, what do you do?" | repa - txts/shakespeare.txt -t 15'
Sleep 500ms
Enter
Sleep 8s

Type "repa - txts/ipsum.txt < txts/shakespeare.txt"
Sleep 500ms
Enter
Sleep 7s
