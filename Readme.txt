VS2008项目
build 环境：vs2008(VC++)、windows SDK v6.1

更新日志：

2011-08-08：
添加了基于intel ipp 5.1 的g723.1支持。linphone-3.4.1
发现bug：
1. linphone-3.3.1 当被叫不支持所有编码返回4**时，主叫方crash（linux平台上，windows平台还没有测试）
2. 在windows平台，当呼叫一个不存在的被叫时，在发起呼叫后，理解点击quit推出linphone，会出现内存无效访问的情况。（已经uninit linphonecore之后，呼叫超时继续处理callfailure时出错）


2011-08-04：
添加了基于intel ipp 5.1 的g729支持。linphone-3.4.1

2010-09-08：
完成在vs2008环境的build