# checkers使用说明
__本程序需在linux系统上运行。__

__原作者：@UBln, @621yuchen。 转载请注明出处。__

**1. 在终端输入make命令编译.c文件。**
**2. 在终端输入./server,运行服务器。**
**3. 另开一个终端输入./client，运行客户端。**

![1632132840(1)](https://user-images.githubusercontent.com/91058183/133986562-ff40385e-e1ab-4c0b-96f5-86b36529ba75.png)

**4. 根据提示选择注册账户、登陆账户或退出程序。**

![1632132760(1)](https://user-images.githubusercontent.com/91058183/133986412-81e4e495-17a8-434a-8adb-48862977c0f6.png)

**5. 根据信息提示选择您的对手，或者输入‘a’等待其他玩家的加入。**

![1632133187(1)](https://user-images.githubusercontent.com/91058183/133987242-020c8fb2-9cfb-4fde-9ee4-0e3d4bd3ca4e.jpg)

**6. 重复步骤3-步骤5，创建并运行另一个客户端。**

![1632133354(1)](https://user-images.githubusercontent.com/91058183/133987542-f4a8d33a-2c9f-4327-8bd7-16e2f62b1498.jpg)

**7. 此时第一个客户端会收到第二个客户端发来的连接请求，输入‘yes’后即可根据提示开始对战！**

![1632133650(1)](https://user-images.githubusercontent.com/91058183/133988120-6d27aff6-1258-40c9-acd1-16852b97e677.jpg)
---
![1632133717(1)](https://user-images.githubusercontent.com/91058183/133988245-290ea048-ffee-4e4f-87e3-3405ac6ec1b7.jpg)

# checkers游戏规则

1. __Normal move：__ 玩家在己方回合可以操纵己方颜色棋子向对角线的空格子移动一位。
2. __Attack move：__ 玩家在己方回合可以操纵己方颜色棋子跳过一个对角线方向的敌方棋子，并将其吃掉。
