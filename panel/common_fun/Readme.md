使用工具qdbuscpp2xml从dbus.h生成XML文件；
qdbuscpp2xml -M ukuipanel_infomation.h -o org.ukui.panel.information.xml

B.使用工具qdbusxml2cpp从XML文件生成继承自QDBusAbstractAdaptor的类,供服务端使用
qdbusxml2cpp com.kylin.security.controller.filectrl.xml -i dbus.h -a dbus-adaptor

https://blog.51cto.com/9291927/21184
