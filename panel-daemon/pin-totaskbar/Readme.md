使用工具qdbuscpp2xml从dbus.h生成XML文件；
qdbuscpp2xml -M pintotaskbar.h -o org.ukui.panel.xml

B.使用工具qdbusxml2cpp从XML文件生成继承自QDBusAbstractAdaptor的类,供服务端使用
qdbusxml2cpp org.ukui.panel.xml -i pintotaskbar.h -a taskbar-dbus-adaptor

https://blog.51cto.com/9291927/21184
