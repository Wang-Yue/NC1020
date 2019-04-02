# NC1020

A 文曲星 Simulator that you can play 黄金英雄坛说. 

This is a cross platform port where SDL2 is the only dependency.

Key binding can be found in `main.cpp`.

This keybinding is optimized for the GameShell console offered from ClockworkPi.

The project is already fully functional and I have no plan to add additional features.

With that said, PRs are very welcomed --- if you have great new features implemented, feel free to let me know and I'll be happy to merge your code.



Tips: 

1. You can also install it on your PC, and mount your GameShell as sshfs/NFS/samba drive so you can access the flash and state files on both machines. With some slight modification to the code you can 打坐 in demand on your PC with at least 200X speed.

2. The rom I found on the internet seems too have issue saving your 物品 in the game. So just remember don't exit the game along the way. Instead on relying on the game's functionality of saving to flash, you can just simply relying on emulator's `Shift+Menu` to save the entire state instead. 

3. You can slightly modify the code to enable key bindings to do `git commit` and `git reset` for the flash and state files, so you can save your playing history and quickly jump among all previous stages.

![GameShell running NC1020](/GameShell.png?raw=true)
