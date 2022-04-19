```
此文档主要介绍任务栏的接口和主要功能实现逻辑。
```

##任务栏介绍
任务栏采用插件加载机制，主要包含startbar(开始菜单)，taskbar(窗口切换区)，statusnotifier(托盘区)，calendar(日历)，nightmode(夜间模式)，showdesktop(显示桌面)这6个插件，下面依次介绍每个插件的功能：
 * startbar
	此插件包含开始菜单按钮和显示任务试图按钮，当这两个按钮被左键点击时，任务栏会调用并执行开始菜单或多任务试图所对应的二进制文件，实现相应的功能。
	在开始菜单按钮上右键可以看到任务栏提供的一些操作（1）用户操作，包括锁屏和注销；（2）休眠或睡眠，依据机器是否支持休眠操作决定休眠选项是否可选；（3）电源，包括重启，定时关机和关机操作。
	显示任务试图按钮是否展示对应任务栏空白处右键菜单的“显示任务试图按钮”项是否勾选
* taskbar
	此区域主要用来展示用户固定到任务栏的应用，以及所有已经打开的应用的图标，通过图标的点击可实现窗口切换。taskbar提供的接口主要有：
	（1）将应用固定到任务栏：此接口主要提供给开始菜单和搜索将应用固定到任务栏使用，它们向任务栏；
	（2）从任务栏取消固定：此接口主要提供给开始菜单和搜索将应用固定到任务栏使用；
	（3）应用分组显示接口：此接口主要实现让同一应用的图标显示在一起，每个应用都有一个组名，组名相同的则将图标放在一起显示，此接口主要由panel-daemon后台服务进程供，当有新窗口打开时，首先检测是否存在已经打开的此应用，如有则作为一组图标显示，若没有则在任务栏增加新的按钮。
* statusnotifier
	托盘区的功能是为应用创建一个快捷操作按钮，可以执行相关操作或显示应用状态。应用可通过SNI协议将图标注册到托盘区域。
* calendar
	日历插件主要用于显示日历界面和时间，左键单击可以调出日历界面，右键单击可进入控制面板的时间和日期的设置界面。
* nightmode
	夜间模式按钮是否展示对应着右键点击任务栏空白处出现的右键菜单中“显示夜间模式按钮”项是否勾选。
* showdesktop
	显示桌面按钮的功能是在当前窗口和桌面之间进行切换。

##任务栏的主要功能
* 调整位置和调整大小
	任务栏有上下左右四个位置可供选择，每个位置有大中小三个尺寸，可通过右键点击任务栏空白处调出右键菜单进行调节，也可通过gsettings命令进行调节。
	获取任务栏当前位置命令：gsettings get org.ukui.panel.settings panelposition ，其中0代表下，1代表上，2代表左，3代表右。
	调整位置命令：gsettings set org.ukui.panel.settings panelposition 1，表示将任务栏设置为上方。
	获取任务栏当前大小命名：gsettings get org.ukui.panel.settings panelsize，返回值为任务栏的高度像素值，小尺寸为46，中尺寸为70，大尺寸为92。
	调整大小命令：gsettings set org.ukui.panel.settings panelsize 92，表示将任务栏设置为大尺寸，设置的数值可以为任意值
	
* 隐藏任务栏
	点击任务栏右键菜单中的隐藏任务栏选项，任务栏进入隐藏状态，留有四像素边距，鼠标进入此边距时可以唤醒任务栏，鼠标离开后又会进入隐藏状态。此状态存储于任务栏配置文件中的hidable中。

* 锁定任务栏
	点击任务栏右键菜单中的锁定任务栏选项后，任务栏将不能被修改位置和大小，不能设置隐藏。此状态存储于任务栏配置文件中的lockPanel中。

##配置文件与用户设置
*  配置文件作用	
	根目录任务栏配置文件地址：/usr/share/ukui/panel.conf
	用户目录任务栏配置文件地址：~/.config/ukui/panel.conf
	它们的作用分别是：
	（1）根目录下的配置文件决定了任务栏需要加载哪些插件，即/usr/share/ukui/panel.conf文件中 [panel1] 字段的plugins的值；
	（2）用户目录下的配置文件保存了用户的设置，包括任务栏固定了哪些应用图标，任务栏的位置和大小，隐藏和显示，以及托盘区哪些应用显示在收纳栏里的设置；
	（3）每次安装任务栏的包或者是进行任务栏升级，更新的只有根目录下的配置文件，而不会修改用户目录下的配置文件，避免了装包或升级导致的用户配置消失的问题。
*	配置文件内容
	[panel1]
alignment=-1                         【任务栏位置居左，0代表居中，1代表居右】
animation-duration=100  【隐藏任务栏的动画时间为100ms】
desktop=0                               【任务栏位于0桌面】
hidable=false						 【任务栏是否隐藏，false代表不隐藏，true代表隐藏】
lineCount=1                           【任务栏单行显示】
lockPanel=false                    【任务栏是否锁定，false代表不锁定，true代表锁定】
plugins=startbar,taskbar,statusnotifier,calendar, nightmode,showdesktop     【任务栏所需要加载的插件，注：此字段从根目录下的配置文件中读取】
position=Bottom                 【任务栏的位置，Bottom代表下，Top代表上，Left代表左， Right代表右】
reserve-space=true
show-delay=0                       【延迟显示的时间】
visible-margin=true
width=100                              【任务栏的长度，数值代表的是占屏幕长度的百分比】
width-percent=true           【任务栏长度是否以百分比的形式显示】
其余字段为各个插件的显示位置和内容，如：3.1以上版本的任务栏的[taskbar]字段包含了固定在任务栏的应用有哪些，默认的是文件管理器，奇安信浏览器，WPS和软件商店
[statusnotifier]字段包含了托盘应用哪些放在收纳栏里面（hideApp），哪些放在收纳栏外边（showApp）。

##编译
	在编译任务栏代码前需要安装debian/control文件中写到的编译依赖，安装完所有依赖后，新建一个build文件夹，在build目录下执行cmake .. && make -j12 && sudo make install进行编译，编译通过后执行./panel/ukui-panel运行。

##调试
	任务栏采用了ukui-log4qt模块的日志功能，输出的日志在用户目录下的.log文件夹内的ukui-panel.log中，如需调试可在终端输入tail -f ukui-panel.log命令，可实时查看任务栏的日志输出

##运行
任务栏的进程共有4个：
ukui-panel    任务栏的GUI界面
panel-daemon   任务栏后台进程，负责监听任务栏添加和删除应用，以及应用的分组显示
sni-daemon   SNI协议后台进程，负责托盘区图标的注册
sni-xembed-proxy   将使用X协议注册的托盘图标转换成SNI协议
任务栏的进程均为开机自启动进程

##任务栏提供的DBUS接口
* 服务：com.ukui.panel.desktop
   路径：/
   接口：com.ukui.panel.desktop
   方法：AddToTaskbar (String desktop) ↦ (Boolean arg_0)   //添加到任务栏
   方法：CheckIfExist (String desktop) ↦ (Boolean arg_0)   //检测任务栏是否已经存在此应用
   方法：RemoveFromTaskbar (String desktop) ↦ (Boolean arg_0)  //从任务栏移除

* 服务：org.ukui.panel.daemon
   路径：/convert/desktopwid
   接口：org.ukui.panel.daemon
   方法：WIDToDesktop (Int32 id) ↦ (String arg_0)   //根据窗口id找到其对应的desktop文件，进行分组显示




	




	
	
