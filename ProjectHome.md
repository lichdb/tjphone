下载 http://tjphone.googlecode.com/files/tjphone.exe-1.1.2.zip

a QT gui linphone on windows，support G729 & G723.1 codec.  support TLS/TCP/UDP
VS2010 Project

# 基于linphone-3.5.2。 #
# 使用QT重写了GUI部分的linphone sip软电话。增加基于Intel IPP实现的G729、G723.1编解码。源代码全部更新，加入了所有需要的静态lib。 #
## 1. 升级到linphone-3.5.2 ##
## 2. 支持了transfer、conference ##
## 3. 增加了对sip message im发送状态的处理。time out/404/200等 ##
## 4. 界面部分部分实现了i18n ##
## 5. 增加了命令 option。其中-l可以输入日志。-f 可以指定配置文件，方便启动多个实例进行测试 ##
## 6. 对界面部分进行了优化，取消了debug窗口内的现实（将来在其中显示sip 信令） ##

```
Usage:
  tjphone.exe [OPTION...] - An internet video phone using the standard SIP (rfc3261) protocol.

Help Options:
  -h, --help            Show help options

Application Options:
  --verbose             log to stdout some debug information while running.
  -l, --logfile         path to a file to write logs into.
  -f, --configfile      path to the tjpphonerc config file.
  --iconified           Start only in the system tray, do not show the main interface.
  -c, --call            address to call right now
  -a, --auto-answer     if set automatically answer incoming calls
  --workdir             Specifiy a working directory (should be the base of the installation, eg: c:\Program Files\Linphone)
```



Screenshots

![http://rock-lizard.org/files/tjphone-1.1.png](http://rock-lizard.org/files/tjphone-1.1.png)
![http://rock-lizard.org/files/tjphone-1.1-conference.png](http://rock-lizard.org/files/tjphone-1.1-conference.png)