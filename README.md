# 基于Socket的文件传输系统
开发环境：C++，Socket，VsCode

实现的基本功能：

* browse：客户端浏览服务端的文件；
* upload：客户端上传文件至服务端；
* download：客户端从服务端下载文件；

基本流程图如下：

服务端挂起，建立socket，调用bind绑定端口，然后进入监听模式，调用accept阻塞，当客户端发起connect请求时，三次握手后建立通信。客户端终端会弹出功能选择菜单，用户输入相关选项后进行浏览与文件传输。

![lct](images/lct.png)

#### 运行说明

代码中使用的是本地环回地址（127.0.0.1）测试，远程使用自行更改ip与端口。

测试时，先挂起服务端，进入监听状态，再运行客户端，连接成功后客户端会弹出菜单。对于客户端，上传与下载的文件位于根目录，服务端上传与下载的文件位于file文件夹内。

#### 常见报错

![error](images/error.png)

由于动态库`ws2_32.lib`链接出错导致，在命令行中手动链接` -lwsock32 `即可：

客户端：

```shell
New-Item build -Type Directory -Force |Out-Null ; if ($?) { g++ "kehuduan.cpp"-lwsock32 -o "build/kehuduan.exe" } ; if ($?) { .\build\"kehuduan.exe" } 
```

服务端：

```shell
New-Item build -Type Directory -Force |Out-Null ; if ($?) { g++ "fuwuduan.cpp"-lwsock32 -o "build/fuwuduan.exe" } ; if ($?) { .\build\"fuwuduan.exe" } 
```



参考：

[(12条消息) 写了一个文件传输软件_涛歌依旧的博客-CSDN博客](https://blog.csdn.net/stpeace/article/details/124087581)

